<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class ero__nucleic_acid_library extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Libraries';

  // The default description for this field.
  public static $default_description = 'Reagent library that is a collection of DNA fragments that is stored and propagated in a population of micro-organisms through the process of molecular cloning.';

  // The default widget for this field.
  public static $default_widget = 'ero__nucleic_acid_library_widget';

  // The default formatter for this field.
  public static $default_formatter = 'ero__nucleic_acid_library_formatter';

  // The module that manages this field.
  public static $module = 'tripal_chado';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'ero',
    // The name of the term.
    'term_name' => 'nucleic_acid_library',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000160',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.

      // The table in Chado that the instance maps to.
        'chado_table' => 'library',
        // The column of the table in Chado where the value of the field comes from.
        'chado_column' => 'library_id',
        // The base table.
        'base_table' => 'library',
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($entity) {
    //TODO:  THIS FIELD ONLY CARES ABOUT ATTACHING TO ORGANISM RIGHT NOW.  If it were to function as a proper linker field, it should load appropriately.

    // ChadoFields automatically load the chado column specified in the
    // default settings above. If that is all you need then you don't even
    // need to implement this function. However, if you need to add any
    // additional data to be used in the display, you should add it here.
    parent::load($entity);

    if ($entity->chado_table == 'organism') {
      //If its an organism, we dont have an organism_library linker table but an organism_id column in library.

      $field_name = $this->field['field_name'];
      $organism = $entity->chado_record;
      $organism_id = $organism->organism_id;

      //load library info for this bundle
      $options = [];

      $values = [
        'organism_id' => $organism_id,
      ];
      $columns = ['library_id', 'name', 'uniquename', 'type_id', 'is_obsolete'];

      $results = chado_select_record('library', $columns, $values, $options);

      //go through the results and lookup the entity id of each
      $output = [];
      $entity->{$field_name}['und'][0]['value'] = null;

      $i = 0;
      foreach ($results as $result) {

        $library_id = $result->library_id;

        $entity_id = chado_get_record_entity_by_table("library", $library_id);

        //convert type into the cvterm value
        $library_type = tripal_get_cvterm(["cvterm_id" => $result->type_id]);

        $output[] = [
          "entity_id" => $entity_id,
          "name" => $result->name,
          "is_obsolete" => $result->is_obsolete,
          "type_id" => $result->type_id,
          "library_type" => $library_type->name
        ];
        $entity->{$field_name}['und'][$i]['value'] = $output;
        $i++;
      }
    }
  }
}