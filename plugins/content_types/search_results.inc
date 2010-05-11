<?php

if (module_exists('apachesolr_search')) {
  $plugin = array(
    'single' => TRUE,
    'title' => t('Search results'),
    'icon' => 'icon_search.png',
    'description' => t('The results of an Apache Solr search.'),
    'category' => t('Apache Solr Search'),
    'defaults' => array(
      'keys_context' => 'none',
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
    'all contexts' => TRUE,
  );
}

/**
 * Render the custom content type.
 */
function apachesolr_panels_search_results_content_type_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();
  $block->module  = 'apachesolr_panels';
  $block->delta   = 'results';

  $search = array(
    'keys' => '',
    'filters' => $_GET['filters'],
    'sort' => $_GET['solrsort'],
  );

  if ($conf['keys_context'] != 'none' && isset($contexts[$conf['keys_context']])) {
    $search['keys'] = $contexts[$conf['keys_context']]->data;
  }

  // Alter the search parts as per configuration.
  foreach ($search as $key => $value) {
    // Add keyword substitutions if configured to do so.
    if ($conf[$key .'_action'] != 'none' && !empty($conf['substitute']) && !empty($contexts)) {
      $conf[$key] = ctools_context_keyword_substitute($conf[$key], array(), $contexts);
    }

    if ($conf[$key .'_action'] == 'append') {
      $search[$key] .= ' '. $conf[$key];
    }
    elseif (($conf[$key .'_action'] == 'fallback' && empty($value)) || $conf[$key .'_action'] == 'override') {
      $search[$key] = $conf[$key];
    }
  }

  // Now we can add remaining search parts.
  $menu_item = menu_get_item();
  $search['path'] = $menu_item['path'];
  $search['page'] = isset($_GET['page']) ? $_GET['page'] : 0;

  // Store search for later use in other functions.
  apachesolr_panels_set_search($search);

  if (!empty($conf['log'])) {
    watchdog('search', '%keys (@type).', array('%keys' => $search['keys'], '@type' => t('Search')), WATCHDOG_NOTICE, l(t('results'), $_GET['q']));
  }

  // Perform search.
  $results = search_data($search['keys'], 'apachesolr_panels');

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
 * Return an edit form for custom type settings.
 */
function apachesolr_panels_search_results_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];

  // @todo What would CTools do?
  $keys_contexts = array('none' => t('None'));
  if (!empty($form_state['contexts'])) {
    foreach ($form_state['contexts'] as $context) {
      if ($context->type == 'string') {
        $keys_contexts[$context->id] = $context->identifier;
      }
    }
  }

  $form['keys_context'] = array(
    '#title' => t('Use keys from context'),
    '#type' => 'select',
    '#options' => $keys_contexts,
    '#default_value' => count($keys_contexts) > 0 ? end(array_keys($keys_contexts)) : $conf['keys_context'],
  );

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

    $rows = array();
    foreach ($form_state['contexts'] as $context) {
      foreach (ctools_context_get_converters('%' . check_plain($context->keyword) . ':', $context) as $keyword => $title) {
        $rows[] = array(
          check_plain($keyword),
          t('@identifier: @title', array('@title' => $title, '@identifier' => $context->identifier)),
        );
      }
    }
    $header = array(t('Keyword'), t('Value'));
    $form['contexts']['context'] = array('#value' => theme('table', $header, $rows));
  }

  return $form;
}

/**
 * Handle submission of edit form.
 */
function apachesolr_panels_search_results_content_type_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}
