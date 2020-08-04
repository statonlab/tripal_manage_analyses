<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__phylotree_link_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Phylotree Link';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__phylotree_link');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state)
  {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
  {
    $feature_id = $entity->chado_record->feature_id;
    $query = chado_db_select('phylonode', 'pn');
    $query->fields('pt', ['phylotree_id', 'name']);
    $query->join('phylotree', 'pt', 'pt.phylotree_id = pn.phylotree_id');
    $query->condition('pn.feature_id', $feature_id, '='); // the feature_id column will be filled if a regex is supplied when the tree is loaded, otherwise need to query pn.label
    $total = $query->countQuery()->execute()->fetchField();
    $table_size = 10;
    $page = pager_default_initialize($total, $table_size);
    $query->range($page * $table_size, $table_size);
    $results = $query->execute();//->fetchAll();

    $header = ['Phylotrees containing this feature'];
    $rows = [];
    while ($record = $results->fetchAssoc()) {
      $rows[] = [
        l($record['name'], 'bio_data/' . chado_get_record_entity_by_table('phylotree', $record['phylotree_id'])),
      ];
    }
    $pager = theme('pager', ['quantity', $total]);
    $pager = $this->ajaxifyPager($pager, $entity);
    $table = theme('table',
      [
        "header" => $header,
        "rows" => $rows,
        'pager' => $pager,
      ]);
    if ($rows == null) {
      return 0;
    }
    $element[0] =
      [
        "#type" => "markup",
        "#markup" => $table . $pager,
      ];
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode)
  {
    return '';
  }

}
