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
      'show_keys' => TRUE,
    ),
    'no title override' => TRUE,
    'render last' => TRUE,
  );
}

/**
 * .
 */
function apachesolr_panels_search_form_content_type_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();
  $block->module  = 'apachesolr_panels';
  $block->delta   = 'apachesolr_panels_search_form';
  $block->title   = '';
  $block->content = drupal_get_form('apachesolr_panels_search_form_form', $conf);
  return $block;
}

/**
 * Panel plugin edit form.
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

/**
 * Handle submission of edit form.
 */
function apachesolr_panels_search_form_content_type_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

/**
 * Search form.
 */
function apachesolr_panels_search_form_form(&$form_state, $conf) {
  $form = drupal_retrieve_form('search_block_form', $form_state);

  $form['#action'] = $GLOBALS['base_url'] .'/'. $conf['destination'];
  $form['#submit'] = 'apachesolr_panels_search_form_form_submit';

  if ($conf['show_keys'] && ($search = apachesolr_panels_get_search())) {
    $form['search_block_form']['#default_value'] = $search['keys'];
  }

  return $form;
}

/**
 * Redirect to specified destination.
 */
function apachesolr_panels_search_form_form_submit($form, &$form_state) {
  $form_state['redirect'] = $form['#action'] .'/'. $form_state['values']['keys'];
}
