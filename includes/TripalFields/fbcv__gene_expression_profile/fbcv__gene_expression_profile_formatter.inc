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

    // Get the settings.
    $settings = $display['settings'];

    $field = 'fbcv__gene_expression_profile';

    if (empty($entity->{$field}['und'][0]['value'])) {
      return;
    }
    if (is_array($entity->{$field}['und'][0]['value'])) {
      $rows = array_map(function ($analysis) {
        $date = new DateTime($analysis->timeexecuted);
        return [
          l($analysis->name, "bio_data/{$analysis->entity_id}"),
          $analysis->program,
          $date->format('M jS, Y'),
        ];
      }, $entity->{$field}['und'][0]['value']);

      $output = theme('table', [
        'header' => ['Name', 'Program', 'Date Constructed'],
        'rows' => $rows,
      ]);

      $element[] = [
        '#type' => 'markup',
        '#markup' => $output,
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
