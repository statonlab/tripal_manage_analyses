<?php

function tripal_manage_analyses_test_form($form, &$form_state) {
  ///PART 2

  //Determine which bio_data table houses analyses
  $term = tripal_load_term_entity([
    'vocabulary' => 'operation',
    'accession' => '2945',
  ]);
  $analysis_bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
  dpm($analysis_bundle);

  return $form;
}
