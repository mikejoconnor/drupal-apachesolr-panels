<?php

if (module_exists('apachesolr_search')) {
  $plugin = array(
    'single' => TRUE,
    'title' => t('Apache Solr search results'),
    'icon' => 'icon_search.png',
    'description' => t('The results of an Apache Solr search.'),
    'required context' => new ctools_context_optional(t('Context to fetch keywords from'), 'string'),
    'category' => t('Apache Solr Search'),
    'defaults' => array(
      'keys_action' => 'none',
      'keys' => '',
      'filters_action' => 'none',
      'filters' => '',
      'sort_action' => 'none',
      'sort' => '',
      'empty_override' => FALSE,
      'empty_title' => '',
      'empty' => '',
      'empty_format' => FILTER_FORMAT_DEFAULT,
      'log' => TRUE,
      'substitute' => TRUE,
    ),
    'weight' => -10,
  );
}

/**
 * Render the search results.
 */
function apachesolr_panels_apachesolr_results_content_type_render($subtype, $conf, $args, $context) {
  $block = new stdClass();
  $block->module  = 'apachesolr_panels';
  $block->delta   = 'results';

  $search = array(
    'keys' => '',
    'filters' => $_GET['filters'],
    'sort' => $_GET['solrsort'],
  );

  if (!empty($context) && !empty($context->data)) {
    $search['keys'] = $context->data;
  }

  // Alter the search parts as per configuration.
  foreach ($search as $key => $value) {
    // Add keyword substitutions if configured to do so.
    if ($conf[$key .'_action'] != 'none' && !empty($conf['substitute']) && !empty($context)) {
      $conf[$key] = ctools_context_keyword_substitute($conf[$key], array(), $context);
    }

    // Alter the search part depending on selected action.
    if ($conf[$key .'_action'] == 'append') {
      $search[$key] .= ' '. $conf[$key];
    }
    elseif (($conf[$key .'_action'] == 'fallback' && empty($value)) || $conf[$key .'_action'] == 'override') {
      $search[$key] = $conf[$key];
    }
  }

  // Add remaining search parts.
  $menu_item = menu_get_item();
  $search['path'] = $menu_item['href'];
  $search['page'] = isset($_GET['page']) ? $_GET['page'] : 0;

  if (!empty($conf['log'])) {
    watchdog('search', '%keys (@type).', array('%keys' => $search['keys'], '@type' => t('Search')), WATCHDOG_NOTICE, l(t('results'), $_GET['q']));
  }

  // Execute the search.
  $results = search_data($search, 'apachesolr_panels');

  if ($results) {
    $block->title = t('Search results');
    $block->content = $results;
  }
  else {
    if (empty($conf['empty_override'])) {
      $block->title = t('Your search yielded no results');
      $block->content = search_help('search#noresults', drupal_help_arg());
    }
    else {
      $block->title = $conf['empty_title'];
      $block->content = check_markup($conf['empty'], $conf['empty_format'], FALSE);
    }
  }

  return $block;
}

/**
 * Return an edit form for the pane's settings.
 */
function apachesolr_panels_apachesolr_results_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];

  $form['keys_action'] = array(
    '#title' => t('Keys'),
    '#type' => 'select',
    '#options' => array(
      'none' => t('From context'),
      'override' => t('Override keys'),
      'append' => t('Append to keys'),
      'fallback' => t('Fallback if no keys set'),
    ),
    '#default_value' => $conf['keys_action'],
  );

  $form['keys'] = array(
    '#title' => '',
    '#type' => 'textfield',
    '#default_value' => $conf['keys'],
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-keys-action' => array('fallback', 'override', 'append')),
  );

  $form['filters_action'] = array(
    '#title' => t('Filters'),
    '#type' => 'select',
    '#options' => array(
      'none' => t('From URL'),
      'override' => t('Override filters'),
      'append' => t('Append to filters'),
      'fallback' => t('Fallback if no filters set'),
    ),
    '#default_value' => $conf['filters_action'],
  );

  $form['filters'] = array(
    '#title' => '',
    '#type' => 'textfield',
    '#default_value' => $conf['filters'],
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-filters-action' => array('fallback', 'override', 'append')),
  );

  $form['sort_action'] = array(
    '#title' => t('Sort'),
    '#type' => 'select',
    '#options' => array(
      'none' => t('From URL'),
      'override' => t('Override sort'),
      'append' => t('Append to sort'),
      'fallback' => t('Fallback if no sort set'),
    ),
    '#default_value' => $conf['sort_action'],
  );

  $form['sort'] = array(
    '#title' => '',
    '#type' => 'textfield',
    '#default_value' => $conf['sort'],
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-sort-action' => array('fallback', 'override', 'append')),
  );

  $form['empty_override'] = array(
    '#title' => t('Override "no result" text'),
    '#type' => 'checkbox',
    '#default_value' => $conf['empty_override'],
  );

  $form['empty_field']['empty_title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $conf['empty_title'],
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-empty-override' => array(1)),
  );

  $form['empty_field']['empty'] = array(
    '#title' => t('No result text'),
    '#type' => 'textarea',
    '#default_value' => $conf['empty'],
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-empty-override' => array(1)),
  );

  $form['empty_field']['format_prefix'] = array(
    '#type' => 'hidden',
    '#id' => 'edit-empty-format',
    '#prefix' => '<div><fieldset id="edit-empty-format" class="collapsed collapsible"><legend>' . t('Input format') . '</legend>',
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-empty-override' => array(1)),
  );
  // Yes, lots of gymnastics to make this fieldset work with dependencies.
  $form['empty_field']['empty_format'] = filter_form($conf['empty_format'], NULL, array('empty_format'));
  unset($form['empty_field']['empty_format']['#type']);

  $form['empty_field']['format_suffix'] = array(
    '#value' => '</fieldset></div>',
  );

  $form['log'] = array(
    '#title' => t('Record a watchdog log entry when searches are made'),
    '#type' => 'checkbox',
    '#default_value' => $conf['log'],
  );

  // Substitutions.
  if (!empty($form_state['contexts'])) {
    $form['substitute'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use context keywords'),
      '#description' => t('If checked, context keywords will be substituted in this content.'),
      '#default_value' => !empty($conf['substitute']),
    );
    $form['contexts'] = array(
      '#title' => t('Substitutions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $header = array(t('Keyword'), t('Value'));

    $rows = array();
    foreach ($form_state['contexts'] as $context) {
      foreach (ctools_context_get_converters('%' . check_plain($context->keyword) . ':', $context) as $keyword => $title) {
        $rows[] = array(
          check_plain($keyword),
          t('@identifier: @title', array('@title' => $title, '@identifier' => $context->identifier)),
        );
      }
    }

    $form['contexts']['context'] = array('#value' => theme('table', $header, $rows));
  }

  return $form;
}

/**
 * Handle submission of edit form.
 */
function apachesolr_panels_apachesolr_results_content_type_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}
