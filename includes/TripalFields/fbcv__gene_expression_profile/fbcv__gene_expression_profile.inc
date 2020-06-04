<?php
/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class fbcv__gene_expression_profile extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Expression Profile';

  // The default description for this field.
  public static $default_description = 'The gene expression profile of an organism.';

  // The default widget for this field.
  public static $default_widget = 'fbcv__gene_expression_profile_widget';

  // The default formatter for this field.
  public static $default_formatter = 'fbcv__gene_expression_profile_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  // set this to 'tripal_chado'
  public static $module = 'tripal_manage_analyses';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
     // It is expected that all fields set a 'value' in the load() function.
     // In many cases, the value may be an associative array of key/value pairs.
     // In order for Tripal to provide context for all data, the keys should
     // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
     // function that are supported by the query() function should be
     // listed here.
     'searchable_keys' => array(),
  );

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = array(
     'TripalTabDownloader',
     'TripalCSVDownloader',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'FBcv',
    // The name of the term.
    'term_name' => 'gene expression profile',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0003088',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  );

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
    * @see ChadoField::load()
    *
    **/

  public function load($entity) {

    // ChadoFields automatically load the chado column specified in the
    // default settings above. If that is all you need then you don't even
    // need to implement this function. However, if you need to add any
    // additional data to be used in the display, you should add it here.
    parent::load($entity);
    $field = get_class($this);
    $entity->{$field}['und'][0]['value'] = NULL;

    if ($entity->chado_table != 'organism') {
      return;
    }

    $genome_term = tripal_load_term_entity([
      'vocabulary' => 'operation',
      'accession' => '0314',
    ]);
    $genome_bundle = tripal_load_bundle_entity(['term_id' => $genome_term->id]);

    if (!$genome_bundle) {
      return;
    }
    $genome_table = 'public.chado_' . $genome_bundle->name;

    $sql = "SELECT  A.analysis_id, A.name, A.program, A.timeexecuted, GT.entity_id
      FROM {chado.analysis} A
      JOIN {chado.organism_analysis} AO ON A.analysis_id = AO.analysis_id
      INNER JOIN {" . db_escape_table($genome_table) . "} GT ON A.analysis_id = GT.record_id
      WHERE AO.organism_id = :organism_id";

    $results = db_query($sql, [
      ':organism_id' => $entity->chado_record->organism_id,
    ]);

    $analyses = [];
    foreach ($results as $analysis) {
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

    $entity->{$field}['und'][0]['value'] = $analyses;
  }



    /**
    * @see ChadoField::query()
    *
    **/

  public function query($query, $condition) {
  }

    /**
    * @see ChadoField::queryOrder()
    *
    **/

  public function queryOrder($query, $order) {
  }


    /**
    * @see ChadoField::elementInfo()
    *
    **/

  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return array(
      $field_term => array(
        'operations' => array('eq', 'ne', 'contains', 'starts'),
        'sortable' => TRUE,
        'searchable' => TRUE,
      ),
    );
  }

}


