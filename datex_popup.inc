<?php

define('DATEX_POPUP_VERSION', '1.2.1');
define('DATEX_POPUP_RELATIVE_PATH', '/'); // /dist/js/ for 2.0.1
define('DATEX_POPUP_RELATIVE_CSS_PATH', '/'); // /dist/css/ for 2.0.1

function _datex_add_lib_cal($name) {
  $name = _datex_language_calendar($name);
  $lib_path = libraries_get_path('jquery_calendars');
  $js_path = $lib_path . DATEX_POPUP_RELATIVE_PATH;

  static $self_loaded = FALSE;
  if (!$self_loaded) {
    $l_load = libraries_load('jquery_calendars');
    if (!$l_load['loaded']) {
      return FALSE;
    }
    $datex_module_path = drupal_get_path('module', 'datex');
    drupal_add_js($datex_module_path . '/files/datex_popup.js');
    drupal_add_css($datex_module_path . '/files/datex_popup.css');
    $theme = variable_get('datex_popup_theme', '');
    if (!empty($theme)) {
      $css_path = $lib_path . DATEX_POPUP_RELATIVE_CSS_PATH;
      drupal_add_css($css_path . $theme);
    }
  }

  if (isset($GLOBALS['datex_calendars'][$name])) {
    return TRUE;
  }

  switch ($name) {
    case 'ethiopic':
      $name = 'ethiopian';
      break;

    case 'persian':
    case 'islamic':
    case 'hebrew':
    case 'coptic':
      break;
  }
  drupal_add_js($js_path . 'jquery.calendars.' . $name . '.js');

  $lang = $GLOBALS['language']->language;
  if ($lang !== 'en') {
    $add = [
      $js_path . 'jquery.calendars.picker-' . $lang . '.js',
      $js_path . 'jquery.calendars-' . $lang . '.js',
      $js_path . 'jquery.calendars.' . $name . '-' . $lang . '.js',
    ];
    foreach ($add as $item) {
      if (file_exists($item)) {
        drupal_add_js($item);
      }
    }
  }

  $GLOBALS['datex_calendars'][$name] = TRUE;
  return TRUE;
}

function _datex_popupify(&$element, DatexInterface $calendar, $date_format, $hidden_value) {
  if (_datex_is_disabled('popup')) {
    return NULL;
  }

  $schema = _datex_field_schema($element);
  _datex_add_lib_cal($schema);

  $name = _datex_language_calendar($schema);
  switch ($name) {
    case 'ethiopic':
      $name = 'ethiopian';
      break;

    case 'persian':
    case 'islamic':
    case 'hebrew':
    case 'coptic':
      break;
  };

  _datex_set_calendar_time_from_field_default_value($element, $calendar);
  $def = $calendar->format('Y-n-j');

  list($from, $to) = date_range_years($element['#date_year_range']);
  $calendar->xSetDate($from, 1, 1);
  $from = $calendar->format('Y-n-j');
  $calendar->xSetDate($to, 1, 1);
  $to = $calendar->format('Y-n-j');

  $replace = [
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
  ];
  $date_format = strtr($date_format, $replace);

  $s['minDate'] = $from;
  $s['maxDate'] = $to;
  $s['calendar'] = _datex_language_calendar(_datex_field_schema($element));
  $s['dir'] = $GLOBALS['language']->direction ? 'topRight' : 'topLeft';
  $s['langcode'] = $GLOBALS['language']->language;
  $s['langcode'] = 'fa';
  $s['calendar'] = $name;
  $s['fmt'] = $date_format;
  $s['def'] = $def;

  $p = array_values($element['#parents']);
  $p[] = 'datex_hidden';
  $element['datex_hidden'] = [
    '#type' => 'hidden',
    '#value' => $hidden_value,
    '#parents' => $p,
    '#attributes' => ['data-datex-findme' => $element['date']['#id']],
  ];
  foreach ($s as $name => $item) {
    $element['datex_hidden']['#attributes']['data-datex-' . $name][] = $item;
  }
}

/**
 * Imlements hook_js_alter().
 *
 * Whenever date_popup.js is loaded, Remove it and inject datex library.
 */
function datex_js_alter(&$js) {
  if (_datex_is_disabled('js')) {
    return;
  }
  foreach (array_keys($js) as $path) {
    foreach ([
               'locale.datepicker.js',
               'date_popup.js',
               'jquery.ui.datepicker.min.js',
             ] as $unset) {
      // TODO minus?
      // TODO non-min?
      if (substr($path, -strlen($unset)) === $unset) {
        unset($js[$path]);
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
             ] as $unset) {
      if (strpos($each, $unset) !== FALSE) {
        unset($css[$each]);
      }
    }
  }
}

/**
 * Implements hook_libraries_info().
 *
 * jQuery World Calendar, by keith wood. http://keith-wood.name
 */
function datex_libraries_info() {
  if (_datex_is_disabled('libraries')) {
    return [];
  }
  $version = DATEX_POPUP_VERSION;
  $libraries['jquery_calendars'] = [
    'name' => t('jQuery Calendars'),
    'vendor url' => 'http://keith-wood.name',
    'download url' => 'http://keith-wood.name/calendars.html',
    'download file url' => 'https://github.com/kbwood/calendars/archive/' . $version . '.tar.gz',
    'version' => $version,
    'version argumentsx' => [
      'file' => 'jquery.calendars.all.js',
      'pattern' => '#v(\\d\\.\\d\\.\\d)\\.#',
      'lines' => 2,
    ],
    'path' => DATEX_POPUP_RELATIVE_PATH,
    'files' => [
      'js' => [
        'jquery.calendars.all.min.js',
      ],
    ],
    'variants' => [
      'normal' => [
        'files' => [
          'js' => [
            'jquery.calendars.all.js',
          ],
        ],
      ],
    ],
  ];
  return $libraries;
}

function _datex_popup_set_hidden_date_on_calendar(DatexInterface $calendar, $hval, $g) {
  // Year-Month-Day
  $parts = explode('-', $hval);
  if (!in_array('month', $g)) {
    // Month 2 as placeholder (for dates without month granularity) is safer
    // than 1, when converting back and forth to Gregorian rounding errors
    // wont cause the year to go back one year accidentally.
    $parts[1] = 2;
  }
  if (!in_array('day', $g)) {
    // Month 2 as placeholder (for dates without month granularity) is safer
    // than 1, when converting back and forth to Gregorian rounding errors
    // wont cause the year to go back one year accidentally.
    $parts[2] = 2;
  }

  try {
    $calendar->setDateLocale($parts[0], $parts[1], $parts[2]);
  } catch (Exception $e) {
    return FALSE;
  }
  _datex_fix_missing_granularity_locale($calendar, $g);
  return TRUE;
}

function _datex_popup_date_field_element_validate_callback(&$element, &$fs) {
  if (date_hidden_element($element) || is_string($element['#value'])) {
    return;
  }

  $calendar = _datex_field_calendar($element);
  if (!$calendar) {
    return;
  }

  $dval = _datex_get_fs($element, $fs, 'date');
  $hval = _datex_get_fs($element, $fs, 'datex_hidden');

  if (empty($dval)) {
    // User has cleared date fields value, so clear datex_hidden too.
    _datex_set_fs($element, $fs, '', 'datex_hidden', TRUE);
    _datex_set_fs($element, $fs, '', 'datex_hidden', FALSE);
    return;
  }
  elseif (empty($hval)) {
    // When we have date value, we MUST have datex_hidden value too.
    form_set_error($element['#name'], t('You have to specify a valid date.'));
    return;
  }

  $def = date_default_date($element);
  $date_fmt = date_popup_date_format($element);
  $g = date_popup_date_granularity($element);
  _datex_fix_missing_granularity($calendar->getLocaleName(), $def, $g);

  if (!_datex_popup_set_hidden_date_on_calendar($calendar, $hval, $g)) {
    form_set_error($element['#name'], t('Invalid date'));
    return;
  }

  $v = $calendar->xFormat($date_fmt);
  _datex_set_fs($element, $fs, $v, 'date', TRUE);
  _datex_set_fs($element, $fs, $v, 'date', FALSE);
}

/**
 * Implements hook_date_popup_process_alter().
 */
function datex_date_popup_process_alter(&$element, &$form_state, $context) {
  if (_datex_is_disabled('date') || _datex_is_disabled('popup')) {
    return;
  }

  $calendar = _datex_field_calendar($element, TRUE);
  if (!$calendar) {
    return NULL;
  }

  $date_fmt = date_popup_date_format($element);
  $def = date_default_date($element);
  $input = _datex_get_fs($element, $form_state);
  $g = date_popup_date_granularity($element);
  $hidden_value = '';
  _datex_fix_missing_granularity($calendar->getLocaleName(), $def, $g);

  if (!empty($element['date']['#default_value'])) {
    _datex_dpm($element, $form_state, 'has def');
    $calendar->setTimestamp($def->getTimestamp());
    $element['date']['#default_value'] = $calendar->format($date_fmt);
    $hidden_value = $calendar->format(DATEX_HIDDEN_VALUE_FMT);
  }

  if ($input && !empty($input['datex_hidden']) && !empty($element['date']['#value'])) {
    if (!_datex_popup_set_hidden_date_on_calendar($calendar, $input['datex_hidden'], $g)) {
      form_set_error($element['#name'], t('Invalid date'));
      return;
    }
    $element['date']['#value'] = $calendar->format($date_fmt);
    $hidden_value = $input['datex_hidden'];
  }
  elseif (!empty($element['date']['#value'])) {
    if ($def) {
      _datex_dpm($element, $form_state, 'has def val');
      $calendar->setTimestamp($def->getTimestamp());
      $element['date']['#value'] = $calendar->format($date_fmt);
      $hidden_value = $calendar->format(DATEX_HIDDEN_VALUE_FMT);
    }
    else {
      $element['date']['#value'] = '';
    }
  }

  _datex_popupify(
    $element,
    $calendar,
    date_part_format('date', $element['#date_format']),
    $hidden_value);
}

