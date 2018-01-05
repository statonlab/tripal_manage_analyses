<?php

function tripal_manage_analyses_test_form($form, &$form_state) {
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
        SELECT AB.entity_id AS entity_id, A.analysis_id AS record_id FROM chado.analysis A
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