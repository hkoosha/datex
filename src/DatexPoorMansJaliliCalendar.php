<?php

/**
 * Jalali calendar for datex.
 *
 * 4 Years later: apparently Amin has deprecated it. It's not on his github
 * page. Don't go bother him with issues about it.
 *
 * Original conversion algorithm by: Amin Saeedi.
 * Forked from project: Shamsi-Date v4.0 (GPL).
 * On github: http://github.com/amsa
 * mail: amin.w3dev@gmail.com
 */
final class DatexPoorMansJaliliCalendar extends DatexPartialImplementation implements DatexInterface {

  public function __construct($tz, $lang_code) {
    parent::__construct($tz, 'persian',
      $lang_code !== 'fa' && $lang_code !== 'en' ? 'fa' : $lang_code);
  }

  public function format($format) {
    $tc = ['context' => 'datex'];
    $names = self::$names[$this->langCode];
    $ampm = parent::xFormat('a');

    list(
      $year,
      $month,
      $day,
      $dayOfYear,
      $dayOfWeek
      ) = self::toJalali(parent::xFormat('U'), parent::xFormat('Z'));

    $isLeap = self::isLeap($year);

    // A series of calls to str replace can not be used since format may
    // contain \ character which should not be replaced.
    $formatted = '';
    for ($i = 0; $i < strlen($format); $i++) {
      $f = $format[$i];
      switch ($f) {
        case '\\':
          $formatted .= $format[$i + 1];
          $i++;
          break;

        case 'w':
          $formatted .= self::dayOfWeek($year, $dayOfYear);
          break;

        case 'N':
          $formatted .= self::dayOfWeek($year, $dayOfYear) + 1;
          break;

        case 'd':
          $formatted .= sprintf('%02d', $day);
          break;

        case 'q':
          $formatted .= t($names['day_abbr'][$dayOfWeek], [], $tc);
          break;

        case 'D':
          $formatted .= t($names['day_abbr_short'][$dayOfWeek], [], $tc);
          break;

        case 'j':
          $formatted .= intval($day);
          break;

        case 'l':
          $formatted .= t($names['day'][$dayOfWeek], [], $tc);
          break;

        case 'S':
          $formatted .= t($names['order'][$day - 1], [], $tc);
          break;

        case 'W':
          $value_w = strval(ceil($dayOfYear / 7));
          $formatted = $formatted . $value_w;
          break;

        case 'z':
          $formatted = $dayOfYear;
          break;

        case 'M':
        case 'F':
          $formatted .= t($names['months'][$month - 1], [], $tc);
          break;

        case 'm':
          $formatted .= sprintf('%02d', $month);
          break;

        case 'n':
          $formatted .= intval($month);
          break;

        case 't':
          $formatted .= ($isLeap && $month == 12) ? 30 : self::$monthDays[$month - 1];
          break;

        case 'L':
          $formatted .= $isLeap ? 1 : 0;
          break;

        case 'Y':
          $formatted .= $year;
          break;

        case 'y':
          $formatted .= substr($year, 2, 4);
          break;

        case 'o':
          $formatted .= $year;
          break;

        case 'a':
          $formatted .= t($names['ampm'][$ampm], [], $tc);
          break;

        case 'A':
          $formatted .= t($names['ampm'][$ampm], [], $tc);
          break;

        case 'c':
          $formatted .= "$year - $month - {$day}T";
          $formatted .= $this->xFormat('H:i:sP');
          break;

        case 'r':
          $formatted .= t($names['day_abbr'][$dayOfWeek], [], $tc) . ', ' . $day . ' ' .
            t($names['months'][$month], [], $tc) . ' ' . $year . $this->xFormat('H:i:s P');
          break;

        default:
          // Any format character not handled by Datex or extended class,
          // Will be handled by DateTime.
          $formatted .= ctype_alpha($format[$i]) ? $this->xFormat($format[$i]) : $format[$i];
          break;
      }
    }

    return $formatted;
  }

  public function setDateLocale($y = 0, $m = 0, $d = 0) {
    list($y, $m, $d) = self::toGregorian($y, $m, $d);
    $this->xSetDate($y, $m, $d);
    return $this;
  }

  public function getMonthNames() {
    /** @noinspection SpellCheckingInspection */
    return [
      t('Farvardin'),
      t('Ordibehesht'),
      t('Khordad'),
      t('Tir'),
      t('Mordad'),
      t('Shahrivar'),
      t('Mehr'),
      t('Aban'),
      t('Azar'),
      t('Dey'),
      t('Bahman'),
      t('Esfand'),
    ];
  }

  public function copy() {
    return new DatexPoorMansJaliliCalendar($this->timezone, $this->langCode);
  }

  public function validate(array $arr) {
    return NULL;
  }


  // ___________________________________________________________________________

  private static /** @noinspection SpellCheckingInspection */
    $names = [
    'en' => [
      'months' => [
        0 => 'Farvardin',
        1 => 'Ordibehesht',
        2 => 'Khordad',
        3 => 'Tir',
        4 => 'Mordad',
        5 => 'Shahrivar',
        6 => 'Mehr',
        7 => 'Aban',
        8 => 'Azar',
        9 => 'Dei',
        10 => 'Bahman',
        11 => 'Esfand',
      ],
      'ampm' => [
        'am' => 'Ghablazohr',
        'pm' => 'Badazohr',
      ],
      'day_abbr' => ['sh', 'y', 'd', 's', 'ch', 'p', 'j'],
      'day_abbr_short' => ['sh', 'y', 'd', 's', 'ch', 'p', 'j'],
      'day' => [
        'Shanbe',
        'Yekshanbe',
        'Doshanbe',
        'Seshanbe',
        'Cheharshanbe',
        'Panjshanbe',
        'Jome',
      ],
      'order' => [
        'Yekom',
        'Dovom',
        'Sevom',
        'Cheharom',
        'Panjom',
        'Sheshom',
        'Haftom',
        'Hashtom',
        'Nohom',
        'Dahom',
        'Yazdahom',
        'Davazdahom',
        'Sizdahom',
        'Chehardahom',
        'Panzdahom',
        'Shanzdahom',
        'Hefdahom',
        'Hejdahom',
        'Noozdahom',
        'Bistom',
        'Bisto yekom',
        'Bisto dovom',
        'Bisto sevom',
        'Bisto cheharom',
        'Bisto panjom',
        'Bisto sheshom',
        'Bisto haftom',
        'Bisto hashtom',
        'Bisto nohom',
        'Siom',
        'Sio yekom',
        'Sio dovom',
      ],

    ],
    'fa' => [
      'months' => [
        0 => 'فروردین',
        1 => 'اردیبهشت',
        2 => 'خرداد',
        3 => 'تیر',
        4 => 'مرداد',
        5 => 'شهریور',
        6 => 'مهر',
        7 => 'آبان',
        8 => 'آذر',
        9 => 'دی',
        10 => 'بهمن',
        11 => 'اسفند',
      ],
      'ampm' => [
        'am' => 'قبل‌ازظهر',
        'pm' => 'بعدازظهر',
      ],
      'day_abbr' => ['ش', 'ی', 'د', 'س', 'چ', 'پ', 'ج'],
      'day_abbr_short' => ['ش', 'ی', 'د', 'س', 'چ', 'پ', 'ج'],
      'day' => [
        'شنبه',
        'یک‌شنبه',
        'دوشنبه',
        'سه‌شنبه',
        'چهارشنبه',
        'پنج‌شنبه',
        'جمعه',
      ],
      'order' => [
        'یکم',
        'دوم',
        'سوم',
        'چهارم',
        'پنجم',
        'ششم',
        'هفتم',
        'هشتم',
        'نهم',
        'دهم',
        'یازدهم',
        'دوازده‌ام',
        'سیزده‌ام',
        'چهارده‌ام',
        'پانزده‌ام',
        'شانزده‌ام',
        'هفده‌ام',
        'هجده‌ام',
        'نوزده‌ام',
        'بیست‌ام',
        'بیست‌ویکم',
        'بیست‌ودوم',
        'بیست‌وسوم',
        'بیست‌وچهارم',
        'بیست‌وپنجم',
        'بیست‌وششم',
        'بیست‌وهفتم',
        'بیست‌وهشتم',
        'بیست‌ونهم',
        'سی‌ام',
        'سی‌ویکم',
        'سی‌ودوم',
      ],

    ],
  ];

  private static $monthDays =
    [31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29];

  /**
   * Constant used in calculating year.
   *
   * Length of a year Calculated by Khayam is 365.2422 days (approx.); but as
   * the years are getting shorter the new value (valid from year 1380
   * Per./2000 A.D.) is used instead.
   */
  private static $khayamYear = 365.24218956;

  /**
   * Correction to khayami constant.
   *
   * Recent calculations has introduced a correcting factor, which Khayam could
   * not reach. This is used to better adjust length of each year in seconds.
   */
  private static $khayamYearCorrection = 0.00000006152;

  /**
   * Reference table made by Khayam for leap years.
   */
  private static $khayamii = [
    5,
    9,
    13,
    17,
    21,
    25,
    29,
    34,
    38,
    42,
    46,
    50,
    54,
    58,
    62,
    67,
    71,
    75,
    79,
    83,
    87,
    91,
    95,
    100,
    104,
    108,
    112,
    116,
    120,
    124,
    0,
  ];

  /**
   * Count of days at the end of each Persian month.
   */
  private static $mCounter = [
    0,
    31,
    62,
    93,
    124,
    155,
    186,
    216,
    246,
    276,
    306,
    336,
  ];


  private static function toJalali($timestamp, $offset = 0) {
    $timestamp = $timestamp + $offset;
    // DateTime will handle time.
    $ts = floor($timestamp % 60);
    $tm = floor(($timestamp % 3600) / 60);
    $th = floor(($timestamp % 86400) / 3600);

    $d = floor($timestamp / 86400) + 287;

    $y = floor(
      ($d / self::$khayamYear) - ($d * self::$khayamYearCorrection)
    );

    $day_of_year = $d - round($y * self::$khayamYear, 0);
    if ($day_of_year == 0) {
      $day_of_year = 366;
    }

    $y += 1348;

    $m = 0;
    while ($m < 12 && $day_of_year > self::$mCounter[$m]) {
      $m++;
    }

    $d = $day_of_year - self::$mCounter[$m - 1];

    $day_of_week = self::dayOfWeek($y, $day_of_year);
    return [$y, $m, $d, $th, $tm, $ts, $day_of_year, $day_of_week];
  }

  private static function toGregorian($jalali_year = 0, $jalali_month = 0, $jalali_day = 0) {
    $now = self::toJalali(time());

    $jalali_year = ($jalali_year ? $jalali_year : $now[0]) - 979;
    $jalali_month = ($jalali_month ? $jalali_month : $now[1]) - 1;
    $jalali_day = ($jalali_day ? $jalali_day : $now[2]) - 1;

    $jalali_day_no = 365 * $jalali_year + intval($jalali_year / 33) * 8 + intval((($jalali_year % 33) + 3) / 4);
    for ($i = 0; $i < $jalali_month; ++$i) {
      $jalali_day_no += self::$monthDays[$i];
    }
    $jalali_day_no += $jalali_day;
    $gregorian_day_no = $jalali_day_no + 79;

    $g_year = 1600 + 400 * intval($gregorian_day_no / 146097);
    $gregorian_day_no = $gregorian_day_no % 146097;

    $leap = TRUE;
    if ($gregorian_day_no >= 36525) {
      $gregorian_day_no--;
      $g_year += 100 * intval($gregorian_day_no / 36524);
      $gregorian_day_no = $gregorian_day_no % 36524;

      if ($gregorian_day_no >= 365) {
        $gregorian_day_no++;
      }
      else {
        $leap = FALSE;
      }
    }

    $g_year += 4 * intval($gregorian_day_no / 1461);
    $gregorian_day_no %= 1461;

    if ($gregorian_day_no >= 366) {
      $leap = FALSE;

      $gregorian_day_no--;
      $g_year += intval($gregorian_day_no / 365);
      $gregorian_day_no = $gregorian_day_no % 365;
    }

    $daysInGregorianMonth = [
      31,
      28,
      31,
      30,
      31,
      30,
      31,
      31,
      30,
      31,
      30,
      31,
    ];
    for ($i = 0; $gregorian_day_no >= $daysInGregorianMonth[$i] + ($i == 1 && $leap); $i++) {
      $gregorian_day_no -= $daysInGregorianMonth[$i] + ($i == 1 && $leap);
    }
    $g_month = $i + 1;
    $g_day = $gregorian_day_no + 1;

    return [$g_year, $g_month, $g_day];
  }

  private static function isLeap($year) {
    $observationYear = $year + 2346;
    $year = ($observationYear % 2820) % 128;
    $is_leap = array_search($year, self::$khayamii);
    return $is_leap;
  }

  private static function dayOfWeek($year, $dayOfYear = 0) {
    $observationYear = $year + 2346;

    $count2820 = floor($observationYear / 2820);
    $mod2820 = $observationYear % 2820;
    $count128 = floor($mod2820 / 128);
    $mod128 = $mod2820 % 128;

    $leapCount = 0;
    while ($mod128 > self::$khayamii[$leapCount]) {
      $leapCount++;
    }

    $yearStartDay = ($count2820 + 1) * 3 +
      $count128 * 5 +
      $mod128 +
      $leapCount;

    if ($dayOfYear > 0) {
      $dayOfYear--;
    }

    return ($yearStartDay + $dayOfYear) % 7;
  }

}
