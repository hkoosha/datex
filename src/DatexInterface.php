<?php

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


  public function getCalendarName();

  public function getMonthNames();


  public function copy();

  public function validate(array $arr);

}
