This repository is mirror of git@git.drupal.org:project/datex.git
https://www.drupal.org/project/datex

### What is Datex

Datex is a zero-configuration, batteries-included, fire-and-forget, zero dependency date localization and internationalization module using php-intl. It is bundled with a popup date picker.

### Installation

drupal date, scheduler: Just download and enable datex.
Popup: D7: Just enable datex_popup, D8: works out of the box.
Webform: D7: Just enable datex_webfrm, D8: works out of the box.

If you are upgrading and get datex_api missing error, just disable and re-enable datex.
Fullcalendar

Drupal 7: install Fullcalendar as usual, download libraries as instructed on Fullcalendar's page (library version 1), replace library files with this one
Drupal 8: install Fullcalendar as usual, but use this library, and then enable datex_fullcalendar.

### Supported Modules

 - date (date field)
 - date popup
 - webform
 - scheduler
 - views
 - fullcalendar
 - views date format sql

### Features

 - Popup: for date fields, node edit form, scheduler, views exposed form, comment edit form, ...
 - Views Exposed Filters works just fine as long as the date field works.
 - Views Contextual Filter:node authored on date.
 - Views Contextual Filter: Date fields (year, year and month, year month day)
 - Node/Comment node and comment edit / add form (with and without popup).
 - Scheduler Module is fully supported, with and without popup.
 - Node admin page
 - Smaller Granularities: date fields with granularity lesser than year-month-day (including year only or year and month only) are supported. Great care has been taken to support this without time offset drift.
 - No patching the core anymore.
 - Intl-fallback: in case php-intl is missing a fallback calendar will be used.
 - Easy admin interface, with no footprint in the database.

