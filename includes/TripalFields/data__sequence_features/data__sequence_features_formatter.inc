<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class data__sequence_features_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Transcript Information';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_features'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $parent = $entity->chado_record->feature_id;

    $children = $this->findChildFeatures($parent);
    //An array of child feature feature_ids.
    //Each of these has two keys: children for THAT child's children, and info, for an array of that child's annotations.


    foreach ($children as $child_id => $child) {

      #populate info for this child
      $name = $child['info']->name;

      $output = "<h3>{$name} </h3>";

      $output .= $this->buildChildTable($child);

      $element[$child] = ['#markup' => $output];

    }


  }


  private function buildChildTable($child) {

    $parent_name = $child['info']->name;



    $header = ['Name', 'Type', 'Parent', 'Annotations', 'Download'];

    $rows = [];



    $children = $child['children'];

    foreach ($children as $gchild){
      $info = $gchild['info'];

      ['Name' => $gchild
      'Type' =>
      'Parent' => $parent_name]





    }


    $out = theme('table', ['header' => $header, 'rows' => $rows]);


    return $out;


  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode) {
    return '';
  }


  /**
   * For a given feature, find all child features.  For each child feature,
   * return:
   *  - the type name
   *  - annotation names in feature_cvterm
   *  - featureloc info, including source feature name
   *
   * @param $feature_id
   *
   * @return array
   */
  private function findChildFeatures($feature_id) {
    $this_children = [];

    $query = db_select('chado.feature_relationship', 'fr')
      ->fields('fr')
      ->condition('object_id', $feature_id)
      ->execute()
      ->fetchAll();

    foreach ($query as $child) {
      chado_set_active('chado');
      $child_id = $child->subject_id;
      $query = db_select('chado.feature', 'f');
      $query->fields('f');
      $query->join('chado.cvterm', 'cvt', 'cvt.cvterm_id = f.type_id');
      $query->addField('cvt', 'name', 'type_name');//table, field, alias
      //Left Join on annotations
      $query->leftJoin('chado.feature_cvterm', 'fcvt', 'fcvt.feature_id = f.feature_id');
      $query->leftJoin('chado.cvterm', 'cvt', 'cvt.cvterm_id = fcvt.cvterm_id');
      //Left join on feature location
      $query->leftJoin('chado.featureloc', 'fl', 'fl.feature_id = f.feature_id');
      $query->leftJoin('chado.feature', 'srcf', 'fl.srcfeature_id = srcf.feature_id');
      $query->addField('srcf', 'name', 'source_feature_name');//table, field, alias
      $query->fields('fl');
      $query->fields('cvt', ['name']);
      $query->condition('f.feature_id', $child_id);

      $child_info = $query->execute()->fetchAll();
      // $this_children[$feature_id][$child_id]['info'] = $child_info;
      $this_children[$child_id]['info'] = $child_info;

      $grand_children = $this->findChildFeatures($child->subject_id);
      if (!empty($grand_children)) {
        $this_children[$child_id]['children'] = $grand_children;
      }
    }

    return $this_children;
  }


}