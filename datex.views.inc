<?php


/**
 * Implementation of hook_views_api().
 */
function datex_views_api() {
  return [
    'api' => 3,
    'path' => drupal_get_path('module', 'datex'),
  ];
}

/**
 * Implements hook_views_data_alter().
 */
function datex_views_data_alter(&$data) {
  if (_datex_is_disabled('views') || _datex_is_patching_mode()) {
    return;
  }

  foreach ($data as $key => &$value) {
    if (is_array($value)) {
      datex_views_data_alter($value);
    }

    if (substr($key, 0, strlen('date_views')) === 'date_views') {
      dpm($value, $key);
    }

    $change = [
      'views_handler_field_date',
      'views_handler_filter_date',
      'views_handler_field_last_comment_timestamp',
      'views_handler_field_ncs_last_updated',
      'views_handler_filter_ncs_last_updated',
      'views_handler_filter_history_user_timestamp',
      'views_handler_field_history_user_timestamp',
      'views_handler_argument_node_created_year',
    ];
    foreach ([
               'year',
               'month',
               'day',
               'fulldate',
               'year_month',
             ] as $item) {
      $change[] = 'views_handler_argument_node_created_' . $item;
    }
    if (in_array($value, $change, TRUE)) {
      $value = '_datex_' . $value;
    }

    foreach (['fulldate', 'year_month', 'year', 'month', 'day', 'week'] as $g) {
      foreach ([
                 'views_handler_argument_node_created_',
                 'views_handler_argument_node_changed_',
               ] as $item) {
        if ($value === $item . $g) {
          $value = '_datex_' . $value;
        }
      }
    }
  }
}

class _datex_views_handler_argument_node_created_year extends views_handler_argument_node_created_year {

}

class _datex_views_handler_argument_node_created_month extends views_handler_argument_node_created_month {

}

class _datex_views_handler_argument_node_created_week extends views_handler_argument_node_created_week {

}

class _datex_views_handler_argument_node_created_day extends views_handler_argument_node_created_day {

}

class _datex_views_handler_argument_node_created_year_month extends views_handler_argument_node_created_year_month {

}

class _datex_views_handler_argument_node_created_fulldate extends views_handler_argument_node_created_fulldate {

}

class _datex_views_handler_field_date extends views_handler_field_date {

  // In this cases, Everything will be handled properly by parent and there
  // is no need for datex to interfere with views original handler.
  private static $parent_handled = [
    'raw time ago',
    'time ago',
    'raw time hence',
    'time hence',
    'raw time span',
    'inverse time span',
    'time span',
  ];

  /**
   * Datex specific options added to date fields.
   */
  function option_definition() {
    // Default option is to follow default global schema.
    $options = parent::option_definition();
    $options['datex_schema'] = ['default' => 'default'];
    return $options;
  }

  /**
   * Schema to use for the displayed date.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['datex_schema'] = [
      '#type' => 'select',
      '#title' => t('Datex schema'),
      '#options' => _datex_schema_form_options(),
      '#default_value' => isset($this->options['datex_schema']) ? $this->options['datex_schema'] : 'default',
    ];
  }

  public function render($values) {
    $schema = isset($this->options['datex_schema'])
      ? $this->options['datex_schema']
      : 'default';

    if (_datex_is_patching_mode()
      || in_array($this->options['date_format'], self::$parent_handled)
      || $schema === 'disabled') {
      return parent::render($values);
    }

    $value = $this->get_value($values);
    if (!$value) {
      return parent::render($values);
    }

    $tz = empty($this->options['timezone'])
      ? drupal_get_user_timezone()
      : $this->options['timezone'];

    $calendar = datex_factory($tz, _datex_language_calendar($schema));
    if (!$calendar) {
      return parent::render($values);
    }

    $format = NULL;
    $custom_format = $this->options['custom_date_format'];
    if (!$custom_format) {
      $format = variable_get('date_format_' . $this->options['date_format'], '');
    }
    else {
      $format = $custom_format;
    }
    if (!$format) {
      return parent::render($values);
    }

    $calendar->setTimestamp($value);
    return $calendar->format($format);
  }

  //    $format = $this->options['date_format'] === 'custom'
  //      ? $this->options['custom_date_format']
  //      : variable_get('date_format_' . $this->options['date_format']);

}

class _datex_views_handler_filter_date extends views_handler_filter_date {

  function option_definition() {
    $options = parent::option_definition();
    $options['datex_schema'] = ['default' => 'default'];
    $options['datex_format'] = ['default' => 'short'];
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $date_formats = [];
    $date_types = system_get_date_types();
    foreach ($date_types as $key => $value) {
      $date_formats[$value['type']] = t('@date_format format',
          ['@date_format' => $value['title']]) . ': ' .
        format_date(REQUEST_TIME, $value['type']);
    }

    $form['datex_format'] = [
      '#type' => 'select',
    ];
    $form['date_format'] = [
      '#type' => 'select',
      '#title' => t('Date format'),
      '#options' => $date_formats,
      '#default_value' => isset($this->options['date_format']) ? $this->options['date_format'] : 'small',
    ];

    $form['datex_schema'] = [
      '#type' => 'select',
      '#title' => t('Datex schema'),
      '#options' => _datex_schema_form_options(),
      '#default_value' => isset($this->options['datex_schema'])
        ? $this->options['datex_schema']
        : 'default',
    ];

    $tvar = ['@fmt' => 'Y-m-d H:i:s', '@eg' => '2017-11-30 23:20:10'];
    $tmsg = '<b>Without Datex-Popup</b>, the <b>ONLY</b> format supported is @fmt E.g: @eg';
    $form['datex_attention'] = [
      '#type' => 'item',
      '#markup' => t($tmsg, $tvar),
    ];

    parent::options_form($form, $form_state);
  }

  function exposed_validate(&$form, &$form_state) {
    //    dpm($form, 'ex val f');
    //    dpm($form_state, 'ex val fs');
    $value = &$form_state['values'][$this->options['expose']['identifier']];
    //    dpm($value, 'value');

    $operator = !empty($this->options['expose']['use_operator'])
    && !empty($this->options['expose']['operator_id'])
      ? $form_state['values'][$this->options['expose']['operator_id']]
      : $this->operator;

    $operators = $this->operators();

    if ($operators[$operator]['values'] == 1) {
      $convert = strtotime($value['value']);
      if (!empty($form['value']) && ($convert == -1 || $convert === FALSE)) {
        form_error($form['value'], t('Invalid date format.'));
      }
    }
    elseif ($operators[$operator]['values'] == 2) {
      $min = strtotime($value['min']);
      if ($min == -1 || $min === FALSE) {
        form_error($form['min'], t('Invalid date format.'));
      }
      $max = strtotime($value['max']);
      if ($max == -1 || $max === FALSE) {
        form_error($form['max'], t('Invalid date format.'));
      }
    }
  }

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    //    dpm($form, 'exposed');
  }
}
