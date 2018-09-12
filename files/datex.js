/**
 * Attaches the calendar behavior to all required fields
 */
(function ($) {
    var i, j, id, datePopup, info,
        calendar, extend, lang, def,
        max, min, findme, fmt, attr,
        findmeName;

    function popupify(dis, dateId) {
        dis = $(dis);
        findmeName = 'input[data-datex-findme="' + dis[0].id + '"]';
        findme = $(findmeName);
        attr = function (name) {
            return findme.attr('data-datex-' + name);
        };

        calendar = $.calendars.instance(attr('calendar'), attr('langcode'));

        max = attr('maxdate').split('-');
        max = calendar.newDate(max[0], max[1], max[2]);
        min = attr('mindate').split('-');
        min = calendar.newDate(min[0], min[1], min[2]);
        def = attr('def').split('-');
        def = calendar.newDate(def[0], def[1], def[2]);

        fmt = attr('fmt');
        console.log(fmt);
        extend = {
            calendar: calendar,
            dateFormat: fmt,
            altField: findmeName,
            altFormat: 'yyyy-mm-dd'
            // minDate: min,
            // maxDate: max,
            // defaultDate: def
        };
        extend = $.extend(extend, $.calendars.picker.regional[lang]);

        dis.calendarsPicker(extend);
        dis.addClass('date-popup-init');
        dis.click(function () {
            dis.focus();
        });
        dis.addClass('date-popup-init');
    }

    Drupal.behaviors.datex = {
        attach: function (context) {
            for (id in Drupal.settings.datePopup) {
                $('#' + id).bind('focus', Drupal.settings.datePopup[id], function (e) {
                    if (!$(this).hasClass('date-popup-init')) {
                        datePopup = e.data;
                        switch (datePopup.func) {
                            case 'datepicker':
                                popupify(this, datePopup, id);
                                break;
                            case 'timeEntry':
                                $(this)
                                    .timeEntry(datePopup.settings)
                                    .addClass('date-popup-init');
                                $(this).click(function () {
                                    $(this).focus();
                                });
                                break;
                            case 'timepicker':
                                // Translate the PHP date format into the style the timepicker
                                // uses.
                                datePopup.settings.timeFormat = datePopup.settings.timeFormat
                                // 12-hour, leading zero,
                                    .replace('h', 'hh')
                                    // 12-hour, no leading zero.
                                    .replace('g', 'h')
                                    // 24-hour, leading zero.
                                    .replace('H', 'HH')
                                    // 24-hour, no leading zero.
                                    .replace('G', 'H')
                                    // AM/PM.
                                    .replace('A', 'p')
                                    // Minutes with leading zero.
                                    .replace('i', 'mm')
                                    // Seconds with leading zero.
                                    .replace('s', 'ss');

                                datePopup.settings.startTime = new Date(datePopup.settings.startTime);
                                $(this)
                                    .timepicker(datePopup.settings)
                                    .addClass('date-popup-init');
                                $(this).click(function () {
                                    $(this).focus();
                                });
                                break;
                        }
                    }
                });
            }
        }
    };

})(jQuery);
