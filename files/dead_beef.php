<?php

class _datex_views_handler_filter_date extends views_handler_filter_date {

  function option_definition() {
    $options = parent::option_definition();
    $options['datex_schema'] = ['default' => 'default'];
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['datex_schema'] = [
      '#type' => 'select',
      '#title' => t('Datex schema'),
      '#options' => _datex_schema_form_options(),
      '#default_value' => isset($this->options['datex_schema'])
        ? $this->options['datex_schema']
        : 'default',
    ];

    $tvar = ['@fmt' => 'Y-m-d H:i:s', '@eg' => '2017-11-30 23:20:10'];
    $tmsg = '<b>Without Datex-Popup</b>, the <b>ONLY</b> format supported is @fmt E.g: @eg';
    $form['datex_attention'] = [
      '#type' => 'item',
      '#markup' => t($tmsg, $tvar),
    ];

    parent::options_form($form, $form_state);
  }

  function _exposed_validate(&$form, &$form_state) {

    return parent::expose_validate($form, $form_state);
    //    $value = &$form_state['values'][$this->options['expose']['identifier']];
    //
    //    $operator = !empty($this->options['expose']['use_operator'])
    //    && !empty($this->options['expose']['operator_id'])
    //      ? $form_state['values'][$this->options['expose']['operator_id']]
    //      : $this->operator;
    //
    //    $operators = $this->operators();
    //
    //    if ($operators[$operator]['values'] == 1) {
    //      $convert = $this->_datex_strtotime($value['value']);
    //      if (!empty($form['value']) && ($convert == -1 || $convert === FALSE)) {
    //        form_error($form['value'], t('Invalid date format.'));
    //      }
    //    }
    //    elseif ($operators[$operator]['values'] == 2) {
    //      $min = $this->_datex_strtotime($value['min']);
    //      if ($min == -1 || $min === FALSE) {
    //        form_error($form['min'], t('Invalid date format.'));
    //      }
    //      $max = $this->_datex_strtotime($value['max']);
    //      if ($max == -1 || $max === FALSE) {
    //        form_error($form['max'], t('Invalid date format.'));
    //      }
    //    }
    //
  }

  function _datex_strtotime($value) {
    return -1;
  }

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
  }

}
