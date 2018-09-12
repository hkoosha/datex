<?php

/**
 * Provides administration form for datex module (menu callback).
 */
function datex_admin_form($form, $form_state) {
  $schema_options = [];
  foreach (array_keys(variable_get('datex_schema', [])) as $name) {
    $summary = '';
    $schema = variable_get('datex_schema');
    foreach ($schema[$name] as $sss_lang => $sss_display) {
      $available = _datex_available_calendars();
      $summary .= ' - ' . t('In language [@lang] display in [@display]', [
          '@display' => $available[$sss_display],
          '@lang' => $sss_lang,
        ]);
    }
    $path = 'admin/config/regional/date-time/datex/edit/' . $name;
    $schema_options[$name] = [
      'title' => [
        'data' => [
          '#type' => 'link',
          '#title' => $name,
          '#href' => $path,
          '#options' => ['inline'],
        ],
      ],
      'summary' => empty($summary) ? t('Disabled') : nl2br($summary),
    ];
  }
  $form['schema_manage'] = [
    '#type' => 'fieldset',
    '#description' => t("Mark schemas to delete after saving, click it's name to edit."),
    '#title' => t('Manage Schemas'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#group' => 'datex_config',
  ];
  $form['schema_manage']['datex_schemas'] = [
    '#type' => 'tableselect',
    '#header' => ['title' => t('Title'), 'summary' => t('Summary')],
    '#options' => $schema_options,
  ];
  $form['schema_manage']['datex_new_schema_machine_name'] = [
    '#title' => t('New schema'),
    '#type' => 'machine_name',
    '#maxlength' => 31,
    '#description' => t('Schema machine readable name containing only letters, numbers and underscores.'),
    '#machine_name' => [
      'exists' => '_datex_machine_name_exists',
    ],
    '#required' => FALSE,
  ];

  $form['mode_fieldset'] = [
    '#type' => 'fieldset',
    '#title' => t('Mode'),
    '#collapsible' => !_datex_is_patching_mode() && DATEX_USE_INTL ? TRUE : FALSE,
    '#collapsed' => !_datex_is_patching_mode() && DATEX_USE_INTL ? TRUE : FALSE,
  ];
  $form['mode_fieldset']['datex_api_intl_status'] = [
    '#type' => 'markup',
    '#title' => t('PHP-Intl status'),
    '#markup' => !DATEX_USE_INTL
      ? t('<b>PHP-Intl is not available in this environment. It is ' .
        "highly recommended that you enable it, else you'll have limited " .
        "functionality.</b>")
      : t('PHP-Intl is available.'),
  ];
  $form['mode_fieldset']['datex_mode'] = [
    '#type' => 'radios',
    '#title' => t('Date integration mode'),
    '#default_value' => _datex_is_patching_mode() ? 1 : 0,
    '#description' => t('The mode which datex uses to integrate with ' .
      'Drupal. Patching mode needs patching a core file and might be needed ' .
      'in some rare extreme cases. It is unsafe and dangerous! ' .
      'It is highly recommended to use non-patching mode.' .
      "- <b>NOT RECOMMENDED</b>, you've been warned"
    ),
    '#options' => [
      0 => t('Operate in non-patching mode'),
      1 => t('Operate in patching mode (Patching of the file "common.inc" is required)'),
    ],
  ];

  $disablable = [
    'datex' => t('Datex (globally)'),
    'views' => t('Views (date field handler, only applicable in' .
      ' non-patching mode [hook_views_data_alter]'),
    'datex_hook' => t('datex hook, only applicable in patching mode [hook_datex_format_alter]'),
    'token' => t('Tokens [hook_token_info]'),
    'libraries' => t('Libraries [hook_libraries_info]'),
    'node' => t('Node ("published on" display) [hook_preprocess_node]'),
    'node_edit' => t('Node Edit Form (authored on) [form_alter]'),
    'node_admin' => t('Node admin ("updated" column display)[via hook_form_alter]'),
    'comment' => t('Comment ("published on" display) [hook_preprocess_comment]'),
    'comment_edit' => t('Comment Edit Form (authored on) [form_alter]'),
    'date' => t('date module (disables date_popup too)'),
    'popup' => t("date_popup module"),
    'js' => t("jQuery Calendar (loading the JS library into page) [drupal_add_js/libraries_load]"),
  ];
  $disabled = [];
  foreach (variable_get('datex_disable') as $name => $is_disabled) {
    if ($is_disabled) {
      $disabled[$name] = $name;
    }
  }
  $form['disable_individually'] = [
    '#type' => 'fieldset',
    '#description' => t("Select datex components to be disabled."),
    '#title' => t('Disable Datex Components'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'datex_config',
  ];
  $form['disable_individually']['do_not_touch'] = [
    '#type' => 'item',
    '#markup' => '<b>' . t('ATTENTION: Dragons ahead! DO NOT select any of these options if you do not know what they do, or you will BREAK your site.') . '</b>',
  ];
  $form['disable_individually']['datex_disable'] = [
    '#type' => 'checkboxes',
    '#title' => '',
    '#default_value' => $disabled,
    '#options' => $disablable,
  ];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#submit' => ['_datex_admin_form_submit'],
    '#value' => t('Save'),
    '#name' => 'config',
  ];
  return $form;
}

/**
 * Form submit for datex admin form.
 *
 * This admin form could be implemented better.
 */
function _datex_admin_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  variable_set('datex_mode', $v['datex_mode'] == 1 || $v['datex_mode'] == '1' ? 1 : 0);
  variable_set('datex_popup_js', $v['datex_popup_js']);

  $schema = variable_get('datex_schema');

  $newName = empty($v['datex_new_schema_machine_name']) ? '' :
    $v['datex_new_schema_machine_name'];
  if ($newName) {
    $schema[$newName] = [];
    drupal_set_message(t("New schema created: @s", ['@s' => $newName]));
  }

  foreach ($v['datex_schemas'] as $select) {
    if ($select !== 'default' && isset($schema[$select])) {
      unset($schema[$select]);
      drupal_set_message(t('Schema removed') . ': ' . $select);
    }
    else {
      if ($select === 'default') {
        $schema['default'] = [];
        $message = t('Default schema was reset.');
        drupal_set_message($message);
      }
    }
  }
  variable_set('datex_schema', $schema);

  $disabled = variable_get('datex_disable', []);
  $disable_changed = FALSE;
  foreach ($v['datex_disable'] as $name => $new_dis_val) {
    if (!isset($disabled[$name]) || $disabled[$name] != $new_dis_val) {
      $disabled[$name] = $new_dis_val ? TRUE : FALSE;
      $disable_changed = TRUE;
    }
  };
  variable_set('datex_disable', $disabled);
  if ($disable_changed) {
    $disabled = [];
    foreach (variable_get('datex_disable') as $name => $is_disabled) {
      if ($is_disabled) {
        $disabled[] = $name;
      }
    }
    $disabled = implode(', ', $disabled);
    if ($disabled) {
      drupal_set_message(t("Datex components disabled: @c", ['@c' => $disabled]));
    }
    drupal_set_message(t('You must clear the caches for changes to take effect!'), 'warning');
  }

  drupal_set_message(t('Configuration saved.'));

  if ($newName) {
    drupal_goto('admin/config/regional/date-time/datex/edit/' . $newName);
  }
}

/**
 * Datex form element validation for schema name.
 */
function _datex_machine_name_exists($value, $element, $form_state) {
  $machine = $form_state['values']['datex_new_schema_machine_name'];
  return $machine === 'default'
    || $machine === 'disabled'
    || in_array($machine, array_keys(variable_get('datex_schema', [])));
}

/**
 * Menu callback, Admin form for editing datex schema.
 */
function datex_schema_edit_form($form, $form_state, $name) {
  $list = array_keys(variable_get('datex_schema', []));
  if (!in_array($name, $list)) {
    $form['notfound'] = [
      '#markup' => t('Not found'),
    ];
    return $form;
  }

  $schema = variable_get('datex_schema');
  $default = $schema[$name];

  $form['schemaname'] = [
    '#type' => 'markup',
    '#markup' => '<h2> Editing: ' . '<b>' . check_plain($name) . '</b></h2>',
  ];
  $form['name'] = [
    '#type' => 'value',
    '#value' => $name,
  ];

  foreach (language_list() as $code => $lang) {
    $t = ['@name' => $lang->name, '@native' => $lang->native];
    $form['datex_' . $code] = [
      '#type' => 'select',
      '#title' => t('Used calendar in @name (@native)', $t),
      '#options' => ['' => 'Disabled'] + _datex_available_calendars(),
      '#default_value' => isset($default[$code]) ? $default[$code] : '',
    ];
  }

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => ['_datex_schema_edit_form_submit'],
  ];

  return $form;
}

/**
 * Form submit callback for: datex_schema_edit_form().
 */
function _datex_schema_edit_form_submit($form, $form_state) {
  $v = $form_state['values'];
  $name = $v['name'];
  $schema = variable_get('datex_schema');

  foreach (language_list() as $lang) {
    $code = $lang->language;
    if ($v['datex_' . $code] === '') {
      if (isset($schema[$name][$code])) {
        unset($schema[$name][$code]);
      }
    }
    else {
      $schema[$name][$code] = $v['datex_' . $code];
    }
  }

  variable_set('datex_schema', $schema);

  $message = t('New configuration for @schema schema saved.', ['@schema' => $name]);
  drupal_set_message($message);

  drupal_goto('admin/config/regional/date-time/datex');
}

/**
 * List of css files which jquery calendar has.
 */
function _datex_api_css_files_list() {
  return [
    //    'ui-black-tie.calendars.picker.css',
    //    'ui-blitzer.calendars.picker.css',
    //    'ui-cupertino.calendars.picker.css',
    //    'ui-dark-hive.calendars.picker.css',
    //    'ui-dot-luv.calendars.picker.css',
    //    'ui-eggplant.calendars.picker.css',
    //    'ui-excite-bike.calendars.picker.css',
    //    'ui-flick.calendars.picker.css',
    //    'ui-hot-sneaks.calendars.picker.css',
    //    'ui-humanity.calendars.picker.css',
    //    'ui-le-frog.calendars.picker.css',
    //    'ui-mint-choc.calendars.picker.css',
    //    'ui-overcast.calendars.picker.css',
    //    'ui-pepper-grinder.calendars.picker.css',
    //    'ui-redmond.calendars.picker.css',
    //    'ui-smoothness.calendars.picker.css',
    //    'ui-south-street.calendars.picker.css',
    //    'ui-start.calendars.picker.css',
    //    'ui-sunny.calendars.picker.css',
    //    'ui-swanky-purse.calendars.picker.css',
    //    'ui-trontastic.calendars.picker.css',
    //    'ui-ui-darkness.calendars.picker.css',
    //    'ui-ui-lightness.calendars.picker.css',
    //    'ui-vader.calendars.picker.css',

    'ui.calendars.picker.css',

    'flora.calendars.picker.css',
    'humanity.calendars.picker.css',
    'jquery.calendars.picker.css',
    'redmond.calendars.picker.css',
    'smoothness.calendars.picker.css',
  ];
}
