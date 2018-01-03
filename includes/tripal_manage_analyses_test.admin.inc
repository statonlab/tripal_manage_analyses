<?php

function tripal_manage_analyses_test_form($form, &$form_state){



  $cvterm = tripal_get_cvterm(
    [
      'name' => 'analysis_unigene_name',
      'cv_id' => ['name' => 'tripal'],
    ]);

  $analysis_unigene_cvt_id = $cvterm->cvterm_id;

dpm($cvterm);

  dpm($analysis_unigene_cvt_id);

  print("aucvtid: " . $analysis_unigene_cvt_id);

  $sql = "SELECT DISTINCT  A.analysis_id, F.type_id  FROM {analysis} A
        INNER JOIN {analysisprop} AP  ON A.analysis_id = AP.analysis_id
        INNER JOIN {cvterm} CVT ON AP.type_id = CVT.cvterm_id
        INNER JOIN {analysisfeature} AF ON A.analysis_id = AF.analysis_id
        INNER JOIN {feature} F ON AF.feature_id = F.feature_id
        WHERE CVT.cvterm_id = :cvterm_id ";

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

  $analysis_mappings = [];

  $results = chado_query($sql, [":cvterm_id" => $analysis_unigene_cvt_id]);
  foreach ($results as $result) {
    $analysis_id = $analysis_mappings[$result["analysis_id"]];

    if ($result["type_id"] == $mrna_contig_id) {
      $property_value = 'tripal_analysis_unigene';
    }
    if ($result["type_id"] == $mrna_id) {
      $property_value = 'genome_assembly';
    }

    if ($property_value) {
      $record = [
        'table' => 'analysis',
        'id' => $analysis_id,
      ];

      $property = [
        'type_name' => $analysis_unigene_cvt_id,
        'cv_name' => 'tripal',
        'value' => $property_value,
      ];
      $options = [
        'update_if_present' => TRUE,
      ];
      chado_update_property($record, $property, $options);
    }
  }

}