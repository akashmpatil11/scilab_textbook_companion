<?php
/**
 * @file
 * Track status of forms sent by post.
 */
function cheque_proposal_user() {
  return drupal_get_form('paper_submission_user_form');
}
function paper_submission_user_form($form, &$form_state) {
  global $user;
  $user_id = $user->uid;
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('uid', $user_id);
  $query->orderBy('id', 'DESC');
  $result = $query->execute();
  if (!$proposal_data = $result->fetchObject()) {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  }
  $proposal_id = $proposal_data->id;
  $query = db_select('textbook_companion_paper_user');
  $query->fields('textbook_companion_paper_user');
  $query->condition('proposal_id', $proposal_id);
  $result = $query->execute();
  if ($form_data = $result->fetchObject()) {
    $form['form_details'] = array(
      '#type' => 'fieldset',
      '#size' => '40',
      '#title' => t('Application Status')
      //'#collapsible' => FALSE,
      //'#collapsed' => FALSE,
    );
    $form_html .= '<ul>';
    if ($form_data->internship_form)
      $form_html .= '<li><strong>Internship Application: </strong> Form Submitted</li>';
    else
      $form_html .= '<li><strong>Internship Application: </strong> Form Not Submitted </li>';
    if ($form_data->copyright_form)
      $form_html .= '<li><strong>Copyright Application: </strong> Form Submitted</li>';
    else
      $form_html .= '<li><strong>Copyright Application: </strong> Form Not Submitted </li>';
    if ($form_data->undertaking_form)
      $form_html .= '<li><strong>Undertaking Application: </strong> Form Submitted</li>';
    else
      $form_html .= '<li><strong>Undertaking Application: </strong> Form Not Submitted </li>';
    $form_html .= '</ul>';
    $query = db_select('postal_services');
    $query->fields('postal_services');
    $query->condition('id', $form_data->post_service);
    $result = $query->execute();
    $data = $result->fetchObject();
    $form_html .= '<strong>Postal Service : </strong>' . $data->service_name . '<br>';
    $form_html .= '<strong>Tracking Details: </strong>' . $form_data->tracking_details . '<br>';
    $form_html .= 'Update your contact details ' . l('Here', 'mycontact') . '<br />';
    $form['form_details']['form'] = array(
      '#type' => 'item',
      '#markup' => $form_html
      //'#title' => t('Application Form Status')
    );
    drupal_set_message("You have sent us your form. We will get back to you after receiving it.", 'status');
  } else {
    $form['proposal_id'] = array(
      '#type' => 'hidden',
      '#value' => $proposal_id
    );
    $form['internshipform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Internship Application'),
      '#description' => t('I have sent Internship Application Form.')
    );
    $form['copyrighttransferform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Copyright Transfer Form'),
      '#description' => t('I have sent Copyright Transfer Form.')
    );
    $form['undertakingform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Undertaking Form'),
      '#description' => t('I have sent Undertaking Form.')
    );
    $form['recieptform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Recieved Reciept Form'),
      '#description' => t('I have sent Reciept Form.')
    );
    $service_options = array();
    $query = db_select('postal_services');
    $query->fields('postal_services');
    $result = $query->execute();
    while ($data = $result->fetchObject()) {
      $service_options[$data->id] = $data->service_name;
    }
    $form['post_service'] = array(
      '#type' => 'radios',
      '#title' => t('Select your postal/courier service.'),
      '#options' => $service_options,
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'tracking_details_ajax',
        'wrapper' => 'tracking-details-div',
        'method' => 'html',
        'effect' => 'fade'
      )
    );
    $form['tracking_details'] = array(
      '#prefix' => '<div id="tracking-details-div">',
      '#suffix' => '</div>',
      '#tree' => TRUE
    );
    if (isset($form_state['values']['post_service'])) {
      $form['tracking_details'] = array(
        '#type' => 'textfield',
        '#title' => 'Enter tracking details',
        '#size' => 30,
        '#prefix' => '<div id="tracking-details-div">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
        '#required' => TRUE
      );
    }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Send Email')
    );
    $form['cancel'] = array(
      '#type' => 'markup',
      '#value' => l(t('Cancel'), 'manage_proposal/all')
    );
  }
  return $form;
}

function paper_submission_user_form_submit($form, &$form_state) {
  /*$query ="UPDATE {textbook_companion_paper} SET internship_form = ".$form_state['values']['internshipform'].", copyright_form = ".$form_state['values']['copyrighttransferform'].", undertaking_form= ".$form_state['values']['undertakingform'].", reciept_form= ".$form_state['values']['recieptform']." WHERE proposal_id = ".$form_state['values']['proposal_id'];
  db_query($query);*/
  $query = "INSERT INTO {textbook_companion_paper_user} (internship_form, copyright_form, undertaking_form, reciept_form, proposal_id, post_service, tracking_details) VALUES (:internship_form, :copyright_form, :undertaking_form, :reciept_form, :proposal_id, :post_service, :tracking_details)";
  $args = array(
    ':internship_form' => $form_state['values']['internshipform'],
    ':copyright_form' => $form_state['values']['copyrighttransferform'],
    ':undertaking_form' => $form_state['values']['undertakingform'],
    ':reciept_form' => $form_state['values']['recieptform'],
    ":proposal_id" => $form_state['values']['proposal_id'],
    ':post_service' => $form_state['values']['post_service'],
    ':tracking_details' => $form_state['values']['tracking_details']
  );
  $result = db_query($query, $args, array(
    'return' => Database::RETURN_INSERT_ID
  ));
  drupal_set_message(t('Proposal Updated'), 'status');
}

function cheque_proposal_all() {
  $form['#redirect'] = FALSE;
  $form['search_cheque'] = array(
    '#type' => 'textfield',
    '#title' => t('Search'),
    '#size' => 48
  );
  $form['submit_cheque'] = array(
    '#type' => 'submit',
    '#value' => t('Search')
  );
  $form['cancel_cheque'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), '')
  );
  $count = 20;
  /* get pending proposals to be approved */
  $proposal_rows = array();
  /*$proposal_q = "SELECT * FROM {textbook_companion_proposal} ORDER BY id DESC";*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->orderBy('id', 'DESC');
  /*$pagerquery = pager_query($proposal_q, $count); */
  $pagerquery = $query->extend('PagerDefault')->limit($count)->execute();
  while ($proposal_data = $pagerquery->fetchObject()) {
    /* get preference */
    /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data = db_fetch_object($preference_q);*/
    $query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $query->condition('proposal_id', $proposal_data->id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $preference_data = $result->fetchObject();
    /*$preference_q1 = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data1 = db_fetch_object($preference_q1);*/
    $query = db_select('textbook_companion_proposal');
    $query->fields('textbook_companion_proposal');
    $query->condition('uid', $proposal_data->id);
    $query->condition('proposal_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $preference_data1 = $result->fetchObject();
    $proposal_status = '';
    switch ($proposal_data->proposal_status) {
      case 0:
        $proposal_status = 'Pending';
        break;
      case 1:
        $proposal_status = 'Approved';
        break;
      case 2:
        $proposal_status = 'Dis-approved';
        break;
      case 3:
        $proposal_status = 'Completed';
        break;
      default:
        $proposal_status = 'Unknown';
        break;
    } //$proposal_data->proposal_status
    $proposal_rows[] = array(
      date('d-m-Y', $proposal_data->creation_date),
      l($proposal_data->full_name, 'user/' . $proposal_data->uid),
      date('d-m-Y', $proposal_data->completion_date),
      l('Form Submission', 'manage_proposal/paper_submission/' . $proposal_data->id) . ' | ' . l('Cheque Details', 'cheque_contact/status/' . $proposal_data->id)
    );
  } //$proposal_data = $pagerquery->fetchObject()
  /* check if there are any pending proposals */
  if (!$proposal_rows) {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  } //!$proposal_rows
  $proposal_header = array(
    'Date of Submission',
    'Contributor Name',
    'Expected Date of Completion',
    'Status'
  );
  $output = theme('table', array(
    'header' => $proposal_header,
    'rows' => $proposal_rows
  ));
  return $output;
  //doubt
  //return $form;
}
function tracking_details_ajax($form, &$form_state) {
  return $form['tracking_details'];
}
