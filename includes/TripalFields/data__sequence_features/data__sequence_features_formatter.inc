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
      $info = $child['info'][0];

      $name = $info->uniquename;

      $element[$child_id] = [
        '#type' => 'fieldset',
        '#title' => $name,
        '#attributes' => [
          'class' => [
            'collapsible',
          ],
        ],
      ];

      #populate info for this child
      $type = $info->type_name;

      $source_feature = $info->source_feature_name;

      $header = ['Type', 'Location', 'Start', 'Stop'];

      $rows[] = [$type, $source_feature, $info->fmin, $info->fmax];

      $output = theme('table', ['header' => $header, 'rows' => $rows]);


      $rows = $this->buildChildTable($child);


      $header = [
        'Name',
        'Type',
        'Start',
        'Stop',
        // 'Parent',
        'Annotations',
        'Sequence',
      ];
      $output .= theme('table', ['header' => $header, 'rows' => $rows]);
      $element[$child_id]['tables'] = ['#markup' => $output];
    }

  }


  private function buildChildTable($child) {

    $parent_name = $child['info'][0]->uniquename;
    $rows = [];

    $children = $child['children'];

    foreach ($children as $gchild) {
      $info = $gchild['info'][0];
      $annotations = [];

      if (count($gchild['info']) > 1){

        foreach ($gchild['info'] as $record){

          $annotations[] = $record->annotation_name;
        }

      }

      $sequences = NULL;
      if ($info->residues) {
        $sequences = "Click to Download";
      }

      $annotation_string = implode('\n', $annotations);
      //If more than one entry, those are extra annotations
      $rows[] = [
        'Name' => $info->uniquename,
        'Type' => $info->type_name,
        'Start' => $info->fmin,
        'Stop' => $info->fmax,
        //  'Parent' => $parent_name,
        'Annotations' => $annotation_string,
        'Download' => $sequences
        // 'Download' => wordwrap($info->residues, 40, "<br>", TRUE),
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
      $query->leftJoin('chado.cvterm', 'cvt2', 'cvt.cvterm_id = fcvt.cvterm_id');
      $query->addField('cvt2', 'name', 'annotation_name');

      //Left join on feature location
      $query->leftJoin('chado.featureloc', 'fl', 'fl.feature_id = f.feature_id');
      $query->leftJoin('chado.feature', 'srcf', 'fl.srcfeature_id = srcf.feature_id');
      $query->addField('srcf', 'name', 'source_feature_name');//table, field, alias
      $query->fields('fl');
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