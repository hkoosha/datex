<?php


function _datex_add_lib() {
  static $self_loaded = FALSE;
  if ($self_loaded) {
    return;
  }
  $self_loaded = TRUE;

  if (_datex_is_disabled('js')) {
    return;
  }

  $datex_module_path = drupal_get_path('module', 'datex');
  $min = variable_get('datex_popup_js_min', TRUE) ? '.min' : '';

  drupal_add_js($datex_module_path . '/asset/datex.js');
  drupal_add_js($datex_module_path . "/asset/PersianDate/dist/persian-date$min.js");
  drupal_add_js($datex_module_path . "/asset/pwt.datepicker/dist/js/persian-datepicker$min.js");

  drupal_add_css($datex_module_path . "/asset/pwt.datepicker/dist/css/persian-datepicker$min.css");
  $theme = variable_get('datex_pop_js_theme', 'blue');
  if ($theme) {
    drupal_add_css($datex_module_path . "/asset/pwt.datepicker/dist/css/theme/persian-datepicker-$theme$min.css");
  }
}

/**
 * Implements hook_js_alter().
 *
 * Whenever date_popup.js is loaded, Remove it and inject datex library.
 */
function datex_js_alter(&$js) {
  if (_datex_is_disabled('js')) {
    return;
  }

  foreach (array_keys($js) as $each) {
    foreach ([
               'locale.datepicker.js',
               'date_popup.js',
               'jquery.ui.datepicker.min.js',
               'timeentry',
               'timepicker',
             ] as $unset) {
      if (strpos($each, $unset) !== FALSE) {
        unset($js[$each]);
        // instead use ours
        _datex_add_lib();
      }
    }
  }
}

/**
 * Implements hook_css_alter().
 */
function datex_css_alter(&$css) {
  if (_datex_is_disabled('js')) {
    return;
  }

  foreach (array_keys($css) as $each) {
    foreach ([
               'jquery.ui.datepicker.css',
               'date_popup',
               'timeentry',
               'timepicker',
             ] as $unset) {
      if (strpos($each, $unset) !== FALSE) {
        unset($css[$each]);
        // instead use ours
        _datex_add_lib();
      }
    }
  }
}

// ____________________________________________________________________________

function _datex_popup_node_edit_form_alter() {
  if (_datex_is_disabled('popup') || !module_exists('date_popup')) {
    return FALSE;
  }
  return TRUE;
}

function _datex_popup_extract_hidden_element(DatexInterface $calendar, $hidden_value, $g) {
  try {
    $hidden_value = substr($hidden_value, 0, 10);
    $calendar->setTimestamp($hidden_value);
    $m = in_array('month', $g) ? $calendar->xFormat('n') : 1;
    $d = in_array('day', $g) ? $calendar->xFormat('j') : 1;
    $calendar->xSetDate($calendar->xFormat('Y'), $m, $d);
    return TRUE;
  } catch (Exception $e) {
    return FALSE;
  }
}

function _datex_date_popup_field_element_validate_callback(&$element, &$fs) {
  if (date_hidden_element($element) || is_string($element['#value'])) {
    return;
  }

  $calendar = _datex_date_field_calendar($element);
  if (!$calendar) {
    return;
  }

  $date_value = _datex_get_fs($element, $fs, 'date');
  $hidden_value = _datex_get_fs($element, $fs, 'datex_hidden');

  if (empty($date_value)) {
    // User has cleared date fields value, so clear datex_hidden too.
    _datex_set_fs($element, $fs, '', 'datex_hidden', TRUE);
    _datex_set_fs($element, $fs, '', 'datex_hidden', FALSE);
    return;
  }
  elseif (empty($hidden_value)) {
    // When we have date value, we MUST have datex_hidden value too. If not,
    // probably the js has failed to work on client side.
    form_set_error($element['#name'], t('You have to specify a valid date.'));
    return;
  }

  $def = date_default_date($element);
  $date_fmt = date_popup_date_format($element);
  $g = date_popup_date_granularity($element);

  if (!_datex_popup_extract_hidden_element($calendar, $hidden_value, $g)) {
    form_set_error($element['#name'], t('Invalid date 0'));
    return;
  }


  $v = $calendar->xFormat($date_fmt);
  dpm($v);
  _datex_set_fs($element, $fs, $v, 'date', TRUE);
  _datex_set_fs($element, $fs, $v, 'date', FALSE);
}

/**
 * Implements hook_date_popup_process_alter().
 */
function datex_date_popup_process_alter(&$element, &$form_state, $context) {
  if (_datex_is_disabled('popup')) {
    return;
  }

  $calendar = _datex_date_field_calendar($element, TRUE);
  if (!$calendar) {
    return NULL;
  }

  $date_fmt = date_popup_date_format($element);
  $time_fmt = date_popup_time_format($element);
  $dg = date_popup_date_granularity($element);
  $tg = date_popup_time_granularity($element);
  $input = _datex_get_fs($element, $form_state);

  $def = date_default_date($element);
  if (!in_array('month', $dg)) {
    $def->setDate($def->format('Y'), 6, $def->format('j'));
  }
  if (!in_array('day', $dg)) {
    $def->setDate($def->format('Y'), $def->format('n'), 25);
  }

  $has_date_input = $input && isset($input['date']) && !empty($input['date']);
  $has_time_input = $input && isset($input['time']) && !empty($input['time']);

  // If there is a hidden value, find it
  $hidden_value = $input && isset($input['datex_hidden']) ? $input['datex_hidden'] : '';

  // Else, set hidden to default value, if there is no input value.
  if (empty($hidden_value) &&
    empty($input['date']) &&
    empty($input['time']) &&
    !empty($element['date']['#default_value'])) {
    $calendar->setTimestamp($def->getTimestamp());
    $element['date']['#default_value'] = $calendar->format($date_fmt);
    $hidden_value = $calendar->xFormat(DATEX_HIDDEN_VALUE_FMT);
  }

  // If there was no hidden value but there is date/time value, this is an
  // error. They must be already synced, and they are not. probably js on client
  // side has failed to work.
  if (!$hidden_value && (!empty($input['date']) || !empty($input['time']))) {
    form_set_error($element['#name'], t('Invalid date 1'));
    return;
  }

  // If user has set any value before, set it again according to hidden_value
  // the only source of truth.
  if ($hidden_value && !empty($element['date']['#value'])) {
    if (!_datex_popup_extract_hidden_element($calendar, $hidden_value, $dg)) {
      form_set_error($element['#name'], t('Invalid date 2'));
      return;
    }
    $element['date']['#value'] = $calendar->format($date_fmt);
  }

  list($from, $to) = date_range_years($element['#date_year_range']);

  $calendar->xSetDate($from, 1, 1);
  $from = $calendar->xFormat('U') . '000';

  $calendar->setDateLocale($to, 1, 1);
  $to = $calendar->xFormat('U') . '000';

  dpm($from, $to);

  $date_fmt = strtr($date_fmt, [
    'd' => 'dd',
    'j' => 'd',
    'l' => 'DD',
    'D' => 'D',
    'm' => 'mm',
    'n' => 'm',
    'F' => 'MM',
    'M' => 'M',
    'Y' => 'yyyy',
    'y' => 'yy',
  ]);

  $parents = array_values($element['#parents']);
  $parents[] = 'datex_hidden';
  $id = 'datex-' . $element['date']['#id'];
  $init = strlen($hidden_value) === 10 ? $hidden_value . '000' : (time() - 100000000) . '000';
  $config = "$from|$to|0|0|0|0|0|0|0|0|1|1|persian|day|L|$init";
  $element['datex_hidden'] = [
    '#type' => 'hidden',
    '#title' => 'datex',
    '#value' => $hidden_value,
    '#parents' => $parents,
    '#id' => $id,
    '#attributes' => [
      'data-datex-find-me' => $element['date']['#id'],
      'data-datex-config' => $config,
      'id' => $id,
      'class' => $id,
    ],
  ];

  $element['date']['#description'] = '';

  $settings['datex-' . $id] = [

  ];
}
