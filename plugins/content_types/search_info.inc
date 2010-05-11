<?php

if (module_exists('apachesolr_search')) {
  $plugin = array(
    'single' => TRUE,
    'title' => t('Search info'),
    'description' => t('Information about an Apache Solr search.'),
    'icon' => 'icon_search.png',
    'category' => t('Apache Solr Search'),
    'hook theme' => 'apachesolr_panels_search_info_theme',
    'render last' => TRUE,
  );
}

/**
 * .
 */
function apachesolr_panels_search_info_theme(&$theme) {
  $theme['apachesolr_panels_search_info'] = array('arguments' => array('response' => NULL, 'search' => NULL));
}

/**
 * .
 */
function apachesolr_panels_search_info_content_type_render($subtype, $conf, $panel_args, $context) {
  $response = apachesolr_static_response_cache();

  if (empty($response)) {
    return;
  }

  $search = apachesolr_panels_get_search();

  $block = new stdClass();
  $block->module  = 'apachesolr_panels';
  $block->delta   = 'results';
  $block->title   = '';
  $block->content = theme('apachesolr_panels_search_info', $response, $search);

  return $block;
}

/**
 * Empty form so we can have the default override title.
 */
function apachesolr_panels_search_info_content_type_edit_form(&$form, &$form_state) {
  // Does nothing!
}

/**
 * .
 */
function theme_apachesolr_panels_search_info($response, $search) {
  if ($total = $response->response->numFound) {
    $start = $response->response->start + 1;
    $end = $response->response->start + count($response->response->docs);

    if (!empty($search['keys'])) {
      $info = t('Results %start - %end of %total for %keys', array('%start' => $start, '%end' => $end, '%total' => $total, '%keys' => $search['keys']));
    }
    else {
      $info = t('Results %start - %end of %total', array('%start' => $start, '%end' => $end, '%total' => $total));
    }

    return $info;
  }
}
