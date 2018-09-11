<?php

/**
 * @file
 * Provides support for date module.
 */

define('DATEX_HIDDEN_VALUE_FMT', 'Y-n-j');

// _____________________________________________________________________ HELPER

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
function _datex_granularity_supported($g) {
  if (is_string($g)) {
    return in_array($g, ['year', 'month']);
  }
  // Year is always there.
  return count($g) === 1 || in_array('month', $g);
}

/**
 * Extract granularity from element (probably a form element).
 */
function _datex_date_field_granularity($element) {
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
  else {
    return [[], []];
  }
}

function _datex_date_field_has_granularity(array $element, $granul) {
  $gr = _datex_date_field_granularity($element);
  switch ($granul) {
    case 'time':
      return
        in_array('hour', $gr, TRUE) ||
        in_array('minute', $gr, TRUE) ||
        in_array('second', $gr, TRUE);
    case 'mad':
      // Month and day
      return
        in_array('month', $gr, TRUE) &&
        in_array('day', $gr, TRUE);
    case 'mod':
      // Month or day
      return
        in_array('month', $gr, TRUE) ||
        in_array('day', $gr, TRUE);
    default:
      return in_array($granul, $gr, TRUE);
  }
}

/**
 * Find the datex schema set for a field, or the global schema of none is set.
 */
function _datex_date_field_schema($e) {
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
function _datex_date_field_calendar($element) {
  if (_datex_is_disabled('date')) {
    return NULL;
  }
  if (_datex_date_field_schema($element) === 'disabled') {
    return NULL;
  }
  if (!_datex_granularity_supported(_datex_date_field_granularity($element))) {
    return NULL;
  }

  $tz = isset($element['#date_timezone']) ? $element['#date_timezone'] : NULL;
  return datex_factory($tz, _datex_language_calendar(_datex_date_field_schema($element)));
}

// ------------------------ DON'T ASK

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

function _datex_fix_missing_granularity(DateObject $date, array $element, $calendar) {
  if ($calendar !== 'persian') {
    return;
  }
  if (_datex_date_field_has_granularity($element, 'month') &&
    !_datex_date_field_has_granularity($element, 'day')) {
    // There are maximum of 23 days difference in Persian calendar and
    // Gregorian, that is, April-1st is 22 days behind Farvardin-1st.
    // We take 24 days to be sure. For leap years included.
    $offset = 24 * (24 * 3600);
    $date->setTimestamp($date->getTimestamp() + $offset);
  }
  if (!_datex_date_field_has_granularity($element, 'month')) {
    // There are 3 months difference in Persian calendar and
    // Gregorian, that is, Jan-1st is 3 months behind Farvardin-1st.
    // We take 4 days to be sure. For leap years included.
    $offset = 4 * 30 * (24 * 3600);
    $date->setTimestamp($date->getTimestamp() + $offset);
  }
}

function _datex_unfix_missing_granularity(DateObject $date, $calendar) {
  if ($calendar !== 'persian') {
    return;
  }
  if ($date->hasGranularity('month') && !$date->hasGranularity('day')) {
    $offset = (24 * 3600) * 24;
    $date->setTimestamp($date->getTimestamp() - $offset);
  }
}

// _______________________________________________________________________ MAIN

/**
 * To convert the day back to Gregorian.
 */
function _datex_select_date_field_element_validate_callback(&$element, &$fs) {
  if (empty($element['#value']) || empty($element['#value']['year'])) {
    return;
  }

  $calendar = _datex_date_field_calendar($element);
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

    //    if ($is_view &&
    //      in_array('month', $element['#granularity'], TRUE) &&
    //      !in_array('day', $element['#granularity'])) {
    //      $calendar->setTimestamp(((int) $calendar->xFormat('U')) + 20 * 24 * 3600);
    //    }

    $f = $calendar->xFormatArray();
    _datex_set_fs($element, $fs, $f, NULL, TRUE);
    _datex_set_fs($element, $fs, $f, NULL, FALSE);
  }
}

/**
 * Implements hook_date_select_process_alter().
 */
function datex_date_select_process_alter(&$element, &$form_state) {
  if (!_datex_granularity_supported($element['#granularity'])) {
    return;
  }

  $calendar = _datex_date_field_calendar($element);
  if (!$calendar) {
    return;
  }

  $year_now = intval($calendar->format('Y'));
  $valid = !empty($element['#value']['year']);
  $element_value = $valid ? $element['#value'] : $element['#default_value'];
  $date_array = $valid ? $element_value : [];
  $input = _datex_get_fs($element, $form_state);

  // Non-empty input means we have processed the element already. But there
  // has been an error on the form (form_set_error probably), and user has
  // re-submitted their form and it's values.
  // At this point we do not process (convert dates) again. It just blows up
  // everything.
  // But if the input is empty, it means we should process and convert dates (
  // this is the first time we are visiting this element).
  if (!empty($element_value) && empty($input)) {
    $date = is_string($element_value)
      ? new DateObject($element_value)
      : date_select_input_date($element, $element_value);
    if ($date) {
      _datex_fix_missing_granularity($date, $element, $calendar->getLocaleName());
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
    // TODO fix number of days, for leap years and all the fissle and hussle.
    if ($valid) {
      $element['day']['#default_value'] = $date_array['day'];
    }
  }
}

/**
 * Implements hook_date_formatter_dates_alter().
 */
function datex_date_formatter_dates_alter(&$dates, $context) {
  $calendar = _datex_date_field_calendar($context);
  if (!$calendar) {
    return;
  }

  foreach ($dates as $index => &$date) {
    $timestamp = $date['local']['object']->format('U');
    $tz = $date['local']['object']->getTimezone();
    $copy = new DateObject('@' . $timestamp, $tz);
    _datex_fix_missing_granularity($copy, $context, $calendar->getLocaleName());
    $calendar->setTimestamp($copy->getTimestamp());
    $calendar->setTime(
      (int) $date['local']['object']->format('G'),
      (int) $date['local']['object']->format('i'),
      (int) $date['local']['object']->format('s')
    );
    $dates[$index]['formatted'] = $calendar->format($context['format']);
  }
}

// ______________________________________________________________________ HELLO

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
  $schema = _datex_date_field_schema($context);
  if (_datex_granularity_supported($granularity)) {
    $form['datex_schema'] = [
      '#title' => t('Datex Schema'),
      '#type' => 'select',
      '#options' => _datex_schema_form_options(),
      '#default_value' => $schema,
    ];
  }
  else {
    $form['datex_schema'] = [
      '#title' => t('Datex Schema'),
      '#type' => 'select',
      '#options' => ['disabled' => t('Disabled')],
      '#default_value' => 'disabled',
      '#disabled' => TRUE,
      '#description' => t('Datex will be disabled for fields without ' .
        'month granularity but with smaller granularity.'),
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

  if (!_datex_granularity_supported($context['field']['settings']['granularity'])) {
    $form['datex_schema'] = [
      '#title' => t('Datex Schema'),
      '#type' => 'select',
      '#options' => ['disabled' => t('Disabled (Unsupported)')],
      '#default_value' => 'disabled',
      '#description' => t('Datex is disabled for fields without month ' .
        'granularity but with day granularity.'),
    ];
  }
  else {
    $form['datex_schema'] = [
      '#title' => t('Datex Schema'),
      '#type' => 'select',
      '#options' => _datex_schema_form_options(),
      '#default_value' => _datex_date_field_schema($context),
      '#description' => '',
    ];
  }
}

/**
 * implements hook_date_field_formatter_settings_summary_alter().
 */
function datex_date_field_formatter_settings_summary_alter(&$summary, $context) {
  if (_datex_is_disabled('date')) {
    return;
  }

  $summary[] = _datex_granularity_supported($context['field']['settings']['granularity'])
    ? t('Datex: @calendar', ['@calendar' => _datex_date_field_calendar($context)])
    : t('Datex: Disabled (Unsupported)');
}
