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
    'draw_parent_glyph' => 1,
    'draw_child_glyphs' => 1,

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


  private $featureviewerData;

  private $masterGlyphData;

  /**
   * @see ChadoFieldFormatter::settingsForm()
   **/
  public function settingsForm($view_mode, $form, &$form_state) {

    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    $element['glyphs'] = [
      '#markup' => '<p>Glyphs are drawn using the <a href ="https://github.com/calipho-sib/feature-viewer"> FeatureViewer</a> library.  This is the library used by the Tripal Analysis Blast and Tripal Analysis Interpro modules.</p>',
    ];
    $element['draw_parent_glyph'] = [
      '#type' => 'checkbox',
      '#title' => "Draw Parent-Level Glyph",
      '#description' => t("Whether or not the field should draw the parent-level glyph.  On a gene page, this means that each child mRNA will be drawn on a single glyph."),
      '#default_value' => array_key_exists('draw_parent_glyph', $settings) ? $settings['draw_parent_glyph'] : 1,
    ];

    $element['draw_child_glyphs'] = [
      '#type' => 'checkbox',
      '#title' => "Draw Child-Level Glyphs",
      '#description' => t("Whether or not the field should draw the child-level glyphs.  On a gene page, this means that each child mRNA will be drawn as its own glyph above its summary table."),
      '#default_value' => array_key_exists('draw_child_glyphs', $settings) ? $settings['draw_child_glyphs'] : 1,
    ];

    $element['reminder'] = [
      '#markup' => '<p>Friendly reminder: you need to press the Update button below and the <b>Save</b> button at the bottom of the page to change these settings.</p>',
    ];
    return $element;
  }


  /**
   * @see ChadoFieldFormatter::settingsSummary()
   **/
  public function settingsSummary($view_mode) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    $summary = t('Draw Parent-Level Glyph: @draw_parent_glyph<br>Draw Child-Level Glyphs: @draw_child_glyphs',
      [
        '@draw_parent_glyph' => $settings['draw_parent_glyph'],
        '@draw_child_glyphs' => $settings['draw_child_glyphs'],
      ]
    );

    return $summary;
  }

  /**
   * @see ChadoFieldFormatter::View()
   **/
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings.
    $settings = $display['settings'];

    $draw_parent_glyph = $settings['draw_parent_glyph'] ?? 1;
    $draw_child_glyphs = $settings['draw_child_glyphs'] ?? 1;

    drupal_add_js("https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.4/dist/feature-viewer.bundle.js", [
      'type' => 'external',
      'scope' => 'header',
      'group' => 15,
      'every_page' => TRUE,
      'weight' => 500,
    ]);

    $child_draw = [];

    $srcfeature_id = chado_query("select srcfeature_id from {featureloc} where feature_id = :target_feature_id", [':target_feature_id' => $entity->chado_record->feature_id])->fetchField();

    $sequences = chado_get_feature_sequences(['feature_id' => $srcfeature_id], []);

    $sequence = $sequences[0]['residues'] ?? NULL;

    if (!$sequence) {
      // Fudge a sequence based on the length.
      $length = chado_query("select seqlen from {feature} where feature_id = :feature_id", [':feature_id' => $srcfeature_id])->fetchField();


      if (!$length) {
        // Fudge the length.
        $length = '500000';
      }
      $sequence = '';
      $i = 0;
      while ($i < $length) {
        $sequence .= 'N';
        $i++;
      }
    }

    $coordinates = $entity->data__sequence_coordinates['und'][0]['value'];

    $this->parent_start = $coordinates['local:fmin'];
    $this->parent_stop = $coordinates['local:fmax'];
    $this->parent_strand = $coordinates['data:0853'];

    $child_draw['residues'] = $sequence;

    $child_draw['parameters'] = [
      'offset' => [
        'start' => $coordinates['local:fmin'],
        'end' => $coordinates['local:fmax'],
      ],
      'showAxis' => FALSE,
      'showSequence' => TRUE,
      'brushActive' => TRUE,
      'toolbar' => TRUE,
      'bubbleHelp' => TRUE,
      'zoomMax' => 3,
    ];

    foreach ($entity->{'data__sequence_features'}['und'] as $i => $data) {
      $child = $data['value'];

      $info = $child['info'];
      $name = $info->uniquename;

      $element[0]['gene_drawing'] = [
        '#type' => 'item',
        '#prefix' => '<div id="tripal_manage_expression_featureloc_gene">',
        '#suffix' => '</div>',
      ];

      $element[$i]['fs'] = [
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

      $element[$i]['fs']['drawing'] = [
        '#type' => 'item',
        '#prefix' => '<div id="tripal_manage_expression_featureloc_viewer_' . $i . '">',
        '#suffix' => '</div>',
      ];

      // We assume the primary child feature is mRNA.
      $mrna = $child['info'];

      // Create base mRNA drawing.
      $this->featureviewerData[$i]['mRNA'] = [
        'name' => 'mRNA',
        'color' => '#FF0000',
        'type' => 'rect',
      ];

      $color = '#' . substr(str_shuffle('ABCDEF0123456789'), 0, 6);

      // Add this mRNA to mqster drawing.
      $this->masterGlyphData[$i] = [
        'name' => $mrna->uniquename,
        // Color is going to be different for each mRNA!
        'color' => $color,
        'type' => 'rect',
      ];

      $sequence = $this->buildSequence($mrna);
      $location = isset($mrna->featureloc) ? $this->buildFeatureString($mrna->featureloc) : 'Not available';
      $rows = [];

      $uname = $mrna->uniquename;

      $entity_id = $mrna->entity_id;

      if ($entity_id) {

        $uname = l($uname, 'bio_data/' . $entity_id);
      }

      $rows[] = [
        'Name' => $uname,
        'Type' => $mrna->type_id->name,
        'Sequence' => $sequence,
        'Locations' => $location,
      ];

      // Build the table of all child features. use CDS and exon subfeatures for the glyphs.
      $rows = array_merge($rows, $this->buildChildTable($child, $i));

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
      $element[$i]['fs'][$i . 'table'] = ['#markup' => $output];

      unset($rows);

    }

    // Un-collapse the first fieldset.
    $element[0]['fs']['#attributes']['class'] = ['collapsible'];

    $child_draw['children']  = [];
    $child_draw['master'] = [];

    if ($draw_child_glyphs) {
      $child_draw['children'] = $this->featureviewerData;
    }

    if ($draw_parent_glyph) {
      $child_draw['master'] = $this->masterGlyphData;
    }

    // Pass in the needed JS info.
    drupal_add_js([
      'children_draw_info' => $child_draw,
    ], 'setting');

    drupal_add_js(drupal_get_path('module', 'tripal_manage_analyses') . "/theme/js/tripal_manage_analyses_featureloc.js");

    // Add JS that makes the sequence hideable.
    drupal_add_js(drupal_get_path('module', 'tripal_manage_analyses') . "/theme/js/hide_show_sequence.js");

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
   * Builds table of sub-feature info, extract CDS/exon info for glyphs.
   *
   * @param $child
   *   Full child info.
   * @param $i
   *
   * @return array
   *   Returns array of rows.
   */
  private function buildChildTable($child, $i) {

    $rows = [];

    $children = $child['children'];

    foreach ($children as $gchild) {

      $info = $gchild['info'];
      $location = isset($info->featureloc) ? $this->buildFeatureString($info->featureloc) : 'Not available';
      $sequence = $this->buildSequence($info);

      $uname = $info->uniquename;

      $entity_id = $info->entity_id;

      if ($entity_id) {

        $uname = l($uname, 'bio_data/' . $entity_id);
      }
      $rows[] = [
        'Name' => $uname,
        'Type' => $info->type_id->name,
        'Sequence' => $sequence,
        'Locations' => $location,
      ];

      switch ($info->type_id->name) {

        case 'CDS':

          // Get locations.  We assume the single CDS feature has multiple featurelocs, which is anti-Chado but what must be done, see:
          // @ticket https://github.com/tripal/tripal/issues/52
          $locs = db_select('chado.featureloc', 'fl')
            ->condition('fl.feature_id', $info->feature_id)
            ->fields('fl', ['fmin', 'fmax', 'rank'])
            ->orderBy('rank', 'ASC')
            ->execute();

          if (empty($locs)) {
            break;
          }

          $this->featureviewerData[$i]['CDS'] = [
            'name' => 'CDS',
            'color' => '#008000',
            'type' => 'rect',
          ];

          foreach ($locs as $loc) {
            $this->featureviewerData[$i]['CDS']['data'][] = [
              'x' => $loc->fmin,
              'y' => $loc->fmax,
              'description' => $info->uniquename,
            ];
          }
          break;

        case 'exon':

          // Exon locations go into the glyph for the mRNA.
          $loc = $info->featureloc->feature_id;

          $feature_viewer_array = [
            'x' => $loc->fmin,
            'y' => $loc->fmax,
            'description' => $info->uniquename,
          ];

          $this->masterGlyphData[$i]['data'][] = $feature_viewer_array;

          $this->featureviewerData[$i]['mRNA']['data'][] = $feature_viewer_array;

          break;

        case 'polypeptide':
        case 'five_prime_UTR':
        case 'three_prime_UTR':
          // Dont draw.
          break;

        default:
          // Draw in specific glyph
          // generate a random color.
          $color = '#' . substr(str_shuffle('ABCDEF0123456789'), 0, 6);

          $this->featureviewerData[$i][$info->type_id->name] = [
            'name' => $info->type_id->name,
            'color' => $color,
            'type' => 'rect',
          ];

          $featureloc = $info->featureloc->feature_id;
          $min = $featureloc->fmin ?? NULL;
          $max = $featureloc ?? NULL;

          $this->featureviewerData[$i][$info->type_id->name]['data'][] = [
            'x' => $min,
            'y' => $max,
            'description' => $info->uniquename,
          ];
          break;

      }

    }

    if (isset($gchild['children'])) {
      $ggchild = $this->buildChildTable($gchild, $i);

      $rows = array_merge($rows, $ggchild);
    }
    return $rows;

  }


  /**
   * Build and display sequence for select feature types.
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

    $seq = $returned[0]['residues'] ?? NULL;
    $header = $returned[0]['defline'] ?? $info->uniquename;

    if (!$returned[0]['residues']) {

      switch ($type) {

        case 'mRNA':
          // Build from child exons.
          $options['aggregate'] = 1;
          $options['sub_feature_types'] = ['exon'];
          $returned = chado_get_feature_sequences(['feature_id' => $info->feature_id], $options);
          $seq = $returned[0]['residues'] ?? NULL;
          break;

        case 'CDS':
          $options['derive_from_parent'] = 1;

          $header = $info->uniquename;

          $returned = chado_get_feature_sequences(['feature_id' => $info->feature_id], $options);
          $seq_string = '';
          foreach ($returned as $sequence) {
            if ($seq_string) {
              $sequence['residues'] = '<br>' . $sequence['residues'];
            }
            $seq_string .= $sequence['residues'];
          }
          $seq = $seq_string;
          break;

        default:
          break;
      }
    }
    $defline = '>' . $header . '<br>';

    $out = $defline . $seq;

    if ($out && $seq) {

      $container_id = 'popover-controller-' . $info->feature_id;
      $id = 'popover-' . $info->feature_id;

      $out = '
      <a id="' . $container_id . '"data-target="' . $id . '" class="sequence-expand-trigger">Sequence</a>
      <div style="display: none;" class="tripal-sequence-popover" id ="' . $id . '"> ' . $out . '</div>';
    }
    else {
      $out = 'Unavailable';
    }

    return $out;

  }

}
