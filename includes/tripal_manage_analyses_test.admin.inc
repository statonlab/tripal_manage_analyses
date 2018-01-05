<?php

function tripal_manage_analyses_test_form($form, &$form_state) {

  $sql = "SELECT * FROM {biomaterial}";
  $results = chado_query($sql);
  foreach ($results as $result) {

    if ($result->dbxref_id) {
      $sql = "SELECT DB.name, DBX.accession FROM {dbxref} DBX
        INNER JOIN {db} DB ON  DBX.db_id = db.db_id
        WHERE DBX.dbxref_id = :dbxref_id";

      $dbxref = chado_query($sql, [":dbxref_id" => $result->dbxref_id])->fetchObject();
      if ($dbxref->name == "NCBI BioSample") {
        $biomaterial_id = $result->biomaterial_id;
        $ncbi_accession = $dbxref->accession;

        tripal_associate_dbxref('biomaterial', $biomaterial_id, [
          'accession' => $ncbi_accession,
          'db_name' => 'NCBI BioSample',
        ]);
      }
    }
  }

  $cv_ids = [
    'analysis_property',
    'tripal',
    'tripal_analysis',
  ];

  foreach ($cv_ids as $name) {
    $cvterm = tripal_get_cvterm(
      [
        'name' => 'analysis_unigene_name',
        'cv_id' => ['name' => $name],
      ]);

    if ($cvterm) {
      break;
    }
  }

  $analysis_unigene_cvt_id = $cvterm->cvterm_id;

  //select all analysis unigenes, and their feature type IDs

  $sql = "SELECT DISTINCT  A.analysis_id, F.type_id  FROM {analysis} A
        INNER JOIN {analysisprop} AP  ON A.analysis_id = AP.analysis_id
        INNER JOIN {cvterm} CVT ON AP.type_id = CVT.cvterm_id
        INNER JOIN {analysisfeature} AF ON A.analysis_id = AF.analysis_id
        INNER JOIN {feature} F ON AF.feature_id = F.feature_id
        WHERE CVT.cvterm_id = :cvterm_id ";

  //Also get the terms for mrna_contig and mrna, to check against the feature types
  $cvterm = tripal_get_cvterm([
    'name' => 'mRNA_contig',
    'cv_id' => ['name' => 'sequence',],
  ]);
  $mrna_contig_id = $cvterm->cvterm_id;
  $cvterm = tripal_get_cvterm([
    'name' => 'mRNA',
    'cv_id' => ['name' => 'sequence'],
  ]);
  $mrna_id = $cvterm->cvterm_id;

  $results = chado_query($sql, [":cvterm_id" => $analysis_unigene_cvt_id]);
  foreach ($results as $result) {
    $property_value = NULL;

    $analysis_id = $result->analysis_id;

    if ($result->type_id == $mrna_contig_id) {
      $property_value = 'tripal_analysis_unigene';
    }
    if ($result->type_id == $mrna_id) {
      $property_value = 'genome_assembly';
    }

    $record = [
      'table' => 'analysis',
      'id' => $analysis_id,
    ];

    if ($property_value) {//if we had a mRNA or mRNA_contig, set the analysis type property based on that

      $analysis_type_cvterm = tripal_get_cvterm([
        'name' => 'analysis_type',
        'cv_id' => ['name' => 'tripal_analysis'],
      ]);

      if (!$analysis_type_cvterm) {
        $analysis_type_cvterm = tripal_insert_cvterm([
          'name' => 'analysis_type',
          'def' => 'The type of analysis was performed. This value is automatically set by each Tripal Analysis module and should be equal to the module name (e.g. tripal_analysis_blast, tripal_analysis_go).',
          'cv_name' => 'tripal_analysis',
          'db_name' => 'analysis_type',
        ]);
      }

      $property = [
        'type_name' => $analysis_type_cvterm->name,
        'cv_name' => 'tripal_analysis',
        'value' => $property_value,
      ];
      $options = [
        'insert_if_missing' => TRUE,
      ];
      (chado_update_property($record, $property, $options));
    }
  }


  ///PART 2

  //Determine which bio_data table houses analyses
  $term = tripal_load_term_entity([
    'vocabulary' => 'operation',
    'accession' => '2945',
  ]);
  $bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
  $analysis_bundle_name = 'public.chado_' . $bundle->name;
  //create Transcriptome and Genome bundles
  $error = '';

  $analysis_type_cvterm = tripal_get_cvterm([
    'name' => 'analysis_type',
    'cv_id' => ['name' => 'tripal_analysis'],
  ]);

  $analysis_type_cvterm_id = $analysis_type_cvterm->cvterm_id;

  //insert terms
  tripal_insert_cvterm([
    'name' => 'Transcriptome Assembly',
    'def' => '',
    'accession' => '3258',
    'cv_name' => 'operation',
    'db_name' => 'EDAM',
  ]);
  tripal_insert_cvterm([
    'name' => 'Transcriptome Assembly',
    'def' => '',
    'accession' => '0525',
    'cv_name' => 'operation',
    'db_name' => 'EDAM',
  ]);

  //define two new bundles, and populate them with *Tripal 3 analyses* That match this type via the analysis_type prop
  $args_array = [
    [
      'vocabulary' => 'operation',
      'accession' => '3258',
      'term_name' => 'Transcriptome assembly',
      'storage_args' => [
        'data_table' => 'analysis',
        'type_linker_table' => 'analysisprop',
        'type_value' => 'tripal_analysis_unigene',
        'type_column' => 'type_id',
        'type_id' => $analysis_type_cvterm_id,
      ],
    ],
    [
      'vocabulary' => 'operation',
      'accession' => '0525',
      'term_name' => 'Genome assembly',
      'storage_args' => [
        'data_table' => 'analysis',
        'type_linker_table' => 'analysisprop',
        'type_value' => 'genome_assembly',
        'type_column' => 'type_id',
        'type_id' => $analysis_type_cvterm_id,
      ],
    ],
  ];
  foreach ($args_array as $args) {

    $type_value = $args['storage_args']['type_value'];

    $term = tripal_load_term_entity([
      'vocabulary' => $args["vocabulary"],
      'accession' => $args["accession"],
    ]);
    if ($term) {
      $bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
    }
    if (!$term or !$bundle) {
      if (!tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
      }
    }

    $destination_term = $bundle->term_id;

    $destination_table = "public.chado_" . $bundle->name;

    //  Copy the plain analysis entities to the new bundle type

    dpm('Table: ' . $destination_table);
    dpm('Bundle: ' . $analysis_bundle_name);

    $sql = "INSERT INTO " . $destination_table . " (entity_id, record_id)
        SELECT A.analysis_id AS record_id, AB.entity_id AS entity_id FROM chado.analysis A
        INNER JOIN chado.analysisprop AP  ON A.analysis_id = AP.analysis_id
        INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
        INNER JOIN  " . $analysis_bundle_name . " AB ON A.analysis_id = AB.record_id
        WHERE CVT.cvterm_id = :cvterm_id
        AND AP.value = :analysis_prop_value";

    $results = db_query($sql, [
      ":cvterm_id" => $analysis_type_cvterm_id,
      ":analysis_prop_value" => $type_value,
    ]);

    //Update the tripal_entity table


    //TODO:  THIS IS NOT WORKING YET!
    //    $sql = "UPDATE public.tripal_entity AS TE SET bundle = :destination_bundle, term_id = :destination_term
    //    WHERE TE.id IN
    //    (SELECT AB.entity_id AS entity_id FROM chado.analysis A
    //        INNER JOIN chado.analysisprop AP  ON A.analysis_id = AP.analysis_id
    //        INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
    //        INNER JOIN  " . $analysis_bundle_name . " AB ON A.analysis_id = AB.record_id
    //        WHERE CVT.cvterm_id = :cvterm_id
    //        AND AP.value = :analysis_prop_value )
    //    ";

    $sql = "UPDATE public.tripal_entity AS TE SET bundle = :destination_bundle, term_id = :destination_term
      WHERE TE.id  IN (SELECT AB.entity_id AS entity_id FROM chado.analysis A
       INNER JOIN chado.analysisprop AP  ON A.analysis_id = AP.analysis_id
        INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
        INNER JOIN  " . $analysis_bundle_name . " AB ON A.analysis_id = AB.record_id
        WHERE CVT.cvterm_id = :cvterm_id
        AND AP.value = :analysis_prop_value )";

    $params = [
      ":cvterm_id" => $analysis_type_cvterm_id,
      ":analysis_prop_value" => $type_value,
      ':destination_term' => $destination_term,
      ":destination_bundle" => $bundle->name,
    ];

    $results = db_query($sql, $params);
    foreach ($results as $result) {
      dpm($result);
    }

    // Delete the old entities, but only if they are present in both the destination table
    $sql = "DELETE  FROM " . $analysis_bundle_name . " AB
    WHERE AB.record_id IN
    (SELECT A.analysis_id AS record_id
    FROM chado.analysis A
    INNER JOIN chado.analysisprop AP  ON A.analysis_id = AP.analysis_id
    INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
    INNER JOIN " . $destination_table . " AS DT ON AB.record_id = DT.record_id
    WHERE CVT.cvterm_id = :cvterm_id
    AND AP.value = :analysis_prop_value)";

    $results = db_query($sql, [
      ":cvterm_id" => $analysis_type_cvterm_id,
      ":analysis_prop_value" => $type_value,
    ]);
  }


  test_organism_analysis();


  return $form;
}


function test_organism_analysis() {

  dpm("yo");


}