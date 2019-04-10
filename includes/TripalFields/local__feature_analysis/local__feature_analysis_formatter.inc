<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__feature_analysis_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Analyses';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__feature_analysis'];

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
    foreach ($items as $item) {
      dpm($item);
      if (!$item['value']['entity_id']) {
        continue;
      }

      $element[] = [
        '#type' => 'item',
        '#markup' => l($item['value']['title'], 'bio_data/' . $item['value']['entity_id']),
      ];
    }
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode) {
    return '';
  }

}
