<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class data__sequence_featuresTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  public function testHelperWorks() {

    $records = $this->create_test_features();

    $entity = $records['entity_id'];

    

  }

  private function create_test_features() {


    $gene = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000704'])->cvterm_id]);
    $mrna = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000234'])->cvterm_id]);
    $cds = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000316'])->cvterm_id]);
    $protein = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000104'])->cvterm_id]);

    $this->associate_features($gene, $mrna);
    $this->associate_features($mrna, $cds);
    $this->associate_features($mrna, $protein);

    // publish the gene feature.

    $this->publish('feature');

    // Find this entity

    $entity_id = chado_get_record_entity_by_table('feature', $gene->feature_id);

    return [
      'entity_id' => $entity_id,
      'gene' => $gene,
      'mrna' => $mrna,
      'cds' => $cds,
      'protein' => $protein,
    ];
  }

  private function associate_features($object, $subject) {
    $values = [
      'object_id' => $object->feature_id,
      'subject_id' => $subject->feature_id,
      'type_id' => [
        'cv_id' => [
          'name' => 'sequence',
        ],
        'name' => 'derives_from',
      ],
      'rank' => 0,
    ];
    $success = chado_insert_record('feature_relationship', $values);
    return $success;
  }
}
