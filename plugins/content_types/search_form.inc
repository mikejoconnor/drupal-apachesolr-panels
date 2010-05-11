<?php

if (module_exists('apachesolr_search')) {
  $plugin = array(
    'single' => TRUE,
    'title' => t('Search form'),
    'icon' => 'icon_search.png',
    'description' => t('Search form for Apache Solr.'),
    'category' => t('Apache Solr Search'),
    'defaults' => array(
      'destination' => '',
      'prompt' => '',
      'show_keys' => FALSE,
    ),
    'no title override' => TRUE,
    'render last' => TRUE,
  );
}

/**
 * .
 */
function apachesolr_panels_search_form_content_type_render($subtype, $conf, $panel_args, $context) {
  $search = apachesolr_panels_get_search();
  if ($conf['destination']) {

  }
  $promt = !empty($conf['prompt']) ? $conf['prompt'] : NULL;

  $block = new stdClass();
  $block->module  = 'apachesolr_panels';
  $block->delta   = 'search_form';
  $block->title   = '';
  $block->content = drupal_get_form('search_form', $search['path'], $search['keys'], 'apachesolr_panels', $prompt);

  return $block;
}

/**
 * .
 */
function apachesolr_panels_search_form_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];

  $form['destination'] = array(
    '#title' => t('Search page'),
    '#type' => 'textfield',
    '#default_value' => $conf['destination'],
  );

  $form['show_keys'] = array(
    '#title' => t('Show search keys in field'),
    '#type' => 'checkbox',
    '#default_value' => $conf['show_keys'],
  );

  return $form;
}
