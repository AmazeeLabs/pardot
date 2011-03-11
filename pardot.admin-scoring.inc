<?php

/**
 * Form callback for managing and viewing scoring entries.
 */
function pardot_admin_scoring() {
  $form = array();

  $scores = db_query('SELECT * FROM {pardot_scoring}');
  while ($score = db_fetch_object($scores)) {
    $form['scores'][$score->scoring_id] = array(
      '#score' => $score,
      'path' => array(
        '#value' => $score->path,
      ),
      'score' => array(
        '#value' => $score->score,
      ),
    );
  }

  $form['new'] = array(
    'path' => array(
      '#prefix' => '<div class="add-new-placeholder">' . t('Add new path') . '</div>',
      '#type' => 'textfield',
      '#size' => 30,
    ),
    'score' => array(
      '#prefix' => '<div class="add-new-placeholder"></div>',
      '#type' => 'textfield',
      '#size' => 30,
    ),
  );

  $form['add'] = array(
    '#prefix' => '<div class="add-new-placeholder"></div>',
    '#type' => 'submit',
    '#value' => t('Add'),
  );


  return $form;
}

function pardot_admin_scoring_validate($form, $form_state) {
  if (!is_numeric($form_state['values']['score'])) {
    form_set_error('score', t('Your score must be a number'));
  }
  if (db_result(db_query("SELECT path FROM {pardot_scoring} WHERE path = '%s'", $form_state['values']['path']))) {
    form_set_error('path', t('Your path much be unique'));
  }
}

function pardot_admin_scoring_submit($form, $form_state) {
  $score = (object) $form_state['values'];
  drupal_write_record('pardot_scoring', $score);
}

function theme_pardot_admin_scoring($form) {

  //
  foreach (element_children($form['scores']) as $id) {
    $score = $form['scores'][$id]['#score'];

    $row = array();
    $row[] = drupal_render($form['scores'][$id]['path']);
    $row[] = drupal_render($form['scores'][$id]['score']);

    $ops = array();
    $ops[] = l(t('Edit'), 'admin/settings/pardot/scoring/' . $score->scoring_id . '/edit');
    $ops[] = l(t('Delete'), 'admin/settings/pardot/scoring/' . $score->scoring_id . '/delete');
    $row[] = implode(' | ', $ops);
    $rows[] = $row;
  }

  $rows[] = array(
    drupal_render($form['new']['path']),
    drupal_render($form['new']['score']),
    drupal_render($form['add']),
  );

  $headers = array(t('Path'), t('Score'), t('Operations'));
  $output = theme('table', $headers, $rows);

  $output .= drupal_render($form);

  return $output;
}

/**
 * Form callback for editing scoring entries.
 */
function pardot_admin_scoring_edit($form_state, $score) {

  $form = array();

  $form['scoring_id'] = array(
    '#type' => 'value',
    '#value' => $score->scoring_id,
  );
  $form['path'] = array(
    '#title' => t('Path'),
    '#type' => 'textfield',
    '#size' => 30,
    '#default_value' => $score->path,
  );
  $form['score'] = array(
    '#title' => t('Score'),
    '#type' => 'textfield',
    '#size' => 30,
    '#default_value' => $score->score,
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function pardot_admin_scoring_edit_validate($form, $form_state) {
  if (!is_numeric($form_state['values']['score'])) {
    form_set_error('score', t('Your score must be a number'));
  }
  if (db_result(db_query("SELECT path FROM {pardot_scoring} WHERE path = '%s' AND scoring_id <> %d", $form_state['values']['path'], $form_state['values']['scoring_id']))) {
    form_set_error('path', t('Your path much be unique'));
  }
}

function pardot_admin_scoring_edit_submit($form, &$form_state) {
  $score = (object) $form_state['values'];
  drupal_write_record('pardot_scoring', $score, array('scoring_id'));
  $form_state['redirect'] = 'admin/settings/pardot/scoring';
}

/**
 * Form callback for deleting scoring entries.
 */
function pardot_admin_scoring_delete($form_state, $score) {
  $form = array();
  $form['#scoring_id'] = $score->scoring_id;

  $description = t('Are you sure you want to delete this scoring. This path will no logger trigger additional scoring in Pardot.');

  return confirm_form($form, t('Are you sure you want to delete this score?'), 'admin/settings/pardot/scoring', $description, 'Delete');
}

function pardot_admin_scoring_delete_submit($form, &$form_state) {
  db_query('DELETE FROM {pardot_scoring} WHERE scoring_id = %d', $form['#scoring_id']);
  $form_state['redirect'] = 'admin/settings/pardot/scoring';
}