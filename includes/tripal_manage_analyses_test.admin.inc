<?php
//
//function tripal_manage_analyses_test_form($form, &$form_state) {
//  ///PART 2
//
//  //Determine which bio_data table houses analyses
//  $term = tripal_load_term_entity([
//    'vocabulary' => 'operation',
//    'accession' => '2945',
//  ]);
//
//  $analysis_bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
//  //create Transcriptome and Genome bundles
//  $error = '';
//
//  $analysis_type_cvterm = tripal_get_cvterm([
//    'name' => 'analysis_type',
//    'cv_id' => ['name' => 'tripal_analysis'],
//  ]);
//
//  $analysis_type_cvterm_id = $analysis_type_cvterm->cvterm_id;
//
//  //double check bundle is defined
//  $args = [
//    'vocabulary' => 'operation',
//    'accession' => '0525',
//    'term_name' => 'Genome assembly',
//    'storage_args' => [
//      'data_table' => 'analysis',
//      'type_linker_table' => 'analysisprop',
//      'type_value' => 'genome_assembly',
//      'type_column' => 'type_id',
//      'type_id' => $analysis_type_cvterm_id,
//    ],
//  ];
//
//
//  $type_value = $args['storage_args']['type_value'];
//
//  $term = tripal_load_term_entity([
//    'vocabulary' => $args["vocabulary"],
//    'accession' => $args["accession"],
//  ]);
//  if ($term) {
//    $bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
//  }
//  if (!$term or !$bundle) {
//    if (!tripal_create_bundle($args, $error)) {
//      throw new Exception($error['!message']);
//    }
//  }
//
//  //load assembly bundle now that we know its created
//  $term = tripal_load_term_entity([
//    'vocabulary' => 'operation',
//    'accession' => '0525',
//  ]);
//
//  $genome_assembly_bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
//
//
//  $analysis_type_term = tripal_get_cvterm([
//    'name' => 'analysis_type',
//    'cv_id' => ['name' => 'tripal_analysis'],
//  ]);
//
//  $descriminating_value = "genome_assembly";
//
//
//
//  dpm($analysis_bundle);
//  dpm($genome_assembly_bundle);
//  dpm($analysis_type_term);
//  dpm($descriminating_value);
//
//  tripal_manage_analyses_convert_entity($analysis_bundle, $genome_assembly_bundle, $analysis_type_term, $descriminating_value);
//  return $form;
//}
//
//
//
//function tripal_manage_analyses_convert_entity($source_bundle, $destination_bundle, $descriminating_cvterm, $descriminating_prop_value) {
//
//dpm($descriminating_cvterm);
//
//  $descriminating_type_cvterm_id = $descriminating_cvterm->cvterm_id;
//
//  $source_bundle_table = "public.chado_" . $source_bundle->name;
//
//  $destination_term = $destination_bundle->term_id;
//  $destination_table = "public.chado_" . $destination_bundle->name;
//
//  print ("\nconverting " . $source_bundle_table . "to " . $destination_table . "\n");
//
//
//  $chado_base_table = "chado." . $source_bundle->data_table;
//
//
//  $chado_prop_table = $chado_base_table . "prop";
//  //TODO: THIS FUNCTION IS NOT QUITE REFACTORED: need vairables for the column selection too.  ie analysis_id
//
//
//
//  //  Copy the plain analysis entities to the new bundle type
//
//  $sql = "INSERT INTO " . $destination_table . " (entity_id, record_id)
//        SELECT AB.entity_id AS entity_id, A.analysis_id AS record_id FROM " . $chado_base_table . "  A
//        INNER JOIN  " . $chado_prop_table .  " AP  ON A.analysis_id = AP.analysis_id
//        INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
//        INNER JOIN  " . $source_bundle_table . " AB ON A.analysis_id = AB.record_id
//        WHERE CVT.cvterm_id = :cvterm_id
//        AND AP.value = :descrim_prop_value";
//
//  dpm($sql);
//  $results = db_query($sql, [
//    ":cvterm_id" => $descriminating_type_cvterm_id,
//    ":descrim_prop_value" => $descriminating_prop_value,
//  ]);
//
//
//  //Update the tripal_entity table
//
//  $sql = "UPDATE public.tripal_entity AS TE SET bundle = :destination_bundle, term_id = :destination_term
//      WHERE TE.id  IN (SELECT AB.entity_id AS entity_id FROM chado.analysis A
//       INNER JOIN " . $chado_prop_table .  " AP  ON A.analysis_id = AP.analysis_id
//        INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
//        INNER JOIN  " . $source_bundle_table . " AB ON A.analysis_id = AB.record_id
//        WHERE CVT.cvterm_id = :cvterm_id
//        AND AP.value = :descrim_prop_value )";
//
//  $params = [
//    ":cvterm_id" => $descriminating_type_cvterm_id,
//    ":descrim_prop_value" => $descriminating_prop_value,
//    ':destination_term' => $destination_term,
//    ":destination_bundle" => $destination_bundle->name,
//  ];
//
//  $results = db_query($sql, $params);
//
//  // Delete the old entities, but only if they are present in both the destination table
//  $sql = "DELETE  FROM " . $source_bundle_table . " AB
//    WHERE AB.record_id IN
//    (SELECT A.analysis_id AS record_id
//    FROM chado.analysis A
//    INNER JOIN " . $chado_prop_table .  " AP  ON A.analysis_id = AP.analysis_id
//    INNER JOIN chado.cvterm CVT ON AP.type_id = CVT.cvterm_id
//    INNER JOIN " . $destination_table . " AS DT ON AB.record_id = DT.record_id
//    WHERE CVT.cvterm_id = :cvterm_id
//    AND AP.value = :descrim_prop_value)";
//
//  $results = db_query($sql, [
//    ":cvterm_id" => $descriminating_type_cvterm_id,
//    ":descrim_prop_value" => $descriminating_prop_value,
//  ]);
//}