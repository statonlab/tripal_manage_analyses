<?php

/**
 * @class
 * Purpose:
 *
 * The linker table version of obi__organism
 *
 * Data:
 * Assumptions:
 */
class obi__organism_linker extends ChadoField {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Organism';

  /**
   * The default description for this field.
   */
  public static $default_description = 'Organism associated with this entity';

  /**
   * The default widget for this field.
   */
  public static $default_widget = 'obi__organism_linker_widget';

  /**
   * The default formatter for this field.
   */
  public static $default_formatter = 'obi__organism_linker_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  /**
   * Set this to 'tripal_chado'.
   */
  public static $module = 'tripal_manage_analyses';

  public static $default_settings = [
    'storage' => 'field_chado_storage',
    'searchable_keys' => [],
  ];

  /**
   * Required for all TripalFields.
   */
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'OBI',
    // The name of the term.
    'term_name' => 'organism',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0100026',
    'term_fixed' => FALSE,
    'auto_attach' => FALSE,
    'download_formatters' => [
      'TripalTabDownloader',
      'TripalCSVDownloader',
    ],
  ];

  public static $no_ui = FALSE;

  public static $no_data = FALSE;

  /**
   * Load the field.
   *
   * @see ChadoField::load()
   */
  public function load($entity) {

    parent::load($entity);

    $entity_record = $entity->chado_record;
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $key = $base_table . '_id';

    $schema = chado_get_schema($field_table);
    if (!$schema) {
      drupal_set_message(t('The organism linker field for ' . $field_table . ' is not present in the chado database!'));
    }
    $pkey = $schema['primary key'][0];

    $linker = 'chado.' . $field_table;
    $linker_key = $field_table . '_id';
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    if ($base_table === 'analysis') {
      $linker = 'chado.organism_' . $base_table;
      $linker_key = 'organism_' . $base_table . '_id';

    }

    $record_id = $entity_record->$key;
    $sql = db_select('chado . organism', 'CO');
    $sql->fields('CO', ['organism_id', 'common_name', 'abbreviation', 'genus', 'species']);
    $sql->join($linker, 'CL', 'CL.organism_id = CO.organism_id');
    $sql->fields('CL', [$linker_key]);
    $sql->condition('CL.' . $field_column, $record_id);

    $results = $sql->execute()->fetchAll();

    $i = 0;

    foreach ($results as $result) {
      $organism_abbreviation = $result->abbreviation;
      $organism_common_name = $result->common_name;
      $organism_genus = $result->genus;
      $organism_species = $result->species;
      $organism_record_id = $result->organism_id;
      $linker_record_id = $result->$linker_key;

      $organism_entity_id = chado_get_record_entity_by_table("organism", $organism_record_id);

      $entity->{$field_name}['und'][$i] = [
        'value' =>
          [
            'entity_id' => $organism_entity_id,
            'common_name' => $organism_common_name,
            'abbreviation' => $organism_abbreviation,
            'genus' => $organism_genus,
            'species' => $organism_species,
            'organism_id' => $organism_record_id,
            'linker_record_id' => $linker_record_id,
          ],
        'chado-' . $field_table . '__' . $pkey => $linker_record_id,
        'chado-' . $field_table . '__' . 'organism_id' => $organism_record_id,
        'chado-' . $field_table . '__' . $fkey_lcolumn => $record_id,

      ];

      ++$i;

    }
  }

  /**
   * Query.
   *
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
  }

  /**
   * Query Order.
   *
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
  }

  /**
   * Element Info.
   *
   * @see ChadoField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
      ],
    ];
  }

}
