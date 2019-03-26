<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class ero__nucleic_acid_library extends ChadoField {

  public static $default_label = 'Libraries';

  /**
   * The default description for this field.
   */
  public static $default_description = 'Reagent library that is a collection of DNA fragments that is stored and propagated in a population of micro-organisms through the process of molecular cloning.';

  /**
   * The default widget for this field.
   */
  public static $default_widget = 'ero__nucleic_acid_library_widget';

  /**
   * The default formatter for this field.
   */
  public static $default_formatter = 'ero__nucleic_acid_library_formatter';

  /**
   * The module that manages this field.
   */
  public static $module = 'tripal_chado';

  public static $default_settings = [
    'storage' => 'field_chado_storage',
    'searchable_keys' => [],
  ];

  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'ERO',
    // The name of the term.
    'term_name' => 'nucleic_acid_library',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000160',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    'chado_table' => 'library',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'library_id',
    /**
 * The base table.
 */
    'base_table' => 'library',
  ];

  public static $no_ui = FALSE;

  public static $no_data = FALSE;

  /**
   * Load field.
   *
   * @see ChadoField::load()
   */
  public function load($entity) {

    if ($entity->chado_table == 'organism') {
      // If its an organism, we dont have an organism_library linker table but an organism_id column in library.
      $field_name = $this->field['field_name'];
      $organism = $entity->chado_record;
      $organism_id = $organism->organism_id;

      // Load library info for this bundle.
      $options = [];

      $values = [
        'organism_id' => $organism_id,
      ];
      $columns = ['library_id', 'name', 'uniquename', 'type_id', 'is_obsolete'];

      $results = chado_select_record('library', $columns, $values, $options);

      // Go through the results and lookup the entity id of each.
      $output = [];
      $entity->{$field_name}['und'][0]['value'] = NULL;
      $i = 0;

      foreach ($results as $result) {

        $library_id = $result->library_id;

        $entity_id = chado_get_record_entity_by_table("library", $library_id);

        // Convert type into the cvterm value.
        $library_type = tripal_get_cvterm(["cvterm_id" => $result->type_id]);
        $output = [
          "entity_id" => $entity_id,
          "name" => $result->name,
          "is_obsolete" => $result->is_obsolete,
          "type_id" => $result->type_id,
          "library_type" => $library_type->name,
        ];
        $entity->{$field_name}['und'][$i]['value'] = $output;
        $i++;
      }
    }
  }

}
