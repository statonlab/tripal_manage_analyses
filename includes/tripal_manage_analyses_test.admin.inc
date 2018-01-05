<?php

function tripal_manage_analyses_test_form($form, &$form_state) {
  ///PART 2

  //Determine which bio_data table houses analyses
  $term = tripal_load_term_entity([
    'vocabulary' => 'operation',
    'accession' => '2945',
  ]);

  $analysis_bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
  //create Transcriptome and Genome bundles
  $error = '';

  $analysis_type_cvterm = tripal_get_cvterm([
    'name' => 'analysis_type',
    'cv_id' => ['name' => 'tripal_analysis'],
  ]);

  $analysis_type_cvterm_id = $analysis_type_cvterm->cvterm_id;

  //double check bundle is defined
  $args = [
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
  ];


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

  //load assembly bundle now that we know its created
  $term = tripal_load_term_entity([
    'vocabulary' => 'operation',
    'accession' => '0525',
  ]);

  $genome_assembly_bundle = tripal_load_bundle_entity(['term_id' => $term->id]);


//  $analysis_type_term = tripal_get_cvterm([
//    'name' => 'analysis_type',
//    'cv_id' => ['name' => 'tripal'],
//  ]);
//
//  $descriminating_value = "genome_assembly";

 // tripal_manage_analyses_convert_entity($analysis_bundle, $genome_assembly_bundle, $analysis_type_term, $descriminating_value);
  return $form;
}
