<?php

namespace Drupal\datex;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\DependencyInjection\ServiceProviderBase;


/**
 *
 * Automatically picked up by DI.
 *
 * @package Drupal\datex
 */
class DatexServiceProvider extends ServiceProviderBase {

  public function alter(ContainerBuilder $container) {
    $formatter = $container->getDefinition('date.formatter');
    $formatter->setClass(DatexFormatter::class);
  }

}
