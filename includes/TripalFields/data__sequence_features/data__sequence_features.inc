<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class data__sequence_features extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------.
  /**
   * The default label for this field.
   */
  public static $default_label = 'Child Features';

  /**
   * The default description for this field.
   */
  public static $default_description = 'Gathers information about all subfeatures (mRNA, CDS, proteins) associated with a top-level feature (gene)';

  /**
   * The default widget for this field.
   */
  public static $default_widget = 'data__sequence_features_widget';

  /**
   * The default formatter for this field.
   */
  public static $default_formatter = 'data__sequence_features_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  /**
   * Set this to 'tripal_chado'.
   */
  public static $module = 'tripal_manage_analyses';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be.
  /**
   * Changed.
   */
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

  // Indicates the download formats for this field.  The list must be the.
  /**
   * Name of a child class of the TripalFieldDownloader.
   */
  public static $download_formatters = array(
    'TripalTabDownloader',
    'TripalCSVDownloader',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are.
  /**
   * Required for all TripalFields.
   */
  public static $default_instance_settings = array(
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'Sequence features',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '1255',
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
  /**
   * And field_create_instance().
   */
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects.
  /**
   * The user but otherwise provides no data.
   */
  public static $no_data = FALSE;

  /**
   * @see ChadoField::load()
   **/
  public function load($entity) {

    // ChadoFields automatically load the chado column specified in the
    // default settings above. If that is all you need then you don't even
    // need to implement this function. However, if you need to add any
    // additional data to be used in the display, you should add it here.
    parent::load($entity);

    $field = get_class($this);

    $parent = $entity->chado_record->feature_id;

    $children = $this->findChildFeatures($parent);


    var_dump($field);
    var_dump("does this seriously never happen?");
    $entity->{$field}['und'][0]['value'] = 'any body home?';

  }

  /**
   * @see ChadoField::query()
   **/
  public function query($query, $condition) {
  }

  /**
   * @see ChadoField::queryOrder()
   **/
  public function queryOrder($query, $order) {
  }

  /**
   * @see ChadoField::elementInfo()
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

  /**
   * For a given feature, find all child features.  For each child feature,
   * return:
   *  - the type name
   *  - annotation names in feature_cvterm
   *  - featureloc info, including source feature name.
   *
   * @param string $feature_id
   *   Chado feature.feature_id.
   *
   * @return array
   */
  private function findChildFeatures(string $feature_id) {
    $this_children = [];

    $prev_db = chado_set_active('chado');

    $query = db_select('chado.feature_relationship', 'fr')
      ->fields('fr')
      ->condition('object_id', $feature_id)
      ->execute()
      ->fetchAll();

    foreach ($query as $child) {
      $child_id = $child->subject_id;
      $query = db_select('chado.feature', 'f');
      $query->fields('f');
      $query->join('chado.cvterm', 'cvt', 'cvt.cvterm_id = f.type_id');
      // table, field, alias.
      $query->addField('cvt', 'name', 'type_name');
      $query->condition('f.feature_id', $child_id);

      $feature_info = $query->execute()->fetchObject();

      // Get annotations.
      $query = db_select('chado.feature_cvterm', 'fcvt')
        // $query->leftJoin('chado.feature_cvterm', 'fcvt', 'fcvt.feature_id = f.feature_id');.
        ->condition('fcvt.feature_id', $child_id);
      // $query->leftJoin('chado.cvterm', 'cvt2', 'cvt.cvterm_id = fcvt.cvterm_id');.
      $query->join('chado.cvterm', 'cvt', 'cvt.cvterm_id = fcvt.cvterm_id');
      $query->addField('cvt', 'name', 'annotation_name');
      $annotations = $query->execute()->fetchAll();

      // Get properties.
      $query = db_select('chado.featureprop', 'fp')
        ->condition('fp.feature_id', $child_id);
      $query->join('chado.cvterm', 'cvt', 'cvt.cvterm_id = fp.type_id');
      $query->addField('cvt', 'name', 'prop_name');
      $query->addField('fp', 'value', 'prop_value');
      $props = $query->execute()->fetchAll();

      // Get feature locations.
      $query = db_select('chado.featureloc', 'fl');
      $query->join('chado.feature', 'srcf', 'fl.srcfeature_id = srcf.feature_id');
      // table, field, alias.
      $query->addField('srcf', 'name', 'source_feature_name');
      // table, field, alias.
      $query->addField('srcf', 'feature_id', 'source_feature_id');
      $query->fields('fl');
      $query->condition('fl.feature_id', $child_id);
      $locations = $query->execute()->fetchAll();

      $child_info = [
        'base_info' => $feature_info,
        'annotations' => $annotations,
        'props' => $props,
        'locations' => $locations,
      ];
      $this_children[$child_id]['info'] = $child_info;

      $grand_children = $this->findChildFeatures($child->subject_id);
      if (!empty($grand_children)) {
        $this_children[$child_id]['children'] = $grand_children;
      }
    }
    chado_set_active($prev_db);
    return $this_children;
  }

}
