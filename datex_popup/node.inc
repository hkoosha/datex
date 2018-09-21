<?php

/**
 * Duplicate of date_popup_authored.
 */

/**
 * Implements hook_form_BASE_FORM_ID_alter() for node_form.
 *
 * Replaces default Authored on field with a datepicker on node submission
 * forms.
 */
function datex_popup_form_node_form_alter(&$form, $form_state, $form_id) {
  if (!$form['author']['#access']) {
    return;
  }

  $calendar = datex_factory(NULL, NULL, 'en');
  if (!$calendar) {
    return;
  }

  $form['author']['date']['#type'] = 'date_popup';

  if (!empty($form['author']['date']['#default_value'])) {
    $date = new DateObject($form['author']['date']['#default_value'], NULL, 'Y-m-d H:i:s O');
    $form['author']['date']['#default_value'] = $calendar->xFormat('Y-m-d H:i:s');
  }

  $form['author']['date']['#date_year_range'] = '-100:+100';
  $form['author']['date']['#date_format'] = 'Y/m/d H:i';

  unset($form['author']['date']['#maxlength']);
  unset($form['author']['date']['#description']);

  $form['author']['date']['#after_build'][] = '_datex_popup_authored_node_element_after_build';
  $form['#submit'][] = '_datex_popup_authored_node_form_submit';
}

function _datex_popup_authored_node_element_after_build($element, &$form_state) {
  // Add a validate handler after the one that is added by date_popup.
  $element['#element_validate'][] = '_datex_popup_authored_element_validate';
  return $element;
}

function _datex_popup_authored_element_validate($element, &$form_state) {
  if (date_hidden_element($element) || is_string($element['#value'])) {
    return;
  }

  // If an error occurred in the validation of the date popup field the date
  // cannot be correctly rendered as a string. In this case clear the date value
  // to avoid subsequent errors when the node is validated.
  // @see date_popup_validate()
  // @see node_validate()
  $input_exists = NULL;
  $input = drupal_array_get_nested_value($form_state['values'], $element['#parents'], $input_exists);
  $date = date_popup_input_date($element, $input);
  if (is_object($date) && !empty($date->errors)) {
    $form_state['values']['date'] = NULL;
  }
}

function _datex_popup_authored_node_form_submit($form, &$form_state) {
  // @see http://drupal.org/node/847854
  if (isset($form_state['values']['date']) && $form_state['values']['date'] instanceof DateObject) {
    $form_state['values']['date'] = $form_state['values']['date']->format('Y-m-d H:i:s O');
  }
}
