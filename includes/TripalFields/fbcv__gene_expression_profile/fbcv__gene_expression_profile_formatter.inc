<?php
/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class fbcv__gene_expression_profile_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Expression Profile';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('fbcv__gene_expression_profile');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

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
      if (!$item['value']['entity_id']) {
        continue;
      }

      $element[] = [
        '#type' => 'item',
        '#markup' => l($item['value']['title'], 'bio_data/' . $item['value']['entity_id']),
      ];
    }
    // Get the settings
    $settings = $display['settings'];
  }

    /**
    * @see ChadoFieldFormatter::settingsSummary()
    *
    **/

  public function settingsSummary($view_mode) {
    return '';
  }

}
