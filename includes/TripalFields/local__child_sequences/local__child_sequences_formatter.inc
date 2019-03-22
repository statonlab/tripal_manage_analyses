<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__child_sequences_formatter extends ChadoFieldFormatter {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Child Sequences';

  /**
   * The list of field types for which this formatter is appropriate.
   */
  public static $field_types = array('local__child_sequences');

  /**
   * The list of default settings for this formatter.
   */
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

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

    // Get the settings.
    $settings = $display['settings'];

    // For now, values come from the data__sequence_features field.
    $data = $entity->data__sequence_features['und'];

    if (!$data) {
      return;
    }

    foreach ($data as $i => $value) {

      $child = $value['value'];

      // Assumption: we're on the gene entity.  We've got an array of mRNA feature ID's.
      $header = [
        'Feature Name',
        'Feature Type',
        'Sequence',
      ];

      $info = $child['info'];
      $name = $info->uniquename;

      $element[0][$i] = [
        '#type' => 'fieldset',
        '#title' => $name,
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
        '#attributes' => [
          'class' => [
            'collapsible',
            'collapsed',
          ],
        ],
        '#attached' => [
          'library' => [
            ['system', 'drupal.collapse'],
          ],
        ],
      ];

      $gchildren = $child

    }
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   **/
  public function settingsSummary($view_mode) {
    return '';
  }

}
