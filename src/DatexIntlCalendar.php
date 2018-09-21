<?php

class DatexIntlCalendar extends DatexPartialImplementation implements DatexInterface {

  protected $intlFormatter;

  protected $locale;

  public function __construct($tz, $calendar, $lang_code) {
    parent::__construct($tz, $calendar, $lang_code);
    $this->locale = $lang_code . '@calendar=' . $calendar;
    $this->intlFormatter = self::intl($this->timezone, $this->locale);
  }

  public function format($format) {
    $rep = preg_replace(self::$remove_pattern, '', $format);
    $pat = strtr($rep, self::$php2intl_format_map);
    $this->intlFormatter->setPattern($pat);
    return $this->intlFormatter->format($this->getTimestamp());
  }

  public function setDateLocale($y, $m, $d) {
    $y = intval($y);
    $m = intval($m);
    $d = intval($d);
    list($gy, $gm, $gd) = $this->toGregorian($this->intlFormatter, $this->timezone, $y, $m, $d);
    parent::xSetDate($gy, $gm, $gd);
    return $this;
  }

  public function copy() {
    return new DatexIntlCalendar($this->timezone, $this->calendar, $this->langCode);
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
