<?php

// Nice comment from some time ago. I want to keep it :)
//
//function _datex_orig_from_date(array $datetime, $tz) {
// For anyone reading this comment: Passing a DateTimeZone to datetime
// constructor has no effect on it! You MUST use setTimezone to set a
// tz on the stupid object.
// Tested on PHP 5.4.15 (built: May 12 2013 13:11:23) Archlinux.
//}

interface DatexInterface {

  /**
   * Format the date always in Gregorian.
   *
   * @param $format string date format
   *
   * @return string formatted date.
   */
  public function xFormat($format);

  /**
   * Format the date according to locale previously set.
   *
   * @param $format string date format
   *
   * @return string formatted date.
   */
  public function format($format);

  public function formatArray();

  public function xFormatArray();

  public function xSetDate($y, $m, $d);

  public function setDateLocale($y, $m, $d);

  public function setTime($hour, $minute, $second);


  public function setTimestamp($timestamp);

  public function getTimestamp();


  public function getLocaleName();

  public function getMonthNames();

}

abstract class DatexPartialImplementation implements DatexInterface {

  private $origin;

  private $timezone;

  private $localeName;

  protected final function timezone() {
    return $this->timezone;
  }

  public function __construct($tz, $localeName) {
    $this->timezone = self::_tz($tz ? $tz : drupal_get_user_timezone());;
    $this->origin = new DateTime('now', $this->timezone);
    $this->localeName = $localeName;
  }

  public final function getLocaleName() {
    return $this->localeName;
  }

  // ------------------------------------ FORMAT

  /**
   * Format date time, in gregorian.
   *
   * @param $format
   *
   * @return string
   */
  public final function xFormat($format) {
    return $this->origin->format($format);
  }

  /**
   * Put all day and time parts in an array, in gregorian.
   *
   * @return array
   */
  public final function xFormatArray() {
    return [
      'year' => intval($this->origin->format('Y')),
      'month' => intval($this->origin->format('n')),
      'day' => intval($this->origin->format('j')),
      'hour' => intval($this->origin->format('G')),
      'minute' => intval($this->origin->format('i')),
      'second' => intval($this->origin->format('s')),
    ];
  }

  public final function xSetDate($y, $m, $d) {
    $this->origin->setDate($y, $m, $d);
    return $this;
  }

  public final function setTimestamp($timestamp) {
    $this->origin->setTimestamp($timestamp);
    return $this;
  }

  public final function getTimestamp() {
    return $this->origin->getTimestamp();
  }

  // ------------------------------- OVERRIDABLE

  public final function setTime($hour, $minute, $second) {
    $this->origin->setTime($hour, $minute, $second);
    return $this;
  }

  /**
   * ATTENTION!!! -> calls subclass.
   *
   * Put all day and time parts in an array.
   *
   * @return array
   */
  public final function formatArray() {
    return [
      'year' => intval($this->format('Y')),
      'month' => intval($this->format('n')),
      'day' => intval($this->format('j')),
      'hour' => intval($this->format('G')),
      'minute' => intval($this->format('i')),
      'second' => intval($this->format('s')),
    ];
  }

  /**
   * ATTENTION!!! -> calls subclass.
   *
   * @return array
   */
  public final function getMonthNames() {
    // Haha. Fix this?
    $m = [];
    $this->setDateLocale(0, 1, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 2, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 3, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 4, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 5, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 6, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 7, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 8, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 9, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 10, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 11, 1);
    $m[] = $this->format('F');
    $this->setDateLocale(0, 12, 1);
    $m[] = $this->format('F');
    return $m;
  }

  // -------------------------------------------

  /**
   * Timezone factory.
   *
   * Looks into core's cached timezones.
   *
   * @param $tz
   *
   * @return \DateTimeZone
   */
  private static function _tz($tz) {
    // Being over-smart. works for now.
    if (!is_string($tz)) {
      return $tz;
    }

    // steal from core
    static $drupal_static_fast;
    if (!isset($drupal_static_fast)) {
      $drupal_static_fast['timezones'] = &drupal_static('format_date');
    }
    $timezones = &$drupal_static_fast['timezones'];
    if (!isset($timezones[$tz])) {
      $timezones[$tz] = timezone_open($tz);
    }
    return $timezones[$tz];
  }

}

final class IntlDatexCalendar extends DatexPartialImplementation implements DatexInterface {

  private $intlFormatter;

  private $locale;

  public function __construct($tz, $localeName) {
    parent::__construct($tz, $localeName);
    $this->locale = 'en@calendar=' . $localeName;
    $this->intlFormatter = self::intl($this->timezone(), $this->locale);
  }

  public function format($format) {
    $rep = preg_replace(self::$remove_pattern, '', $format);
    $pat = strtr($rep, self::$php2intl_format_map);
    $this->intlFormatter->setPattern($pat);
    return $this->intlFormatter->format($this->getTimestamp());
  }

  public function setDateLocale($y, $m, $d) {
    list($gy, $gm, $gd) = $this->toGregorian($this->intlFormatter, $this->timezone(), $y, $m, $d);
    parent::xSetDate($gy, $gm, $gd);
    return $this;
  }

  // -------------------------------------------

  /**
   * php's date format modifiers differ from Intl's. This is a mapping of the
   * two.
   *
   * @var array
   */
  private static $php2intl_format_map = [
    'd' => 'dd',
    'D' => 'EEE',
    'j' => 'd',
    'l' => 'EEEE',
    'N' => 'e',
    'S' => 'LLLL',
    'w' => '',
    'z' => 'D',
    'W' => 'w',
    'm' => 'MM',
    'M' => 'MMM',
    'F' => 'MMMM',
    'n' => 'M',
    't' => '',
    'L' => '',
    'o' => 'yyyy',
    'y' => 'yy',
    'Y' => 'YYYY',
    'a' => 'a',
    'A' => 'a',
    'B' => '',
    'g' => 'h',
    'G' => 'H',
    'h' => 'hh',
    'H' => 'HH',
    'i' => 'mm',
    's' => 'ss',
    'u' => 'SSSSSS',
    'e' => 'z',
    'I' => '',
    'O' => 'Z',
    'P' => 'ZZZZ',
    'T' => 'v',
    'Z' => '',
    'c' => '',
    'r' => '',
    'U' => '',
    ' ' => ' ',
    '-' => '-',
    '.' => '.',
    ':' => ':',
  ];

  /**
   * Some format modifiers are not supported in intl. They are simply removed.
   *
   * @var array
   */
  private static $remove_pattern = '/[^ \:\-\/\.\\\\dDjlNSwzWmMFntLoyYaABgGhHisueIOPTZcrU]/';

  private static function toGregorian(IntlDateFormatter $fmt, DateTimeZone $tz, $y, $m, $d) {
    $fmt->setPattern('MM/dd/y HH:mm:ss');
    // TODO needed?
    $fmt->setLenient(TRUE);
    // TODO time?
    $ts = $fmt->parse($m . '/' . $d . '/' . $y . ' 10:10:10');
    $d = new DateTime('@' . $ts, $tz);
    return [$d->format('Y'), $d->format('n'), $d->format('j')];
  }

  /**
   * factory method.
   *
   * @param \DateTimeZone $tz
   * @param $locale
   *
   * @return \IntlDateFormatter
   */
  private static function intl(DateTimeZone $tz, $locale) {
    return new IntlDateFormatter(
      $locale,
      IntlDateFormatter::NONE,
      IntlDateFormatter::NONE,
      // TODO Why new DateTimeZone? use tz
      IntlTimeZone::fromDateTimeZone($tz),
      // Why always traditional?
      IntlDateFormatter::TRADITIONAL
    );
  }

}

/**
 * @param null $tz
 * @param null $locale
 *
 * @return \DatexInterface
 */
function datex_factory($tz = NULL, $locale = NULL) {
  if (!$locale) {
    $locale = _datex_language_calendar();
  }
  if (!$locale) {
    return NULL;
  }

  if (DATEX_USE_INTL) {
    return new IntlDatexCalendar($tz, $locale);
  }
  else {
    if ($locale === 'persian') {
      require_once 'calendar.poor_mans_intl.inc';
      return new DatexPoorMansJaliliCalendar(is_string($tz) ? timezone_open($tz) : $tz);
    }
    else {
      return NULL;
    }
  }
}

