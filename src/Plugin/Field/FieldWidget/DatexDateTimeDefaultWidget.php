<?php

namespace Drupal\datex\Plugin\Field\FieldWidget;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\datetime\Plugin\Field\FieldType\DateTimeItem;
use Drupal\datetime\Plugin\Field\FieldWidget\DateTimeDefaultWidget;
use Drupal\datetime\Plugin\Field\FieldWidget\DateTimeWidgetBase;
use Drupal\datex\Datex\DatexDrupalDateTime;
use Symfony\Component\DependencyInjection\ContainerInterface;


/**
 * Plugin implementation of the 'datetime_default' widget.
 *
 * @FieldWidget(
 *   id = "datetime_default",
 *   label = @Translation("Datex date and time"),
 *   field_types = {
 *     "datetime"
 *   }
 * )
 */
class DatexDateTimeDefaultWidget extends DateTimeDefaultWidget {

  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element = parent::formElement($items, $delta, $element, $form, $form_state);

    $cal = datex_factory();
    if (!$cal) {
      return $element;
    }

    if ($items[$delta] && $items[$delta]->date) {
      $date = $items[$delta]->date;
      $date = DatexDrupalDateTime::convert($date);
      $element['value']['#default_value'] = $date;
    }

    $element['value']['#date_date_element'] = 'text';
    return $element;
  }

}
