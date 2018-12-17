<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('php', 'tripal_chado', '../tests/TripalFieldTestHelper');
/**
 *
 */
class data__sequence_featuresTest extends TripalTestCase {

  /**
 * Uncomment to auto start and rollback db transactions per test method.
 */
  use DBTransaction;

  private static $records = [];

  /**
   *
   */
  public function testHelperWorks() {

    $records = $this->create_test_features();

    $entity = $records['entity'];
    $bundle_name = $records['bundle_name'];
    $field_name = 'data__sequence_features';
    $formatter_name = 'data__sequence_features_formatter';

    // Initialize the widget class via the TripalFieldTestHelper class.
    $machine_names = [
      'field_name' => $field_name,
      'formatter_name' => $formatter_name,
    ];
    $field_info = field_info_field($field_name);
    $instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);

    $helper = new \TripalFieldTestHelper($bundle_name, $machine_names, $entity, $field_info, $instance_info, 'tripal_manage_analyses');
    $formatter_class = $helper->getInitializedClass();

    // Check we have the variables we initialized.
    $this->assertNotEmpty($helper->bundle,
      "Could not load the bundle.");
    $this->assertNotEmpty($helper->getFieldInfo($field_name),
      "Could not lookup the field information.");
    $this->assertNotEmpty($helper->getInstanceInfo($bundle_name, $field_name),
      "Could not lookup the instance information.");
    $this->assertNotEmpty($formatter_class,
      "Couldn't create a formatter class instance.");
    $this->assertNotEmpty($entity,
      "Couldn't load an entity.");

  }

  /**
   * @group wip
   */
  public function testFieldFindsAllRecords() {

    $records = $this->create_test_features();
    $gene = $records['gene'];

    $entity = $records['entity'];
    $entity_id = $records['entity_id'];
    $bundle_name = $records['bundle_name'];
    $field_name = 'data__sequence_features';
    $formatter_name = 'data__sequence_features_formatter';

    // Initialize the widget class via the TripalFieldTestHelper class.
    $machine_names = [
      'field_name' => $field_name,
      'formatter_name' => $formatter_name,
    ];
    // $field_info = field_info_field($field_name);
    //    $instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);.
    $entities = tripal_load_entity('TripalEntity', [$entity_id], FALSE);

    $entity = $entities[$entity_id];
    $this->assertEquals($gene->uniquename, $entity->chado_record->uniquename);

    // Load the fields.  TODO: can we load specific fields instead?
    $entity->save();
    $entity->view();

    // OK we got our gene back!
    $field = $entity->{'data__sequence'};

    var_dump($entity) ;
var_dump($field);
  }

  /**
   *
   */
  private function create_test_features() {

    // warning: this doesnt actually work, we never shortcut....
    if (!empty($this->records)) {
      return $this->records;
    }

    $gene_term = chado_get_cvterm(['id' => 'SO:0000704']);

    $organism_id = factory('chado.organism')->create()->organism_id;
    $gene = factory('chado.feature')->create(['type_id' => $gene_term->cvterm_id, 'organism_id' => $organism_id, 'residues' => 'AAAAAAAA']);
    $mrna = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000234'])->cvterm_id, 'organism_id' => $organism_id]);
    $cds = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000316'])->cvterm_id, 'organism_id' => $organism_id]);
    $protein = factory('chado.feature')->create(['type_id' => chado_get_cvterm(['id' => 'SO:0000104'])->cvterm_id, 'organism_id' => $organism_id]);

    $this->associate_features($gene, $mrna);
    $this->associate_features($mrna, $cds);
    $this->associate_features($mrna, $protein);

    // Publish the gene feature.
    $this->publish('feature');

    // Find this entity.
    $entity_id = chado_get_record_entity_by_table('feature', $gene->feature_id);

    $entity = entity_load('TripalEntity', [$entity_id]);

    $bundle_details = db_query("
         SELECT bundle_id, type_column, type_id
         FROM chado_bundle b
         WHERE data_table=:table AND type_id=:type_id
         ORDER BY bundle_id ASC LIMIT 1",
      [
        ':table' => 'feature',
        ':type_id' => $gene_term->cvterm_id,
      ])->fetchObject();
    $bundle_id = $bundle_details->bundle_id;

    $bundle_name = 'bio_data_' . $bundle_id;

    $records = [
      'entity' => $entity,
      'entity_id' => $entity_id,
      'gene' => $gene,
      'mrna' => $mrna,
      'cds' => $cds,
      'protein' => $protein,
      'bundle_name' => $bundle_name,
    ];

    $this->records = $records;

    return $records;
  }

  /**
   *
   */
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
