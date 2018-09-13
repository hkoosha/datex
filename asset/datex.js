/**
 * Attaches the calendar behavior to all date-popup-enabled fields
 */
(function ($) {
    function attach(ctx) {
        for (var id in Drupal.settings.datePopup) {
            if (!Drupal.settings.datePopup.hasOwnProperty(id)) {
                continue;
            }
            var data = Drupal.settings.datePopup[id];
            if (data.func === 'datepicker') {
                var find = $('#' + id);
                window.glob = find;
                console.log(find, 'find');
                find.pDatepicker();
            }
        }
    }

    Drupal.behaviors.datex = {
        attach: attach
    };

})(jQuery);
