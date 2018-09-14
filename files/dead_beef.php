<?php

class _datex_views_handler_filter_date extends views_handler_filter_date {

  function option_definition() {
    $options = parent::option_definition();
    $options['datex_schema'] = ['default' => 'default'];
    return $options;
  }

  function options_form(&$form, &$form_state) {
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

  function _exposed_validate(&$form, &$form_state) {

    return parent::expose_validate($form, $form_state);
    //    $value = &$form_state['values'][$this->options['expose']['identifier']];
    //
    //    $operator = !empty($this->options['expose']['use_operator'])
    //    && !empty($this->options['expose']['operator_id'])
    //      ? $form_state['values'][$this->options['expose']['operator_id']]
    //      : $this->operator;
    //
    //    $operators = $this->operators();
    //
    //    if ($operators[$operator]['values'] == 1) {
    //      $convert = $this->_datex_strtotime($value['value']);
    //      if (!empty($form['value']) && ($convert == -1 || $convert === FALSE)) {
    //        form_error($form['value'], t('Invalid date format.'));
    //      }
    //    }
    //    elseif ($operators[$operator]['values'] == 2) {
    //      $min = $this->_datex_strtotime($value['min']);
    //      if ($min == -1 || $min === FALSE) {
    //        form_error($form['min'], t('Invalid date format.'));
    //      }
    //      $max = $this->_datex_strtotime($value['max']);
    //      if ($max == -1 || $max === FALSE) {
    //        form_error($form['max'], t('Invalid date format.'));
    //      }
    //    }
    //
  }

  function _datex_strtotime($value) {
    return -1;
  }

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
  }

}

/**
 * Implements hook_token_info().
 *
 * TODO per calendar
 * TODO move this to a file,
 * TODO if possible move the _datex_available_calendars to admin file.
 */
function datex_token_info() {
  if (_datex_is_disabled('token')) {
    return [];
  }


  foreach (_datex_available_calendars() as $cal => $cal_name) {
    $info['tokens']['datex'][$cal . '_now'] = t('!cal date (now).', ['!cal' => $cal_name]);
    $calendar = datex_factory(NULL, $cal);
    $calendar->setTimestamp(REQUEST_TIME);
  }

  $types['datex'] = [
    'name' => t("Localized Date (Datex)"),
    'description' => t(""),
  ];

  $format = variable_get('date_format_short');
  $date['short'] = [
    'name' => t("Short format"),
    'description' => t("A date in 'short' format. (%date)", ['%date' => $calendar->format($format)]),
  ];

  $format = variable_get('date_format_medium');
  $date['medium'] = [
    'name' => t("Medium format"),
    'description' => t("A date in 'medium' format. (%date)", ['%date' => $calendar->format($format)]),
  ];

  $format = variable_get('date_format_long');
  $date['long'] = [
    'name' => t("Long format"),
    'description' => t("A date in 'long' format. (%date)", ['%date' => $calendar->format($format)]),
  ];

  $date['custom'] = [
    'name' => t("Custom format"),
    'description' => t("A date in a custom format and a select calendar. See !php-date for details and check datex for available calendars.", ['!php-date' => l(t('the PHP documentation'), 'http://php.net/manual/en/function.date.php')]),
    'dynamic' => TRUE,
  ];

  $tokens = [
    'types' => $types,
    'tokens' => [
      'datex' => $date,
    ],
  ];

  return $tokens;
}

/**
 * Implements hook_tokens().
 */
// TODO
function datex_tokens($type, $tokens, array $data = [], array $options = []) {
  if (_datex_is_disabled('token')) {
    return [];
  }

  $replacements = [];

  if ($type == 'datex') {
    $date = empty($data['date']) ? REQUEST_TIME : $data['date'];
    $c = datex_factory(NULL, 'persian');
    $c->setTimestamp($date);

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'short':
          $format = variable_get('date_format_short');
          $replacements[$original] = $c->format($format);
          break;

        case 'medium':
          $format = variable_get('date_format_short');
          $replacements[$original] = $c->format($format);
          break;

        case 'long':
          $format = variable_get('date_format_short');
          $replacements[$original] = $c->format($format);
          break;
      }
    }
    if ($created_tokens = token_find_with_prefix($tokens, 'custom')) {
      foreach ($created_tokens as $name => $original) {
        list($locale, $format) = @explode(':', $name);
        if (isset($format) && _datex_calendar_is_valid($locale)) {
          $c = datex_factory(NULL, $locale);
          $c->setTimestamp($date);
          $replacements[$original] = $c->format($format);
        }
        else {
          watchdog(WATCHDOG_WARNING, 'Invalid token arguments for datex. Format is not given or requested calerndar is not available. Token replacement ignored.');
        }
      }
    }
  }

  return $replacements;
}

