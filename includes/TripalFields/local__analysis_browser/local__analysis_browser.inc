<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class local__analysis_browser extends ChadoField {

  public static $default_label = 'Analyses';


  public static $default_description = 'Display a set of analyses related to an organism';

  public static $default_widget = 'local__analysis_browser_widget';

  public static $default_formatter = 'local__analysis_browser_formatter';

  /**
   * The module that manages this field.
   */
  public static $module = 'tripal_manage_analyses';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be.
  /**
   * Changed.
   */
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    'searchable_keys' => [],
  ];

  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'analysis_browser',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'analysis_browser',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,

    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    //'chado_table' => 'organism',
    //// The column of the table in Chado where the value of the field comes from.
    //'chado_column' => 'organism_id',
    //// The base table.
    //'base_table' => 'organism',
  ];

  public static $no_ui = FALSE;


  public static $no_data = FALSE;

  /**
   * Load field.
   *
   * @see ChadoField::load()
   * @throws \Exception
   */
  public function load($entity) {
    parent::load($entity);
    $field = get_class($this);

    $sql = "SELECT  A.analysis_id, A.name, A.program, A.timeexecuted
      FROM {analysis} A
      JOIN {organism_analysis} AO ON A.analysis_id = AO.analysis_id
      WHERE AO.organism_id = :organism_id";

    $results = chado_query($sql, [
      ':organism_id' => $entity->chado_record->organism_id,
    ]);
    $entity->{$field}['und'][0]['value'] = '';

    if (!$results) {
      return;
    }

    $analyses = [];
    foreach ($results as $analysis) {
      $analysis->entity_id = chado_get_record_entity_by_table('analysis', $analysis->analysis_id);
      $analysis->title = db_query('SELECT title FROM {tripal_entity} WHERE id = :id', [
        ':id' => $analysis->entity_id,
      ])->fetchField();
      $analyses[$analysis->analysis_id] = $analysis;
    }

    // Push any entries containing the word archived to the back of the array.
    foreach ($analyses as $key => $analysis) {
      if (stripos($analysis->name, 'archived') !== FALSE) {
        // Store the analysis.
        $value = $analysis;
        // Remove the entry from the array.
        unset($analyses[$key]);
        // Add the stored analysis to the back of the array.
        $analyses[] = $value;
      }
    }

    $i = 0;
    foreach ($analyses as $analysis){
      $entity->{$field}['und'][$i]['value'] = $analysis;
      $i++;
    }
  }
}
