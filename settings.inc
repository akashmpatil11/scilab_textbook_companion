<?php
// $Id$
function textbook_companion_settings_form($form, &$form_state) {
  $form['emails'] = array(
    '#type' => 'textfield',
    '#title' => t('Notification emails'),
    '#description' => t('A comma separated list of email addresses to receive notifications emails [bcc]'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_emails', '')
  );
  $form['bcc_emails_rem'] = array(
    '#type' => 'textfield',
    '#title' => t('Notification emails of failed reminder bcc'),
    '#description' => t('A comma separated list of email addresses to receive notifications emails'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_emails_fail_rem_bcc', '')
  );
  $form['to_emails'] = array(
    '#type' => 'textfield',
    '#title' => t('Notification emails to all'),
    '#description' => t('A comma separated list of email addresses to receive notifications emails'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_emails_all', '')
  );
  $form['cc_emails'] = array(
    '#type' => 'textfield',
    '#title' => t('(Cc) Notification emails'),
    '#description' => t('Specify emails id for CC option of mail system with comma separated'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_cc_emails', '')
  );
  $form['from_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Outgoing from email address'),
    '#description' => t('Email address to be display in the from field of all outgoing messages'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_from_email', '')
  );
  $form['extensions']['source'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed source file extensions'),
    '#description' => t('A comma separated list WITHOUT SPACE of source file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_source_extensions', '')
  );
  $form['extensions']['sample_source_file'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed sample source file extensions'),
    '#description' => t('A comma separated list WITHOUT SPACE of source file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_sample_source_file_extensions', '')
  );
  $form['extensions']['dependency'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed dependency file extensions'),
    '#description' => t('A comma separated list WITHOUT SPACE of dependency file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_dependency_extensions', '')
  );
  $form['extensions']['result'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed result file extensions'),
    '#description' => t('A comma separated list WITHOUT SPACE of result file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_result_extensions', '')
  );
  $form['extensions']['xcos'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed xcos file extensions'),
    '#description' => t('A comma separated list WITHOUT SPACE of xcos file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_xcos_extensions', '')
  );
  $form['book_prefences'] = array(
    '#type' => 'select',
    '#title' => t('Number of books in proposal required'),
    '#options' => array(
      '1' => '1',
      '2' => '2',
      '3' => '3'
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_book_preferences', '')
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}
function textbook_companion_settings_form_validate($form, &$form_state) {
  return;
}
function textbook_companion_settings_form_submit($form, &$form_state) {
  variable_set('textbook_companion_emails', $form_state['values']['emails']);
  variable_set('textbook_companion_cc_emails', $form_state['values']['cc_emails']);
  variable_set('textbook_companion_emails_fail_rem_bcc', $form_state['values']['bcc_emails_rem']);
  variable_set('textbook_companion_emails_all', $form_state['values']['to_emails']);
  variable_set('textbook_companion_from_email', $form_state['values']['from_email']);
  variable_set('textbook_companion_source_extensions', $form_state['values']['source']);
  variable_set('textbook_companion_sample_source_file_extensions', $form_state['values']['sample_source_file']);
  variable_set('textbook_companion_dependency_extensions', $form_state['values']['dependency']);
  variable_set('textbook_companion_result_extensions', $form_state['values']['result']);
  variable_set('textbook_companion_xcos_extensions', $form_state['values']['xcos']);
  variable_set('textbook_companion_book_preferences', $form_state['values']['book_prefences']);
  drupal_set_message(t('Settings updated'), 'status');
}
