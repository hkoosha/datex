INTRODUCTION
------------

Datex uses PHP's IntlDateFormatter and makes all calendars in any language
available to site builders in Drupal.

By default datex works in `non-patching` mode (does not need to patch the core)
but has somewhat more limited functionality. Date, Views, and Panels modules work out
of the box, no patch is needed. If you find some date string not translated/localized
you could patch the Drupal core and configure datex accordingly to use the patch.

INSTALLATION
------------

  - Download and enabled datex as usual.
  - Preferably, enable the module Locale, and add desired languages to your site.
  - Go to admin/config/regional/date-time/datex and configure schemas.

If you want to apply the patch, the patch file resides in `files` directory in
this module's directory: drupal-date_format_hook_support-0-0.patch

JQUERY LIBRARY
--------------

Since default library of date (datepicker) does not support international
calendars, A very good robust library written by "Keith Wood" is used.
Download it from:

  - https://github.com/kbwood/calendars/releases

And extract it in "sites/all/libraries/jquery_calendars" of your Drupal.
So finally you will have a file like this: 
sites/all/libraries/jquery_calendars/dist/js/jquery.calendars.all.min.js

Or even better, use drush;

$> drush ldl jquery_calendars

`ldl` is the command provided by 'libraries' module.

CONFIGURATION
-------------

For each part of Drupal, Datex follows a behaviour called 'schema'. Different 
schemas can be defined and edited at datex configuration page. By default,
everything follows the default schema unless set otherwise.
Node display page, Views and other stuff always follow the default schema, But
date field widget, Display formatter, And views formatter can be configured
independently. Just go to the configuration form of each, And you see a option
indicating what datex should do. If it's Forced disabled, Then datex ignores 
the field completely.


TRANSLATION
-----------

No localized, Translated string is hardcoded in the code, So a persian month is
like 'Aban', You can translate it to "آبان" using locale.
