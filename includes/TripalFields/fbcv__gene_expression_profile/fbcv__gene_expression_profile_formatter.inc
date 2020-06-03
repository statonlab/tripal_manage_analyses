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
    $field = 'local__analysis_browser';

    if (!($entity->{$field}['und'][0]['value'])) {

      $output  = 'There are no analyses associated with this organism.';
      $element[] = [
        '#type' => 'markup',
        '#markup' => $output,
      ];
      return;
    }


    $rows = [];

    foreach ($items as $analysis) {

      $info = $analysis['value'];

      $date = new DateTime($info->timeexecuted);
      $date_formatted = $date->format('M jS, Y');
      if ($date_formatted === "Jan 1st, 1900") {
        $date_formatted = "Unknown";
      }


      $rows[] =  [
        l($info->name, "bio_data/{$info->entity_id}"),
        $info->program,
        $date_formatted,
      ];
    }


    $output = theme('table', [
      'header' => ['Name', 'Program', 'Date Constructed'],
      'rows' => $rows,
    ]);

    $element[] = [
      '#type' => 'markup',
      '#markup' => $output,
    ];
  }

    /**
    * @see ChadoFieldFormatter::settingsSummary()
    *
    **/

  public function settingsSummary($view_mode) {
    return '';
  }

}
