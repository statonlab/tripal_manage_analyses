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
   * Featureloc start rel to parent.
   */
  private $parent_start;

  /**
   * Featureloc stop rel to parent.
   */
  private $parent_stop;

  /**
   * Featureloc strand rel to parent.
   */
  private $parent_strand;

  /**
   * Holds converted featureloc information for the feature viewer drawing.
   */
  private $feature_coords;

  private $featureviewerData;


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

    drupal_add_js("https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.4/dist/feature-viewer.bundle.js", [
      'type' => 'external',
      'scope' => 'header',
      'group' => 15,
      'every_page' => TRUE,
      'weight' => 500,
    ]);

    $child_draw = [];
    $sequence = $entity->data__sequence['und'][0]['value'];

    if (!$sequence) {

      $feature_id = $entity->chado_record->feature_id;

      $options = ['build_from_parent' => 1];
      $returned = chado_get_feature_sequences(['feature_id' => $feature_id], $options);

      $sequence = $returned[0]['residues'] ?? NULL;

      if (!$sequence) {
        // Fudge a sequence based on the length.
        $length = $entity->data__sequence_length['und'][0]['value'];
        if (!$length) {
          // Fudge the length.
          $length = '5000';
        }
        $sequence = '';
        $i = 0;
        while ($i < $length) {
          $sequence .= 'N';
          $i++;
        }
      }

    }

    $coordinates = $entity->data__sequence_coordinates['und'][0]['value'];

    $this->parent_start = $coordinates['local:fmin'];
    $this->parent_stop = $coordinates['local:fmax'];
    $this->parent_strand = $coordinates['data:0853'];

    $child_draw['residues'] = $sequence;

    $element[0]['gene_drawing'] = [
      '#type' => 'item',
      '#title' => t('mRNAs'),
      '#prefix' => '<div id="tripal_manage_expression_featureloc_gene">',
      '#suffix' => '</div>',
    ];


    foreach ($entity->{'data__sequence_features'}['und'] as $i => $data) {
      $child = $data['value'];

      $info = $child['info'];
      $name = $info->uniquename;

      $element[$i] = [
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

      $element[$i]['drawing'] = [
        '#type' => 'item',
        '#title' => t('Drawing'),
        '#prefix' => '<div id="tripal_manage_expression_featureloc_viewer_' . $i . '">',
        '#suffix' => '</div>',
      ];

      $rows = $this->buildChildTable($child);

      //Now build glyph data

      //First, get all exons for use with mRNA

      //Now, add mRNA to master glyph with exons

      //Finally build the glyph for this mRNA with just the CDS and mRNA from exons.

      $this->build_featureviewer_data($i, $child);

      if (empty($rows)) {
        continue;
      }

      $header = [
        'Name',
        'Type',
        'Sequence',
        'Location',
      ];
      $output = theme('table', ['header' => $header, 'rows' => $rows]);
      $element[$i][$i . 'table'] = ['#markup' => $output];

      unset($rows);

    }

    // Un-collapse the first fieldset.
    $element[0]['#attributes']['class'] = ['collapsible'];

    $child_draw['children'] = $this->feature_coords;
    // Pass in the needed JS info.
    drupal_add_js([
      'children_draw_info' => $child_draw,
    ], 'setting');

    drupal_add_js(drupal_get_path('module', 'tripal_manage_analyses') . "/theme/js/tripal_manage_analyses_featureloc.js");

    // Add JS that makes the sequence hideable.
    drupal_add_js(drupal_get_path('module', 'tripal_manage_analyses') . "/theme/js/hide_show_sequence.js");

  }

  /**
   *
   */
  private function build_featureviewer_data($i, $child) {

    $info = $child['info'];

    $grand_children = $child['children'] ?? NULL;


    $exons = $this->prepare_exons_for_mRNA_glyph($i, $info->feature_id, $info);
    // Set base info
    // All child features will be drawn on this one in 'data'
    // Convert and store the coordinates for hte feature viewer.
    $this->convertFeatureCoords($i, $info->feature_id, $info, $exons);

    $this->buildNewDataScheme($i, $info->feature_id, $info);

    // Repeat for grandchildren;.
    if ($grand_children) {
      foreach ($grand_children as $grand_child) {
        $this->build_featureviewer_data($i, $grand_child);
      }
    }
  }

  /**
   * Builds featureloc string for display to user.
   *
   * @param $featureloc
   *   The featureloc object returned from chado_expand_var on featureloc.
   *
   * @return string
   */
  private function buildFeatureString($featureloc) {

    $info = $featureloc->feature_id;

    $min = $info->fmin ?? NULL;
    $max = $info->fmax ?? NULL;
    $strand = $info->strand ?? NULL;
    $parent = $info->srcfeature_id->name ?? NULL;

    if (!$min or !$max or !$strand) {
      return 'No location available.';
    }

    $out = "${parent}:  ${min}-${max} (${strand})";

    return $out;
  }


  /**
   * Converts featureloc coordinates to be based on the entity.
   *
   * @param $i
   * @param $feature_id
   * @param $info
   * @param $exons
   * Array of exon data for drawing the mRNA.
   */
  private function convertFeatureCoords($i, $feature_id, $info, $exons) {

    $type = $info->type_id->name;

    $use = ['CDS', 'mRNA'];

    if (!in_array($type, $use)){

    }

    $featureloc = $info->featureloc->feature_id;
    // TODO: what if theres no featureloc relative to a parent?
    $parent_start = $this->parent_start;
    $parent_stop = $this->parent_stop;
    $strand = $this->parent_strand;

    $min = $featureloc->fmin ?? NULL;
    $max = $featureloc->fmax ?? NULL;
    $strand = $featureloc->strand ?? NULL;

    if ($strand == '+') {

      // It doesnt matter what strand it is, we always do this.
      // TODO: check that assertion.
      $start = $min - $parent_start + 1;
      $stop = $max - $parent_start + 1;
    }

    else {
      $start = $min - $parent_start + 1;
      $stop = $max - $parent_start + 1;
    }

    $type = $info->type_id->name;


    if ($type == 'mRNA') {
      //Dont draw the mRNA itself, build it from exons instead.
      return;
    }

    $cases = ['exon', 'CDS'];
    if (!inarray($type, $cases)){
      return;
    }

    if ($type == 'exon') {

      $name = 'mRNA';
      //This is going in to build the mRNA
    }

    if ($type == 'CDS') {

      $name = 'CDS';

      // Build its locations from featureloc instead.

      $query = db_select('chado.featureloc', 'fl');
        $query->fields('fl', ['fmin', 'fmax']);
        $query->orderBy('fmin', 'ASC');
        $query->join('chado.feature', 'f', 'f.feature_id = fl.feature_id');
        $query->fields('f', ['uniquename']);
        $query->condition('f.feature_id')->execute();

        $locs = $query->execute();
        $color = $this->get_feature_color('CDS');

      if (!isset($this->feature_coords[$i]['CDS'])) {

        $this->feature_coords[$i][$type] = [
          'name' => $info->uniquename,
          'color' => $color,
          'type' => 'rect',
        ];
      }

        foreach ($locs as $loc) {

          $this->feature_coords[$i]['CDS']['data'][] = [
            'x' => $loc->fmin,
            'y' => $loc->fmax,
            'description' => $loc->fmin . '-' . $loc->fmax,
          ];
        }
    }
//
//    $this->feature_coords[$i][$type]['data'][] = [
//      'x' => $start,
//      'y' => $stop,
//      'description' => $info->uniquename,
//    ];

  }

  /**
   * A color lookup to pass different colors to different feature subtypes.
   *
   * @param string $name
   *   The feature type name.
   *
   * @return string
   *   a hex color code.
   */
  private function get_feature_color(string $name) {

    switch ($name) {
      case 'mRNA':
        return '#12E09D';

      case 'polypeptide':
        return '#808080';

      case 'CDS':
        return '#FF0000';

      case 'exon':
        return '#F4D4AD';

      case NULL:
        return '#000000';
    }

  }

  /**
   *
   */
  private function buildChildTable($child, $i) {

    $rows = [];

    $children = $child['children'];

    $mrna = $child['info'];

    $sequence = $this->buildSequence($mrna);
    $location = isset($mrna->featureloc) ? $this->buildFeatureString($mrna->featureloc) : 'Not available';

    $rows[] = [
      'Name' => $mrna->uniquename,
      'Type' => $mrna->type_id->name,
      'Sequence' => $sequence,
      'Locations' => $location,
    ];

    foreach ($children as $gchild) {

      $info = $gchild['info'];

      $location = isset($info->featureloc) ? $this->buildFeatureString($info->featureloc) : 'Not available';

      $sequence = $this->buildSequence($info);

      $rows[] = [
        'Name' => $info->uniquename,
        'Type' => $info->type_id->name,
        'Sequence' => $sequence,
        'Locations' => $location,
      ];

      if ($info->type_id->name == 'CDS') {

        $this->featureviewerData[$i]['CDS'] = [
          'name' => $info->uniquename,
          'color' => $color,
          'type' => 'rect'
        ];

        $locs = $this->unwrittenFetchCDSLocArray;

        foreach ($locs as $loc) {
        $this->feature_coords[$i]['CDS']['data'][] = [
          'x' => $loc->fmin,
          'y' => $loc->fmax,
          'description' => $loc->fmin . '-' . $loc->fmax,
        ];

      }


      }




      }


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

  /**
   * Build and display sequence for polypeptides and mRNA.
   *
   * @param $info
   *
   *   Info object.  @see \chado_generate_var().
   *
   * @return string
   */
  public function buildSequence($info) {

    module_load_include('inc', 'popup', 'includes/popup.api');

    $type = $info->type_id->name;
    $out = '';
    // Only display polypeptide, mRNA CDS.
    $check = ['polypeptide', 'mRNA', 'CDS'];

    // Might want to offer noncoding RNA?
    if (!in_array($type, $check)) {
      $out = 'N/A';
      return $out;
    }

    $options = ['width' => 40, 'is_html' => 1];
    // TODO: add in more options based on the sequence we're looking at.
    // aggregate: Set to '1' if the sequence should only contain sub features, excluding intro sub feature sequence. For example, set this option to obtain just the coding sequence of an mRNA.
    // sub_feature_types: Only include sub features (or child features) of the types provided in the array.
    // TODO: I guess this should be done in the LOAD instead of the formatter?
    $returned = chado_get_feature_sequences(['feature_id' => $info->feature_id], $options);

    if (!$returned[0]['residues']) {

      switch ($type) {

        case 'mRNA':
          //Build from child exons
          $options['aggregate'] = 1;
          $options['sub_feature_types'] = ['exon'];
          $returned = chado_get_feature_sequences(['feature_id' => $info->feature_id], $options);
          break;

        case 'CDS':
          $options['derive_from_parent'] = 1;
          $returned = chado_get_feature_sequences(['feature_id' => $info->feature_id], $options);
          break;

        default:
          break;
      }

      foreach ($returned as $sequence) {

        $seq_string = $sequence['residues'];
        $header = $sequence['defline'] ?? $info->name;

        $defline = '>' . $header . '<br>';

        if ($seq_string) {
          $out .= $defline . $seq_string;
        }
      }
    }

    if ($out) {

      $container_id = 'popover-controller-' . $info->feature_id;
      $id = 'popover-' . $info->feature_id;

      $out = '
      <a id="' . $container_id . '"data-target="' . $id . '" class="sequence-expand-trigger">Sequence</a>
      <div style="display: none;" class="tripal-sequence-popover" id ="' . $id . '"> ' . $out . '</div>';
    }

    if (!$out) {
      $out = 'Unavailable';
    }

    return $out;

  }

}
