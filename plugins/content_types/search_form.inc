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
  $search = apachesolr_panels_get_search();

  $block = new stdClass();
  $block->module  = 'apachesolr_panels';
  $block->delta   = 'apachesolr_panels_search_form';
  $block->title   = '';
  $block->content = drupal_get_form('apachesolr_panels_search_form', $conf);

  return $block;
}

/**
 * User search form.
 */
function apachesolr_panels_search_form(&$form_state, &$conf) {
  global $base_url;

  $form['#action'] = $base_url .'/'. $conf['destination'];

  $form['keys'] = array('#title' => t('Search'), '#type' => 'textfield');

	if ($conf['show_keys']) {
	  $search = apachesolr_panels_get_search();
		$form['keys']['#default_value'] = $search['keys'];
	}

  $form['submit'] = array('#type' => 'submit', '#value' => t('Search'));

  return $form;
}

/**
 * Check for empty searches.
 */
function apachesolr_panels_search_form_validate($form_id, $form_values, $form){
  if (empty($form_values['values']['keys'])) {
    form_set_error('keys', t('Please enter some keywords.'));
  }
}

/**
 * Change redirect path for the form.
 */
function apachesolr_panels_search_form_submit($form, &$form_state) {
  $form_state['redirect'] = $form['#action'] .'/'. $form_state['values']['keys'];
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
