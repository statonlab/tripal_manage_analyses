<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('php', 'tripal_chado', '../tests/TripalFieldTestHelper');
module_load_include('inc', 'tripal_manage_analyses', 'includes/TripalFields/data__sequence_features/data__sequence_features');

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
   * @group doggo
   */
  public function testFieldDirect() {
    $records = $this->create_test_features();
    $gene = $records['gene'];
    $mrna = $records['mrna'];
    $cds = $records['cds'];
    $protein = $records['protein'];

    $entity_id = $records['entity_id'];
    $bundle_name = $records['bundle_name'];
    $field_name = 'data__sequence_features';
    $formatter_name = 'data__sequence_features_formatter';

    // Initialize the widget class via the TripalFieldTestHelper class.
    $machine_names = [
      'field_name' => $field_name,
      'formatter_name' => $formatter_name,
    ];
    $field = field_info_field($field_name);
    $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);

    $id = $field['id'];
    $entities = tripal_load_entity('TripalEntity', [$entity_id], FALSE, [$id]);


    $entity = $entities[$entity_id];
    $entity->view();

    $field_object = new \data__sequence_features($field, $instance);
    // $entity->view();
    $entity = $field_object->load($entity);

    $value = $entity->{'data__sequence_features'}['und'][0]['value'];


    $this->assertNotEmpty($value);

    foreach ($value as $mrna_key => $fmrna) {

      $this->assertEquals($mrna->feature_id, $mrna_key);
      $this->assertArrayHasKey('info', $fmrna);
      $this->assertArrayHasKey('children', $fmrna);

      $mrna_info = $fmrna['info'];
      var_dump($mrna_info);

      $this->assertArrayHasKey($mrna_info['residues']);


      $gchildren = $fmrna['children'];

      $this->assertArrayHasKey($cds->feature_id, $gchildren);
      $this->assertArrayHasKey($protein->feature_id, $gchildren);


      $fcds = $gchildren[$cds->feature_id];
      $fprotein = $gchildren[$protein->feature_id];

      $this->assertArrayNotHasKey('children', $fcds);
      $this->assertArrayNotHasKey('children', $fprotein);

      $this->assertArrayHasKey('info', $fcds);
      $this->assertArrayHasKey('info', $fprotein);

    }

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
    $gene = factory('chado.feature')->create([
      'type_id' => $gene_term->cvterm_id,
      'organism_id' => $organism_id,
      'residues' => 'AAAAAAAA',
    ]);
    $mrna = factory('chado.feature')->create([
      'type_id' => chado_get_cvterm(['id' => 'SO:0000234'])->cvterm_id,
      'organism_id' => $organism_id,
      'residues' => 'MRNAMRNAMRNA',

    ]);
    $cds = factory('chado.feature')->create([
      'type_id' => chado_get_cvterm(['id' => 'SO:0000316'])->cvterm_id,
      'organism_id' => $organism_id,
      'residues' => 'CDSCDSCDS',

    ]);
    $protein = factory('chado.feature')->create([
      'type_id' => chado_get_cvterm(['id' => 'SO:0000104'])->cvterm_id,
      'organism_id' => $organism_id,
      'residues' => 'PROTPROT'
    ]);

    $this->associate_features($gene, $mrna);
    $this->associate_features($mrna, $cds);
    $this->associate_features($mrna, $protein);

    factory('chado.featureprop')->create(['feature_id' => $mrna->feature_id]);
    factory('chado.featureprop')->create(['feature_id' => $mrna->feature_id]);

    factory('chado.feature_cvterm')->create(['feature_id' => $mrna->feature_id]);
    factory('chado.feature_cvterm')->create(['feature_id' => $mrna->feature_id]);



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



  /**
   * Dont run this test, its redundant with below, just anotehr way to approach it.
   **/
  public function FieldFindsAllRecords() {

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
    $field_info = field_info_field($field_name);
    // $instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);.
    $id = $field_info['id'];
    $entities = tripal_load_entity('TripalEntity', [$entity_id], FALSE, [$id]);

    $entity = $entities[$entity_id];

    $this->assertEquals($gene->uniquename, $entity->chado_record->uniquename);

    // Load the fields.  TODO: can we load specific fields instead?
    $entity->view();

    // OK we got our gene back!
    $field = $entity->{'data__sequence_features'};
    var_dump($field['und'][0]['value']);
  }

}
