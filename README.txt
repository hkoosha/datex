INTRODUCTION
------------

Datex is a zero-configuration, batteries-included, fire-and-forget, zero
dependency date localization and internationalization module. It supports
Gregorian (doh!), Persian, and... bundled with a nice jquery date picker.

It uses PHP-Intl but works without it too. To translate names (such as Shahrivar)
use the locale module in Drupal core. No particular configuration is needed. To
get popup support, enable date_popup in date module.


INSTALLATION
------------

  - Download and enabled datex as usual.
  - Enable Locale (comes with core), go to languages page, add one or more languages.
  - Go to admin/config/regional/date-time/datex and configure schemas.
  - If you get wrong <i>time</i>, set you'r site's timezone properly.
  - To get better support for views, enable date_views in date module.

JQUERY LIBRARY - POPUP DATEPICKER
--------------

It is not required to download any library, datex comes bundles with the great
calendar developed by (Babakhani)[https://github.com/babakhani/pwt.datepicker]
<b>BUT</b> It required (Jquery Update Module)[https://drupal.org/project/jquery_update].


FEATURES
----------

 - <b>Smaller Granularities:</b> date fields with granularity lesser than
   year-month-day (including year only or year and month only) are supported.
   Great care has been taken to support this without time offset drift.
 - <b>Views Contextual Filter:</b> <i>node created date</i> can be set as a
   contextual filter. <b>year</b> and <b>year and month</b> are supported. more
   support is underway.
 - <b>Date - Views Contextual Filter:</b> Schema based support for date field
    contextual filters are fully available.
 - <b>Views Exposed Filters</b> works just fine as long as the date field works.
   For instance the -between- operator is not supported by date module yet.
   node created is supported but popup for it is underway.
 - <b>Node/Comment</b> node and comment edit / add form are fully supported.
 - <b>Scheduler Module</b> is fully supported, with and without popup.
 - <b>Node admin page</b> is fully localized.

OTHER FEATURES
--------------

 - Intl-fallback: in case php-intl is missing a fallback calendar will be used.
 - Easy admin interface, with no footprint in the database.


FEATURE REQUESTS
----------------

Datex has a very clean readable code base, so if you wish to have something
added to datex, feel free to create a pull request.



