<?php

if (module_exists('apachesolr_search')) {
  ctools_include('content_types/block/block', 'ctools', 'plugins');

  $plugin = array(
    'title' => t('Block'),
    'render last' => TRUE,
  );
}

/**
 * Return all Apache Solr blocks.
 */
function apachesolr_panels_search_blocks_content_type_content_types() {
  $types = array();
  $search_modules = array('apachesolr', 'apachesolr_search');
  foreach ($search_modules as $module) {
    $module_blocks = module_invoke($module, 'block', 'list');
    if ($module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        $types["$module-$delta"] = array(
          'title' => strip_tags($block['info']),
          'category' => t('Apache Solr Search'),
        );
      }
    }
  }
  return $types;
}

/**
 * .
 */
function apachesolr_panels_search_blocks_content_type_render($subtype, $conf) {
  return ctools_block_content_type_render($subtype, $conf);
}

/**
 * Empty form so we can have the default override title.
 */
function apachesolr_panels_search_blocks_content_type_edit_form(&$form, &$form_state) {
  return ctools_block_content_type_edit_form(&$form, &$form_state);
}

/**
 * Returns the administrative title for a type.
 */
function apachesolr_panels_search_blocks_content_type_admin_title($subtype, $conf) {
  return ctools_block_content_type_admin_title($subtype, $conf);
}

/**
 * Output function for the 'block' content type. Outputs a block based on the
 * module and delta supplied in the configuration.
 */
function apachesolr_panels_search_blocks_content_type_admin_info($subtype, $conf) {
  return ctools_block_content_type_admin_info($subtype, $conf);
}
