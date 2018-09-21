<?php

abstract class DatexPartialImplementation implements DatexInterface {

  protected $origin;

  protected $timezone;

  protected $calendar;

  protected $langCode;

  public function __construct($tz, $calendar, $langCode) {
    $this->timezone = self::_tz($tz ? $tz : drupal_get_user_timezone());;
    $this->origin = new DateTime('now', $this->timezone);
    $this->calendar = $calendar;
    $this->langCode = $langCode;
  }

  public final function getCalendarName() {
    return $this->calendar;
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

  public function validate(array $arr) {
    return NULL;
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
