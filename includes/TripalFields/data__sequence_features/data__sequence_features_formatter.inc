<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class data__sequence_features_formatter extends ChadoFieldFormatter {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Transcript Information';

  /**
   * The list of field types for which this formatter is appropriate.
   */
  public static $field_types = ['data__sequence_features'];

  /**
   * The list of default settings for this formatter.
   */
  public static $default_settings = [
    'setting1' => 'default_value',
  ];


  /**
   * featureloc start rel to parent.
   */
  private $parent_start;

  /**
   * featureloc stop rel to parent.
   */
  private $parent_stop;

  /**
   * featureloc strand rel to parent.
   */
  private $parent_strand;

  /**
   * Holds converted featureloc information for the feature viewer drawing.
   */
  private $feature_coords;


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

    $parent = $entity->chado_record->feature_id;

    drupal_add_js("https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.0/dist/feature-viewer.bundle.js", [
      'type' => 'external',
      'scope' => 'header',
      'group' => 15,
      'every_page' => TRUE,
      'weight' => 500,
    ]);

    $child_draw = [];
    $sequence = $entity->data__sequence['und'][0]['value'];

    if (!$sequence) {
      //TODO: We cant draw without a sequence.
      //Now what?
    }


    $coordinates = $entity->data__sequence_coordinates['und'][0]['value'];

    $this->parent_start = $coordinates['local:fmin'];
    $this->parent_stop = $coordinates['local:fmax'];
    $this->parent_strand = $coordinates['data:0853'];


    $child_draw['residues'] = $sequence;

    $element[0]['drawing'] = [
      '#type' => 'item',
      '#title' => t('Drawing'),
      '#prefix' => '<div id="tripal_manage_expression_featureloc_viewer">',
      '#suffix' => '</div>',
    ];

    foreach ($entity->{'data__sequence_features'}['und'] as $i => $data) {

      $child = $data['value'];

      $info = $child['info'];
      $name = $info->uniquename;

      $element[$i]['fs'] = [
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

      $rows = $this->buildChildTable($child);

      $this->feature_coords[$name]['base'] = [
        'data' => [
          'x' => 0,
          'y' => 10000,
        ],
      ];

      $this->build_featureviewer_data($name, $child);

      if (empty($rows)) {
        continue;
      }

      $header = [
        'Name',
        'Type',
        'Location',
      ];
      $output = theme('table', ['header' => $header, 'rows' => $rows]);
      $element[$i]['fs'][$i . 'table'] = ['#markup' => $output];

      unset($rows);

    }

    // Un-collapse the first fieldset.
    $element[0]['fs']['#attributes']['class'] = ['collapsible'];


    dpm($this->feature_coords);
    $child_draw['children'] = $this->feature_coords;
    // Pass in the needed JS info.
    drupal_add_js([
      'children_draw_info' => $child_draw,
    ], 'setting');

    drupal_add_js(drupal_get_path('module', 'tripal_manage_analyses') . "/theme/js/tripal_manage_analyses_featureloc.js");

  }

  private function build_featureviewer_data($parent_name, $child) {

    $info = $child['info'];

    $grand_children = $child['children'] ?? NULL;

    //Convert and store the coordinates for hte feature viewer.
    $this->convertFeatureCoords($parent_name, $info->feature_id, $info->featureloc);

    //    data (array of objects) : An array of objects, each containing :
    //x (int) :
    //first position
    //y (int) :
    //last position (or a value for features of type "line")
    //id (string) :
    //an Id (Optionnal)
    //description (string) :
    //Small description (Optionnal)
    //color (string) :
    //a custom color (Optionnal)


    //repeat for grandchildren;

    if ($grand_children){
      foreach ($grand_children as $grand_child) {
        $this->build_featureviewer_data($parent_name, $grand_child);
      }
    }

  }

  /**
   * @param $featureloc
   *   The featureloc object returned from chado_expand_var on featureloc.
   *
   * @return string
   */
  private function buildFeatureString($featureloc) {

    $info = $featureloc->feature_id;

    $min = $info->fmin;
    $max = $info->fmax;
    $strand = $info->strand;
    $parent = $info->srcfeature_id->name;

    if (!$min or !$max or !$strand) {
      return 'No location available.';
    }

    $out = "${parent}:  ${min}-${max} (${strand})";

    return $out;
  }

  private function convertFeatureCoords($base_name, $feature_id, $featureloc) {

    //TODO: what if theres no featureloc relative to a parent?
    $parent_start = $this->parent_start;
    $parent_stop = $this->parent_stop;
    $strand = $this->parent_strand;

    $info = $featureloc->feature_id;
    $min = $info->fmin;
    $max = $info->fmax;
    $strand = $info->strand;


    //parent mRNA coordinates, for a positive strand:

    //158059..165483

    //165031-165483 (1)

    //Contig1002: 162296-162356 (1)

    //Contig1002: 158059-158149 (1)

    if ($strand == '+') {

      // it doesnt matter what strand it is, we always do this.
      $start = $min - $parent_start + 1;
      $stop = $max - $parent_start + 1;
    }

    //TODO: this is a wreck!  WTF!
    else {
      $start = $min - $parent_start + 1;
      $stop =  $max - $parent_start + 1;
    }


    $this->feature_coords[$base_name][$feature_id] = [
      'data' =>
        [
          'x' => $start,
          'y' => $stop,
        ],
    ];

  }


  /**
   *
   */
  private function buildChildTable($child) {

    $rows = [];

    $children = $child['children'];

    foreach ($children as $gchild) {

      $info = $gchild['info'];

      $location = isset($info->featureloc) ? $this->buildFeatureString($info->featureloc) : 'Not available';

      $rows[] = [
        'Name' => $info->uniquename,
        'Type' => $info->type_id->name,
        'Locations' => $location,
      ];

      if (isset($gchild['children'])) {
        $ggchild = $this->buildChildTable($gchild);

        $rows = array_merge($rows, $ggchild);
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
