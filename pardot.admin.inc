<?php

/**
 * Settings form for general pardot information.
 */
function pardot_admin_form() {
  $form = array();

  $form['pardot_a_id'] = array(
    '#title' => t('Pardot account id'),
    '#type' => 'textfield',
    '#description' => t('The value show in the pardot demo script for piAId. eg. if the script has piAId = "1001"; this field should be 1001'),
    '#default_value' => variable_get('pardot_a_id', ''),
  );
  $form['pardot_c_id'] = array(
    '#title' => t('Default Pardot campaign id'),
    '#type' => 'textfield',
    '#description' => t('The value show in the pardot demo script for piCId. eg. if the script has piCId = "1001"; this field should be 1001'),
    '#default_value' => variable_get('pardot_c_id', ''),
  );

  return system_settings_form($form);
}

function pardot_webform_components_form($form_state, $node) {
  $form = array();

  $form['#node'] = $node;
  $record = pardot_webform_load($node->nid);
  $form['#record'] = $record;

  $form['details'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
  );

  $form['details']['is_active'] = array(
    '#title' => 'Is active',
    '#type' => 'checkbox',
    '#default_value' => $record->is_active,
  );

  $form['details']['url'] = array(
    '#title' => 'Post url',
    '#type' => 'textfield',
    '#default_value' => $record->url,
    '#description' => t('Visit your form handler page in pardot and select "View form handler code", copy the url out of the example code and then paste it here. <br />!example', array(
      '!example' => theme('image', drupal_get_path('module', 'pardot') . '/form-handler-url.jpg'),
    )),
  );

  $form['components'] = array(
    '#tree' => TRUE,
  );
  foreach ($node->webform['components'] as $k => $component) {
    $form['components'][$k] = array(
      '#component' => $component,
      'key' => array(
        '#title' => 'Field name',
        '#type'  => 'textfield',
        '#size' => 25,
        '#default_value' => isset($record->data[$component['form_key']]['key']) ? $record->data[$component['form_key']]['key'] : '',
      ),
    );
  }

  $form['save'] = array(
    '#value' => t('Save'),
    '#type' => 'submit',
  );
//  $form['delete'] = array(
//    '#value' => t('Delete'),
//    '#type' => 'submit',
//  );

  return $form;
}

function pardot_webform_components_form_submit($form, $form_state) {

  $node = $form['#node'];

  $record = $form['#record'];
  if ($record) {
    $update = array('nid');
  }
  else {
    $record = new stdClass();
    $update = array();
  }

  $data = array();
  foreach (element_children($form['components']) as $k) {
    $component = $form['components'][$k]['#component'];
    $data[$component['form_key']]['key'] = $form_state['values']['components'][$k]['key'];
  }

  $record->nid = $node->nid;
  $record->url = $form_state['values']['url'];
  $record->is_active = (bool) $form_state['values']['is_active'];
  $record->data = $data;

  drupal_write_record('pardot_webform', $record, $update);
}

function theme_pardot_webform_components_form($form) {
  $output = '';

  $output .= drupal_render($form['details']);

  foreach (element_children($form['components']) as $k) {
    $row = array();

    // Name
    $row[] = $form['#node']->webform['components'][$k]['name'];

    // Type
    $row[] = $form['#node']->webform['components'][$k]['type'];

    // Pardot key
    unset($form['components'][$k]['key']['#title']);
    $row[] = drupal_render($form['components'][$k]['key']);

    $rows[] = $row;
  }
  $output .= theme('table', array('Name', 'Type', 'Pardot key'), $rows);

  $output .= drupal_render($form);

  return $output;
}
