<?php

/**
 * @file
 * Provides support for date module.
 */

define('DATEX_HIDDEN_VALUE_FMT', 'Y-n-j');

/**
 * Checks to see if a field is supported.
 *
 * If month is present, Then it's supported. But if month is not present then
 * none of smaller granularities should be present either, Otherwise it's
 * impossible to calculate correct date, Since date module sets month to 1.
 *
 * it's still possible to support it. But I'm really tired to do so, and who
 * wants year and day, without month?!
 */
function _datex_date_granularity_supported($g) {
  if (is_string($g)) {
    return in_array($g, ['year', 'month']);
  }
  // Year is always there.
  return count($g) < 2 || in_array('month', $g);
}

/**
 * Extract granularity from element (probably a form element).
 */
function _datex_field_granularity($element, $is_popup = FALSE) {
  if (isset($element['#granularity'])) {
    return $element['#granularity'];
  }
  elseif (isset($element['field']['settings']['granularity'])) {
    $granularities = array_values($element['field']['settings']['granularity']);
    foreach ($granularities as $index => $value) {
      if ($value === 0) {
        unset($granularities[$index]);
      }
    }
    return $granularities;
  }
  elseif ($is_popup) {
    return [
      date_popup_date_granularity($element),
      date_popup_time_granularity($element),
    ];
  }
  else {
    return NULL;
  }
}

/**
 * Find the datex schema set for a field, or the global schema of none is set.
 */
function _datex_field_schema($e) {
  $ret = NULL;
  if (isset($elementp['#instance']['settings']['datex_schema'])) {
    $ret = $element['#instance']['settings']['datex_schema'];
  }
  elseif (isset($e['#instance']['widget']['settings']['datex_schema'])) {
    $ret = $e['#instance']['widget']['settings']['datex_schema'];
  }
  elseif (isset($e['display']['settings']['datex_schema'])) {
    $ret = $e['display']['settings']['datex_schema'];
  }
  elseif (isset($e['view_mode']) &&
    $e['instance']['display'][$e['view_mode']]['settings']['datex_schema']) {
    $ret = $e['instance']['display'][$e['view_mode']]['settings']['datex_schema'];
  }
  else {
    $ret = 'default';
  }
  return $ret ? $ret : 'default';
}

/**
 * Create a datex-calendar based on field config and the schema set for it.
 */
function _datex_field_calendar($element, $is_popup = FALSE) {
  $schema = _datex_field_schema($element);
  if ($schema === 'disabled') {
    return NULL;
  }

  list($date_granule, $time_granule) = _datex_field_granularity($element, $is_popup);
  if (!_datex_date_granularity_supported($date_granule)) {
    return NULL;
  }

  $tz = isset($element['#date_timezone']) ? $element['#date_timezone'] : NULL;
  return datex_factory($tz, _datex_language_calendar($schema));
}

function _datex_fix_missing_granularity($calendar, DateObject $dt, array $g) {
  switch ($calendar) {
    case 'persian':
      $missing_month_default = 2;
      $missing_day_default = 2;
      if (!in_array('month', $g)) {
        $dt->setDate(
          $dt->format('Y'),
          $missing_month_default,
          $missing_day_default
        );
      }
      elseif (!in_array('day', $g)) {
        $dt->setDate(
          $dt->format('Y'),
          $dt->format('n'),
          $missing_day_default
        );
      }
      break;
  }
}

function _datex_fix_missing_granularity_locale(DatexInterface $calendar, array $g) {
  switch ($calendar) {
    case 'persian':
      $missing_month_default = 3;
      $missing_day_default = 20;
      $arr = $calendar->formatArray();
      if (!in_array('month', $g)) {
        $calendar->setDateLocale(
          $arr['year'],
          $missing_month_default,
          $missing_day_default);
      }
      elseif (!in_array('day', $g)) {
        $calendar->setDateLocale(
          $arr['year'],
          $arr['month'],
          $missing_day_default);
      }
      break;
  }
}

function _datex_get_fs(&$element, &$fs, $extra_parent = NULL, $is_input = TRUE) {
  $key = $is_input ? 'input' : 'value';
  if (!isset($fs[$key]) || !is_array($fs[$key])) {
    return NULL;
  }

  $dummy = NULL;
  $parents = $element['#parents'];
  $nested = drupal_array_get_nested_value($fs[$key], $parents, $dummy);

  if (is_string($nested)) {
    return NULL;
  }
  elseif ($extra_parent) {
    return isset($nested[$extra_parent]) ? trim($nested[$extra_parent]) : NULL;
  }
  else {
    return $nested;
  }
}

function _datex_set_fs(&$element, &$fs, $value, $extra_parent = NULL, $is_input = TRUE) {
  $key = $is_input ? 'input' : 'values';
  if (!isset($fs[$key])) {
    return;
  }

  $parents = $element['#parents'];
  if ($extra_parent) {
    $parents[] = $extra_parent;
  }
  drupal_array_set_nested_value($fs[$key], $parents, $value);
}

// ____________________________________________________________________________

/**
 * To convert the day back to Gregorian.
 */
function _datex_select_date_field_element_validate_callback(&$element, &$fs) {
  if (empty($element['#value']) || empty($element['#value']['year'])) {
    return;
  }

  if (_datex_field_schema($element) === 'disabled') {
    return;
  }

  $tz = isset($element['#date_timezone']) ? $element['#date_timezone'] : NULL;
  $calendar = datex_factory($tz, _datex_language_calendar(_datex_field_schema($element)));
  if (!$calendar) {
    return;
  }

  $is_view = isset($fs['view']);
  $fval = _datex_get_fs($element, $fs);
  if ($fval) {
    $calendar->setDateLocale(
      $fval['year'],
      isset($fval['month']) ? $fval['month'] : 1,
      isset($fval['day']) ? $fval['day'] : 1
    );
    $calendar->setTime(
      isset($fval['hour']) ? $fval['hour'] : 12,
      isset($fval['minute']) ? $fval['minute'] : 0,
      isset($fval['second']) ? $fval['second'] : 0
    );

    if ($is_view &&
      in_array('month', $element['#granularity'], TRUE) &&
      !in_array('day', $element['#granularity'])) {
      $calendar->setTimestamp($calendar->xFormat('U') + 20 * 24 * 3600);
    }

    $f = $calendar->xFormatArray();
    _datex_set_fs($element, $fs, $f, NULL, TRUE);
    _datex_set_fs($element, $fs, $f, NULL, FALSE);
  }
}

/**
 * Implements hook_date_select_process_alter().
 */
function datex_date_select_process_alter(&$element, &$form_state) {
  if (_datex_is_disabled('date')) {
    return;
  }
  if (_datex_field_schema($element) === 'disabled') {
    return;
  }
  if (!_datex_date_granularity_supported($element['#granularity'])) {
    return;
  }

  $tz = isset($element['#date_timezone']) ? $element['#date_timezone'] : NULL;
  $calendar = datex_factory($tz, _datex_language_calendar(_datex_field_schema($element)));
  if (!$calendar) {
    return;
  }

  $year_now = intval($calendar->format('Y'));

  $is_view = isset($form_state['view']);
  $valid = !empty($element['#value']['year']);
  $element_value = $valid ? $element['#value'] : $element['#default_value'];
  $date_array = $valid ? $element_value : [];
  $input = _datex_get_fs($element, $form_state);
  if (!empty($element_value) && empty($input)) {
    $date = is_string($element_value)
      ? new DateObject($element_value)
      : date_select_input_date($element, $element_value);
    if ($date->hasGranularity('month') && !$date->hasGranularity('day')) {
      // If we set day to 25th, as date module has set it to 1st, conversion
      // will be correct. Why? can't explain, really.
      $date->setTimestamp($date->format('U') + 24 * 3600 * 25);
    }
    if ($date) {
      $calendar->setTimestamp($date->format("U"));
      $date_array = $calendar->formatArray();
      $element['#value'] = $date_array;
    }
  }

  if (in_array('year', $element['#granularity'])) {
    list($year_before, $year_after) = explode(':', $element['#date_year_range']);
    $year_before = intval($year_before);
    $year_after = intval($year_after);

    $element['year']['#options'] = $element['#required']
      ? []
      : [array_shift($element['year']['#options'])];

    for ($year = $year_now + $year_before; $year <= $year_now + $year_after; $year++) {
      $element['year']['#options'][$year] = $year;
    }

    if ($valid) {
      $element['year']['#default_value'] = $date_array['year'];
    }
  }
  if (in_array('month', $element['#granularity'])) {
    $element['month']['#options'] = $element['#required']
      ? []
      : [array_shift($element['month']['#options'])];

    $names = $calendar->getMonthNames();
    foreach ($names as $index => $item) {
      $names[$index] = t($item);
    }


    for ($i = 0; $i < 12; $i++) {
      $element['month']['#options'][$i + 1] = $names[$i];
    }

    if ($valid) {
      $element['month']['#default_value'] = $date_array['month'];
    }
  }
  if (in_array('day', $element['#granularity'])) {
    // TODO fix number of days.
    if ($valid) {
      $element['day']['#default_value'] = $date_array['day'];
    }
  }
}

// -------------------

/**
 * Implements hook_date_formatter_dates_alter().
 */
function datex_date_formatter_dates_alter(&$dates, $context) {
  if (_datex_is_disabled('date')) {
    return;
  }

  $calendar = _datex_field_calendar($context);
  if (!$calendar) {
    return;
  }

  foreach ($dates as $name => &$date) {
    $grs = $context['field']['settings']['granularity'];
    // 12 hours
    $offset = !($grs['hour'] || $grs['minute'] || $grs['second']) ? 43200 : 0;
    if (!($grs['month'] || $grs['day'])) {
      // 6 months.
      $offset += 16070400;
    }
    elseif (!$grs['day']) {
      // 31 days.
      $offset += 2678400;
    }

    // To set the year/month/day
    $timestamp = $date['local']['object']->format('U');
    $calendar->setTimestamp($timestamp + $offset);

    // to set the hour/minute/second
    $calendar->setTime(
      (int) $date['local']['object']->format('G'),
      (int) $date['local']['object']->format('i'),
      (int) $date['local']['object']->format('s')
    );

    $dates[$name]['formatted'] = $calendar->format($context['format']);
  }
}

// ____________________________________________________________________________

/**
 * Adds datex as a date form element validator.
 */
function datex_element_info_alter(&$elements) {
  if (_datex_is_disabled('date')) {
    return;
  }

  if (isset($elements['date_select'])) {
    $el = &$elements['date_select'];
    $validator = isset($el['#element_validate']) ? $el['#element_validate'] : [];
    $validator = array_merge(['_datex_select_date_field_element_validate_callback'], $validator);
    $el['#element_validate'] = $validator;
  }

  if (!_datex_is_disabled('popup')) {
    if (isset($elements['date_popup'])) {
      $el = &$elements['date_popup'];
      $validator = isset($el['#element_validate']) ? $el['#element_validate'] : [];
      $validator = array_merge(['_datex_popup_date_field_element_validate_callback'], $validator);
      $el['#element_validate'] = $validator;
    }
  }
}

/**
 * Implements hook_field_info_alter().
 *
 * Add an extra option to control datex for each field instance.
 */
function datex_field_info_alter(&$info) {
  if (_datex_is_disabled('date')) {
    return;
  }

  foreach (['datetime', 'date', 'datestamp'] as $type) {
    if (isset($info[$type]['instance_settings'])) {
      $info[$type]['instance_settings']['datex_schema'] = 'default';
    }
  }
}

/**
 * Implements hook_date_field_instance_settings_form_alter().
 *
 * For per-field control.
 */
function datex_date_field_widget_settings_form_alter(&$form, $context) {
  if (_datex_is_disabled('date')) {
    return;
  }

  $granularity = $context['field']['settings']['granularity'];
  $schema = isset($context['instance']['widget']['settings']['datex_schema'])
    ? $context['instance']['widget']['settings']['datex_schema'] : 'default';

  $form['datex_schema'] = [
    '#title' => t('Datex Schema'),
    '#type' => 'select',
    '#description' => t('Datex will be disabled for fields without month granularity ' .
      'but with smaller granularity.'),
  ];
  if (_datex_date_granularity_supported($granularity)) {
    $form['datex_schema'] += [
      '#options' => _datex_schema_form_options(),
      '#default_value' => $schema,
    ];
  }
  else {
    $form['datex_schema'] += [
      '#options' => ['disabled' => t('Disabled')],
      '#default_value' => 'disabled',
      '#disabled' => TRUE,
    ];
  }
}

/**
 * Add datex configuration to date field.
 *
 * Add an extra option to control datex for each field instance display.
 */
function datex_field_formatter_info_alter(&$info) {
  if (_datex_is_disabled('date')) {
    return;
  }

  if (isset($info['date_default'])) {
    $info['date_default']['settings']['datex_schema'] = 'default';
  }
  if (isset($info['date_plain'])) {
    $info['date_plain']['settings']['datex_schema'] = 'default';
  }
}

/**
 * implements hook_date_field_formatter_settings_form_alter().
 */
function datex_date_field_formatter_settings_form_alter(&$form, &$form_state, $context) {
  if (_datex_is_disabled('date')) {
    return;
  }

  if (!_datex_date_granularity_supported($context['field']['settings']['granularity'])) {
    $options = ['disabled' => t('Disabled (Unsupported)')];
    $schema = 'disabled';
    $desc = t('Datex is disabled for fields without month granularity but with day granularity.');
  }
  else {
    $schema = _datex_field_schema($context);
    $options = _datex_schema_form_options();
    $desc = '';
  }

  $form['datex_schema'] = [
    '#title' => t('Datex Schema'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $schema,
    '#description' => $desc,
  ];
}

/**
 * implements hook_date_field_formatter_settings_summary_alter().
 */
function datex_date_field_formatter_settings_summary_alter(&$summary, $context) {
  if (_datex_is_disabled('date')) {
    return;
  }

  $summary[] = _datex_date_granularity_supported($context['field']['settings']['granularity'])
    ? t('Datex: @calendar', ['@calendar' => _datex_field_calendar($context)])
    : t('Datex: Disabled (Unsupported)');
}
