/**
 * Attaches the calendar behavior to all required fields
 */
(function ($) {
    var i, j, id, datePopup, info,
        calendar, extend, lang, def,
        max, min, findme, fmt, attr,
        findmeName;

    function popupify(dis, id) {
    }

    Drupal.behaviors.datex = {
        attach: function (context) {
            for (id in Drupal.settings.datePopup) {
                var ddd = $('#' + id);
                var data = Drupal.settings.datePopup[id];
                if (data.func === 'datepicker') {
                    console.log(ddd);
                    ddd[0].pDatepicker();
                }
            }
        }
    };

})(jQuery);
