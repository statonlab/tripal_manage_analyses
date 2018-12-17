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

    //TODO when we move this to the load, we'll actually have cardinality.  Let's copy that into items.

    $items = [];
    //In theory we'll have the load

    $i = 0;

    foreach ($children as $child_id => $child) {

      // (chado_get_feature_sequences(['feature_id' => $child_id], ['is_html' => TRUE]));

      $info = $child['info']['base_info'];

      $name = $info->uniquename;

      $element[0][$i] = [
        '#type' => 'fieldset',
        '#title' => $name,
        '#attributes' => [
          'class' => [
            'collapsible',
            'collapsed',
          ],
        ],
        #see: https://www.drupal.org/forum/support/module-development-and-code-questions/2012-02-07/drupal-render-fieldset-element
        '#attached' => ['js' => ['misc/collapse.js', 'misc/form.js']],
      ];


      #populate info for this child
      $type = $info->type_name;

      $locations = $child['info']['locations'];
      $header = ['Name', 'ID', 'Type', 'Location', 'Start', 'Stop'];


      foreach ($locations as $location) {
        $source_feature = $location->source_feature_name;
        $rows[] = [
          $info->name,
          $info->uniquename,
          $type,
          $source_feature,
          $location->fmin,
          $location->fmax,
        ];
      }

      $output = theme('table', ['header' => $header, 'rows' => $rows]);


      $rows = [];
      $header = [];

      $rows = $this->buildChildTable($child);

      if (empty($rows)) {
        continue;
      }

      $output .= '<h3>Derived Features</h3>';

      $header = [
        'Name',
        'Type',
        'Location',
        // 'Parent',
        'Annotations',
        'Sequence',
      ];
      $output .= theme('table', ['header' => $header, 'rows' => $rows]);
      $element[0][$i][$i . 'table'] = ['#markup' => $output];
      $i++;

      unset($rows);
      unset($header);
    }

  }


  private function buildChildTable($child) {

    $rows = [];

    $children = $child['children'];

    foreach ($children as $gchild) {
      $base_info = $gchild['info']['base_info'];
      $props = $gchild['info']['props'];
      $locations = $gchild['info']['locations'];
      $annotations = $gchild['info']['annotations'];

      $sequences = NULL;
      if ($base_info->residues) {

        module_load_include('inc', 'popup', 'includes/popup.api');

        $sequences = popup_element('sequence', $base_info->residues, $attributes = []);

      }
      else {
        //Try to infer sequence instead.


        //        $sequences = chado_get_feature_sequences(array('feature_id' => $feature->feature_id), array('derive_from_parent' => 1, 'aggregate' => 1,
        //          sub_feature_types => array('exon') ,'is_html' => 1, 'width' => $num_bases));

      }


      //TODO: build annotations string.
      $annotation_string = NULL;

      //Build the location string.  If one feature has lots of locations on different source features, let's separate them out.

      $location_string = NULL;
      $parents_loc = [];

      foreach ($locations as $location) {

        $strand_string = "0";

        if ($location->strand == (int) 1){
          $strand_string = "+";
        }
        if ($location->strand == (int) -1){
          $strand_string = "-";
        }

        if (!isset($parents[$location->source_feature_id])) {
          $parents_loc[$location->source_feature_id] = $location->source_feature_name . ': [' . $location->fmin . '-' . $location->fmax . '(' . $strand_string . ')]';
          continue;
        }

        $prev = $parents_loc[$location->source_feature_id];


        //TODO: I think we have got to sort this instead.
        $parents[$location->source_feature_id] = $prev . ', [' . $location->fmin . '-' . $location->fmax . '(' . $location->strand . ')]';
      }

      $location_string = implode('<br/>', $parents_loc);

      $rows[] = [
        'Name' => $base_info->uniquename,
        'Type' => $base_info->type_name,
        'Locations' => $location_string,
        //  'Parent' => $parent_name,
        'Annotations' => $annotation_string,
        'Download' => $sequences,
      ];

      if (isset($gchild['children'])) {
        $ggchild = $this->buildChildTable($gchild);

      }

    }
    return $rows;
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
      $query->addField('cvt', 'name', 'type_name');//table, field, alias
      $query->condition('f.feature_id', $child_id);

      $feature_info = $query->execute()->fetchObject();

      //get annotations
      $query = db_select('chado.feature_cvterm', 'fcvt')
        //$query->leftJoin('chado.feature_cvterm', 'fcvt', 'fcvt.feature_id = f.feature_id');
        ->condition('fcvt.feature_id', $child_id);
      // $query->leftJoin('chado.cvterm', 'cvt2', 'cvt.cvterm_id = fcvt.cvterm_id');
      $query->join('chado.cvterm', 'cvt', 'cvt.cvterm_id = fcvt.cvterm_id');
      $query->addField('cvt', 'name', 'annotation_name');
      $annotations = $query->execute()->fetchAll();

      //get properties
      $query = db_select('chado.featureprop', 'fp')
        ->condition('fp.feature_id', $child_id);
      $query->join('chado.cvterm', 'cvt', 'cvt.cvterm_id = fp.type_id');
      $query->addField('cvt', 'name', 'prop_name');
      $query->addField('fp', 'value', 'prop_value');
      $props = $query->execute()->fetchAll();

      //get feature locations
      $query = db_select('chado.featureloc', 'fl');
      $query->join('chado.feature', 'srcf', 'fl.srcfeature_id = srcf.feature_id');
      $query->addField('srcf', 'name', 'source_feature_name');//table, field, alias
      $query->addField('srcf', 'feature_id', 'source_feature_id');//table, field, alias
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