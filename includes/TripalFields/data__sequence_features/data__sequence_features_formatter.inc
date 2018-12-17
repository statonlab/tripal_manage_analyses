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

    $children = $entity->{'data__sequence_features'}['und'][0]['value'];


    //An array of child feature feature_ids.
    //Each of these has two keys: children for THAT child's children, and info, for an array of that child's annotations.

    $i = 0;

    foreach ($children as $child_id => $child) {

      // mRNA.

      $info = $child['info'];

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


      $rows = [];
      $header = [];

      $rows = $this->buildChildTable($child);

      if (empty($rows)) {
        continue;
      }


      $header = [
        'Name',
        'Type',
        'Location',
        'Annotations',
        'Sequence',
      ];
      $output = theme('table', ['header' => $header, 'rows' => $rows]);
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

      $info = $gchild['info'];


      $rows[] = [
        'Name' => $info->uniquename,
   //     'Type' => $base_info->type_name,
//        'Locations' => $location_string,
//        //  'Parent' => $parent_name,
//        'Annotations' => $annotation_string,
//        'Download' => $sequences,
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


}