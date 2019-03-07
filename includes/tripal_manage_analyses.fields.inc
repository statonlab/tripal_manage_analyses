<?php

/**
 * @file
 * Contains all field specific code outside the classes.
 */

/**
 * Implements hook_bundle_fields_info().
 *
 *
 * @param $entity_type
 *   This should be 'TripalEntity' for all Tripal Content.
 * @param $bundle
 *   This object describes the Type of Tripal Entity (e.g. organism or Gene)
 *   this hook is being called for. However, since this hook creates field
 *   types (by definition not tied to a specific Tripal Content Type (bundle))
 *   and since a field type will only be created if it doesn't already exist,
 *   this parameter doesn't actually matter. NOTE: If you do need to determine
 *   the bundle in this hook, we suggest inspecting the data_table since the
 *   label can be changed by site administrators.
 *
 * @return
 *   An array of field definitions. Each field in this array will be created if
 *   it doesn't already exist. To trigger create of fields when developing call
 *   tripal_refresh_bundle_fields() for the specific bundle.
 */
function tripal_manage_analyses_bundle_fields_info($entity_type, $bundle) {
  $fields = [];

  if (isset($bundle->data_table) and ($bundle->data_table == 'feature') and $bundle->term->name == 'gene') {

  }

  // Organism linker on analysis.
  if (isset($bundle->data_table) and ($bundle->data_table == 'analysis')) {
    $field_name = 'obi__organism_linker';
    $field_type = 'obi__organism_linker';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

  }

  // Organism linker on map.
  if (isset($bundle->data_table) and ($bundle->data_table == 'featuremap')) {

    $field_name = 'obi__organism_linker';
    $field_type = 'obi__organism_linker';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  if (isset($bundle->data_table) and ($bundle->data_table == 'organism')) {
    // First add my term.
    tripal_insert_cvterm([
      'id' => 'local:analysis_browser',
      'name' => 'analysis_browser',
      'cv_name' => 'local',
      'definition' => 'Display a set of analyses related to an organism',
    ]);

    // Then describe the field defined by that term.
    $field_name = 'local__analysis_browser';
    $field_type = 'local__analysis_browser';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

    tripal_insert_cvterm([
      'id' => 'ero:0000160',
      'name' => 'nucleic_acid_library',
      'cv_name' => 'ero',
      'definition' => 'Reagent library that is a collection of DNA fragments that is stored and propagated in a population of micro-organisms through the process of molecular cloning.',
    ]);

    // Then describe the field defined by that term.
    $field_name = 'ero__nucleic_acid_library';
    $field_type = 'ero__nucleic_acid_library';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

    tripal_insert_cvterm([
      'id' => 'local:transcriptome_viewer',
      'name' => 'transcriptome_viewer',
      'cv_name' => 'local',
      'definition' => 'The transcriptome associated with this entity',
    ]);

    // Then describe the field defined by that term.
    $field_name = 'local__transcriptome_viewer';
    $field_type = 'local__transcriptome_viewer';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

    tripal_insert_cvterm([
      'id' => 'local:genome_browser',
      'name' => 'genome_browser',
      'cv_name' => 'local',
      'definition' => 'List the genomes for an organism',
    ]);

    // Then describe the field defined by that term.
    $field_name = 'local__genome_browser';
    $field_type = 'local__genome_browser';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

  }

  return $fields;
}

/**
 * Implements hook_bundle_instances_info().
 *
 * This hook tells Drupal/Tripal to create a field instance of a given field
 * type on a specific Tripal Content type (otherwise known as the bundle). Make
 * sure to implement hook_bundle_create_fields() to create your field type
 * before trying to create an instance of that field.
 *
 * @param $entity_type
 *   This should be 'TripalEntity' for all Tripal Content.
 * @param $bundle
 *   This object describes the Type of Tripal Entity (e.g. organism or Gene)
 *   the field instances are being created for. Thus this hook is called once
 *   per Tripal Content Type on your site. The name of the bundle is the
 *   machine name of the type (e.g. bio_data_1) and the label of the bundle
 *   (e.g. organism) is what you see in the interface. Since the label can be
 *   changed by site admin, we suggest checking the data_table to determine if
 *   this is the entity you want to add field instances to.
 *
 * @return
 *   An array of field instance definitions. This is where you can define the
 *   defaults for any settings you use in your field. Each entry in this array
 *   will be used to create an instance of an already existing field.
 */
function tripal_manage_analyses_bundle_instances_info($entity_type, $bundle) {
  $instances = [];

  if (isset($bundle->data_table) and ($bundle->data_table == 'feature') and $bundle->term->name == 'gene') {


  }

  if (isset($bundle->data_table) and ($bundle->data_table == 'analysis')) {

    $field_name = 'obi__organism_linker';
    $field_type = 'obi__organism_linker';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Organism',
      'description' => 'Organism associated with this entity',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'OBI',
        'term_name' => 'organism',
        'term_accession' => '0100026',
        'auto_attach' => FALSE,
        'chado_table' => 'organism_analysis',
        'chado_column' => 'analysis_id',
        'base_table' => 'analysis',
      ],
      'widget' => [
        'type' => 'obi__organism_linker_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'obi__organism_linker_formatter',
          'settings' => [],
        ],
      ],
    ];

  }

  // FEATUREMAPS
  // ==============.
  if (isset($bundle->data_table) and ($bundle->data_table == 'featuremap')) {

    $field_name = 'obi__organism_linker';
    $field_type = 'obi__organism_linker';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Organism',
      'description' => 'Organism associated with this map.',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'OBI',
        'term_name' => 'organism',
        'term_accession' => '0100026',
        'auto_attach' => FALSE,
        'chado_table' => 'featuremap_organism',
        // Chado table is what its inserting into.
        'chado_column' => 'featuremap_id',
        'base_table' => 'featuremap',
        // The bundle's base table i think?
      ],
      'widget' => [
        'type' => 'obi__organism_linker_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'obi__organism_linker_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  // ORGANISM.
  // ===============.
  if (isset($bundle->data_table) and ($bundle->data_table == 'organism')) {
    $field_name = 'local__analysis_browser';
    $field_type = 'local__analysis_browser';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Analyses',
      'description' => 'Display a set of analyses related to an organism',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'local',
        'term_name' => 'analysis_browser',
        'term_accession' => 'analysis_browser',
        'auto_attach' => FALSE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'organism_id',
        'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'local__analysis_browser_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'local__analysis_browser_formatter',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'ero__nucleic_acid_library';
    $field_type = 'ero__nucleic_acid_library';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Libraries',
      'description' => 'Displays libraries (BAC, YAC etc) associated with this entity.',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'ero',
        'term_name' => 'nucleic_acid_library',
        'term_accession' => '0000160',
        'auto_attach' => FALSE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'organism_id',
        'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'ero__nucleic_acid_library',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'ero__nucleic_acid_library',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'local__transcriptome_viewer';
    $field_type = 'local__transcriptome_viewer';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Transcriptome',
      'description' => 'The transcriptome associated with this entity',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'local',
        'term_name' => 'transcriptome_viewer',
        'term_accession' => 'transcriptome_viewer',
        'auto_attach' => FALSE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'organism_id',
        'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'local__transcriptome_viewer_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'local__transcriptome_viewer_formatter',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'local__genome_browser';
    $field_type = 'local__genome_browser';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Genome Browser',
      'description' => 'List the genomes for an organism',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'local',
        'term_name' => 'genome_browser',
        'term_accession' => 'genome_browser',
        'auto_attach' => FALSE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'organism_id',
        'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'local__genome_browser_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'local__genome_browser_formatter',
          'settings' => [],
        ],
      ],
    ];

  }

  return $instances;
}
