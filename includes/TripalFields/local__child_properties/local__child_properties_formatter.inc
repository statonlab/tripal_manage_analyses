<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__child_properties_formatter extends ChadoFieldFormatter {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Child Properties';

  /**
   * The list of field types for which this formatter is appropriate.
   */
  public static $field_types = ['local__child_properties'];

  /**
   * The list of default settings for this formatter.
   */
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see ChadoFieldFormatter::settingsForm()
   **/
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * @see ChadoFieldFormatter::View()
   **/
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings.
    $settings = $display['settings'];

    // For now, values come from the data__sequence_features field.
    $data = $entity->{'data__sequence_features'}['und'][0]['value'];

    if (!$data) {
      return;
    }

    $i = 0;

    foreach ($data as $child_id => $child) {

      // Assumption: we're on the gene entity.  We've got an array of mRNA feature ID's.

      $header = [
        'Feature Name',
        'Feature Type',
        'Property Name',
        'Property Value',
      ];

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
        // see: https://www.drupal.org/forum/support/module-development-and-code-questions/2012-02-07/drupal-render-fieldset-element
        '#attached' => ['js' => ['misc/collapse.js', 'misc/form.js']],
      ];

      $rows = $this->getPropRows($child);

      $table = theme('table', ['rows' => $rows, 'header' => $header]);
      $element[0][$i]['prop_table'] = [
        '#markup' => $table,
        '#title' => t("Child Properties for !root", ['!root' => $name]),
      ];

      $i++;
    }
  }

  /**
   * Recursively goes through the child feature array and builds an array of
   *
   * @param $data
   */
  private function getPropRows($data) {

    $rows = [];

    $info = $data['info'];

    $children = $data['children'] ?? NULL;


    $props = $info->featureprop;

    //        'Feature Name',
    //        'Feature Type',
    //        'Property Name',
    //        'Property Value',

    if ($props) {

      // If there is only one property, it will be an object not an array.
      if (is_array($props)) {
        foreach ($props as $prop) {

          $rows[] = [
            $info->uniquename,
            $info->type_id->name,
            $prop->type_id->name,
            $prop->value,
          ];
        }
      }
      else {

        $rows[] = [
          $info->uniquename,
          $info->type_id->name,
          $props->type_id->name,
          $props->value,
        ];

      }

    }

    if ($children && !empty($children)) {

      foreach ($children as $child) {
        $rows = array_merge($this->getPropRows($child), $rows);
      }
    }
    return $rows;
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   **/
  public function settingsSummary($view_mode) {
    return '';
  }

}
