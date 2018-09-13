/**
 * Attaches the calendar behavior to all date-popup-enabled fields
 */
(function ($) {
    function sett(
        altField,
        minDate,
        maxDate,
        // -----------------
        timeOnly,
        hasTime,
        // -----------------
        hasHour,
        hourStep,
        hasMinute,
        minuteStep,
        hasSecond,
        secondStep,
        // -----------------
        hasDay,
        hasMonth,
        calType,
        viewMode,
        format
    ) {
        return {
            // inline: true,
            format: format,
            responsive: true,
            persianDigit: false,
            viewMode: "day",
            initialValue: false,
            minDate: minDate,
            maxDate: maxDate,
            autoClose: true,
            position: "auto",
            altFormat: "l - h/m/s",
            altField: altField,
            onlyTimePicker: timeOnly,
            // "onlySelectOnDate": false,
            calendarType: calType,
            inputDelay: 800,
            observer: false,
            calendar: {
                persian: {
                    locale: "fa",
                    showHint: true,
                    leapYearMode: "algorithmic"
                },
                gregorian: {
                    locale: "en",
                    showHint: true
                }
            },
            navigator: {
                enabled: true,
                scroll: {
                    enabled: true
                },
                text: {
                    btnNextText: "<",
                    btnPrevText: ">"
                }
            },
            toolbox: {
                enabled: true,
                calendarSwitch: {
                    enabled: true,
                    format: "MMMM"
                },
                todayButton: {
                    enabled: true,
                    text: {
                        fa: "امروز",
                        en: "Today"
                    }
                },
                submitButton: {
                    enabled: true,
                    text: {
                        fa: "تایید",
                        en: "Submit"
                    }
                },
                text: {
                    btnToday: "امروز"
                }
            },
            timePicker: {
                enabled: hasTime,
                // "step": 1,
                hour: {
                    enabled: hasHour,
                    step: hourStep
                },
                minute: {
                    enabled: hasMinute,
                    step: minuteStep
                },
                second: {
                    enabled: hasSecond,
                    step: secondStep
                },
                meridian: {
                    enabled: true
                }
            },
            dayPicker: {
                enabled: hasDay,
                titleFormat: "YYYY MMMM"
            },
            monthPicker: {
                enabled: hasMonth,
                titleFormat: "YYYY"
            },
            yearPicker: {
                enabled: true,
                titleFormat: "YYYY"
            },
            altFieldFormatter: function (unixDate) {
                return unixDate;
            },
        };
    }

    function process(who, settings, id) {
        var alt = '#datex-' + id;
        var configStr = $(alt).attr('data-datex-config');
        var c = configStr.split("|");
        var cfg = sett(
            alt,
            parseInt(c[0]), // min date
            parseInt(c[1]), // max date
            parseInt(c[2]) === 1, // time only
            parseInt(c[3]) === 1, // hasTime
            parseInt(c[4]) === 1, // hasHour
            parseInt(c[5]), // hourStep
            parseInt(c[6]) === 1, // hasMinute
            parseInt(c[7]), // minuteStep
            parseInt(c[8]) === 1, // hasSecond
            parseInt(c[9]), // secondStep
            parseInt(c[10]) === 1, // hasDay
            parseInt(c[11]) === 1, // hasMonth
            c[12], // cal type
            c[13], // view mode
            c[14] // format
        );
        var init = parseInt(c[15]);
        var pd = who.pDatepicker(cfg);
        pd.setDate(init);

    }

    function attach(ctx) {
        for (var id in Drupal.settings.datePopup) {
            if (!Drupal.settings.datePopup.hasOwnProperty(id)) {
                continue;
            }
            var data = Drupal.settings.datePopup[id];
            if (data.func === 'datepicker') {
                var find = $('#' + id);
                process(find, data.settings, id)
            }
        }
    }

    Drupal.behaviors.datex = {
        attach: attach
    };

})(jQuery);
