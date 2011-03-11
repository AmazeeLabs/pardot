<?php

/**
 * Form callback for managing and viewing campaign entries.
 */
function pardot_admin_campaign() {
  $form = array();

  $campaigns = db_query('SELECT * FROM {pardot_campaign}');
  while ($campaign = db_fetch_object($campaigns)) {
    $form['campaigns'][$campaign->campaign_id] = array(
      '#campaign' => $campaign,
      'campaign_id' => array(
        '#value' => $campaign->campaign_id,
      ),
      'name' => array(
        '#value' => $campaign->name,
      ),
    	'paths' => array(
        '#value' => $campaign->paths,
      ),
    );
  }

  $form['new'] = array(
    'campaign_id' => array(
      '#prefix' => '<div class="add-new-placeholder">' . t('Campaign ID') . '</div>',
  		'#type' => 'textfield',
      '#size' => 10,
      '#description' => t('Numeric campaign code(piCId) from tracking code preview in Pardot administration interface.')
    ),
    'name' => array(
      '#prefix' => '<div class="add-new-placeholder">' . t('Campaign name') . '</div>',
    	'#type' => 'textfield',
      '#size' => 25,
      '#description' => t('A short descriptive name for administration purposes. Can be the same as the campaign name in Pardot but not required.')
    ),
    'paths' => array(
      '#prefix' => '<div class="add-new-placeholder">' . t('Add new path') . '</div>',
      '#type' => 'textarea',
      '#wysiwyg' => FALSE,
    	'#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog. <front> is the front page."),
    ),
  );

  $form['add'] = array(
    '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
  	'#type' => 'submit',
    '#value' => t('Add'),
  );


  return $form;
}

function pardot_admin_campaign_validate($form, $form_state) {

  if (db_result(db_query("SELECT campaign_id FROM {pardot_campaign} WHERE campaign_id = '%s'", $form_state['values']['campaign_id']))) {
    form_set_error('campaign_id', t('Your Campaign ID much be unique'));
  }
}

function pardot_admin_campaign_submit($form, $form_state) {
  $campaign = (object) $form_state['values'];
  drupal_write_record('pardot_campaign', $campaign);
}

function theme_pardot_admin_campaign($form) {

  foreach (element_children($form['campaigns']) as $id) {

    $row = array();
    $row[] = drupal_render($form['campaigns'][$id]['campaign_id']);
    $row[] = drupal_render($form['campaigns'][$id]['name']);
    $row[] = drupal_render($form['campaigns'][$id]['paths']);

    $ops = array();
    $ops[] = l(t('Edit'), 'admin/settings/pardot/campaign/' . $form['campaigns'][$id]['#campaign']->campaign_id . '/edit');
    $ops[] = l(t('Delete'), 'admin/settings/pardot/campaign/' . $form['campaigns'][$id]['#campaign']->campaign_id . '/delete');
    $row[] = implode(' | ', $ops);
    $rows[] = $row;
  }

  $rows[] = array(
    drupal_render($form['new']['campaign_id']),
    drupal_render($form['new']['name']),
    drupal_render($form['new']['paths']),
    drupal_render($form['add']),
  );

  $headers = array(t('Campaign ID'), t('Name'), t('Path'), t('Operations'));
  $output = theme('table', $headers, $rows);

  $output .= drupal_render($form);

  return $output;
}

/**
 * Form callback for editing campaign entries.
 */
function pardot_admin_campaign_edit($form_state, $campaign) {

  $form = array();

  $form['campaign_id'] = array(
    '#type' => 'value',
    '#value' => $campaign->campaign_id,
  );
  $form['name'] = array(
    '#title' => t('Campaign name'),
    '#type' => 'textfield',
    '#default_value' => $campaign->name,
    '#description' => t('A short descriptive name for administration purposes. Can be the same as the campaign name in Pardot but not required.'),
  );
  $form['paths'] = array(
    '#title' => t('Paths'),
    '#type' => 'textarea',
  	'#wysiwyg' => FALSE,
    '#default_value' => $campaign->paths,
    '#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog. <front> is the front page."),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function pardot_admin_campaign_edit_submit($form, &$form_state) {
  $campaign = (object) $form_state['values'];
  drupal_write_record('pardot_campaign', $campaign, array('campaign_id'));
  $form_state['redirect'] = 'admin/settings/pardot/campaign';
}

/**
 * Form callback for deleting campaign entries.
 */
function pardot_admin_campaign_delete($form_state, $campaign) {
  $form = array();
  $form['#campaign_id'] = $campaign->campaign_id;

  $description = t('Are you sure you want to delete this campaign. This paths will no logger trigger unique campaigns in Pardot.');

  return confirm_form($form, t('Are you sure you want to delete this campaign?'), 'admin/settings/pardot/campaign', $description, 'Delete');
}

function pardot_admin_campaign_delete_submit($form, &$form_state) {
  db_query("DELETE FROM {pardot_campaign} WHERE campaign_id = '%s'", $form['#campaign_id']);
  $form_state['redirect'] = 'admin/settings/pardot/campaign';
}