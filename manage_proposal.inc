<?php
// You can find all functions regarding manage proposal
/*
"proposal_status is chaged to new_proposal_status"
textbook companion propasal table column "new_proposal_status" field values means
0 = 'Pending'
1 = 'Approved'
2 = 'Dis-approved'
*/
function _proposal_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  /*$pending_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 ORDER BY id DESC");*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  /*changed*/
  //$query->condition('proposal_status', 0);
  $query->condition('new_proposal_status', 0);
  $query->orderBy('id', 'DESC');
  $pending_q = $query->execute();
  while ($pending_data = $pending_q->fetchObject())
  {
    $pending_rows[$pending_data->id] = array(
      date('d-m-Y', $pending_data->creation_date),
      l($pending_data->full_name, 'user/' . $pending_data->uid),
      date('d-m-Y', $pending_data->proposed_completion_date),
      l('Approve', 'manage_proposal/approve/' . $pending_data->id) . ' | ' . l('Edit', 'manage_proposal/edit/' . $pending_data->id)
    );
  } //$pending_data = $pending_q->fetchObject()
  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no pending proposals.'), 'status');
    return '';
  } //!$pending_rows
  $pending_header = array(
    'Date of Submission',
    'Contributor Name',
    'Proposed Date of Completion',
    'Action'
  );
  $output = theme('table', array(
    'header' => $pending_header,
    'rows' => $pending_rows
  ));
  return $output;
}
/*new function _proposal_all() written | removed pervious one*/
function _proposal_all()
{
  function _tbc_ext($status, $preference_id)
  {
    if ($status == "Approved")
    {
      return " | " . l("ER", "tbc_external_review/add_book/" . $preference_id);
    } //$status == "Approved"
    else
    {
      return "";
    }
  }
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->orderBy('id', 'DESC');
  $proposal_q = $query->execute();
  while ($proposal_data = $proposal_q->fetchObject())
  {
    $proposal_status = '';
    switch ($proposal_data->new_proposal_status)
    {
      case 0:
        $proposal_status = 'Pending';
        break;
      case 1:
        $proposal_status = 'Approved';
        break;
      case 2:
        $proposal_status = 'Dis-approved';
        break;
    } //$proposal_data->proposal_status
    $proposal_rows[] = array(
      date('d-m-Y', $proposal_data->creation_date),
      l($proposal_data->full_name, 'user/' . $proposal_data->uid),
      date('d-m-Y', $proposal_data->proposed_completion_date),
      date('d-m-Y', $proposal_data->completion_date),
      $proposal_status,
      l('Status', 'manage_proposal/status/' . $proposal_data->id) . ' | ' . l('Edit', 'manage_proposal/edit/' . $proposal_data->id) . _tbc_ext($proposal_status, $preference_data->id)
    );
  } //$proposal_data = $proposal_q->fetchObject()
  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  } //!$proposal_rows
  $proposal_header = array(
    'Date of Submission',
    'Contributor Name',
    'Proposed Date of Completion',
    'Actual Date of Completion',
    'Status',
    'Action'
  );
  $output = theme('table', array(
    'header' => $proposal_header,
    'rows' => $proposal_rows
  ));
  return $output;
}

function _category_all()
{
  /* get pending proposals to be approved */
  $preference_rows = array();
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 ORDER BY id DESC");*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('approval_status', 1);
  $query->orderBy('id', 'DESC');
  $preference_q = $query->execute();
  while ($preference_data = $preference_q->fetchObject())
  {
    switch ($preference_data->category)
    {
      case 0:
        $category_data = 'Not Selected';
        break;
      case 1:
        $category_data = 'Fluid Mechanics';
        break;
      case 2:
        $category_data = 'Control Theory & Control Systems';
        break;
      case 3:
        $category_data = 'Chemical Engineering';
        break;
      case 4:
        $category_data = 'Thermodynamics';
        break;
      case 5:
        $category_data = 'Mechanical Engineering';
        break;
      case 6:
        $category_data = 'Signal Processing';
        break;
      case 7:
        $category_data = 'Digital Communications';
        break;
      case 8:
        $category_data = 'Electrical Technology';
        break;
      case 9:
        $category_data = 'Mathematics & Pure Science';
        break;
      case 10:
        $category_data = 'Analog Electronics';
        break;
      case 11:
        $category_data = 'Digital Electronics';
        break;
      case 12:
        $category_data = 'Computer Programming';
        break;
      case 13:
        $category_data = 'Others';
        break;
      default:
        $category_data = 'Unknown';
        break;
    } //$preference_data->category
    $preference_rows[] = array(
      $preference_data->book . "<br> <i>by " . $preference_data->author . "</i>",
      $preference_data->isbn,
      $preference_data->publisher,
      $preference_data->edition,
      $preference_data->year,
      $category_data,
      l('Edit', 'manage_proposal/category/edit/' . $preference_data->id)
    );
  } //$preference_data = $preference_q->fetchObject()
  $preference_header = array(
    'Book',
    'ISBN',
    'Publisher',
    'Edition',
    'Year',
    'Category',
    'Status'
  );
  $output = theme('table', array(
    'header' => $preference_header,
    'rows' => $preference_rows
  ));
  return $output;
}
/******************************************************************************/
/************************** PROPOSAL APPROVAL FORM ****************************/
/******************************************************************************/
function proposal_approval_form($form, &$form_state)
{
  global $user;
  /* get current proposal */
  $proposal_id = arg(2);
  /*$result = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 and id = %d", $proposal_id);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  //need to be removed
  //$query->condition('proposal_status', 0);
  $query->condition('new_proposal_status', 0);
  $query->condition('id', $proposal_id);
  $result = $query->execute();
  if ($result)
  {
    if ($row = $result->fetchObject())
    {
      /* everything ok */
    } //$row = $result->fetchObject()
    else
    {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    }
  } //$result
  else
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }
  $form['full_name'] = array(
    '#type' => 'item',
    '#markup' => l($row->full_name, 'user/' . $row->uid),
    '#title' => t('Contributor Name')
  );
  $form['email'] = array(
    '#type' => 'item',
    '#markup' => user_load($row->uid)->mail,
    '#title' => t('Email')
  );
  $form['mobile'] = array(
    '#type' => 'item',
    '#markup' => $row->mobile,
    '#title' => t('Mobile')
  );
  $form['how_project'] = array(
    '#type' => 'item',
    '#markup' => $row->how_project,
    '#title' => t('How did you come to know about this project')
  );
  $form['course'] = array(
    '#type' => 'item',
    '#markup' => $row->course,
    '#title' => t('Course')
  );
  $form['branch'] = array(
    '#type' => 'item',
    '#markup' => $row->branch,
    '#title' => t('Department/Branch')
  );
  $form['university'] = array(
    '#type' => 'item',
    '#markup' => $row->university,
    '#title' => t('University/Institute')
  );
  $form['city'] = array(
    '#type' => 'item',
    '#markup' => $row->city,
    '#title' => t('City')
  );
  $form['all_state'] = array(
    '#type' => 'item',
    '#markup' => $row->state,
    '#title' => t('State')
  );
  $form['country'] = array(
    '#type' => 'item',
    '#markup' => $row->country,
    '#title' => t('Country')
  );
  $form['pincode'] = array(
    '#type' => 'item',
    '#markup' => $row->pincode,
    '#title' => t('Pincode')
  );
  $form['faculty'] = array(
    '#type' => 'item',
    '#markup' => $row->faculty,
    '#title' => t('College Teacher/Professor')
  );
  $form['reviewer'] = array(
    '#type' => 'item',
    '#markup' => $row->reviewer,
    '#title' => t('Reviewer')
  );
  $form['proposed_completion_date'] = array(
    '#type' => 'item',
    '#markup' => date('d-m-Y', $row->proposed_completion_date),
    '#title' => t('Proposed Date of Completion')
  );
  $form['completion_date'] = array(
    '#type' => 'item',
    '#markup' => date('d-m-Y', $row->completion_date),
    '#title' => t('Actual Date of Completion')
  );
  $form['operating_system'] = array(
    '#type' => 'item',
    '#markup' => $row->operating_system,
    '#title' => t('Operating System')
  );
  $form['scilab_version'] = array(
    '#type' => 'item',
    '#markup' => $row->scilab_version,
    '#title' => t('Scilab Version')
  );
  /************************** reference link filter *******************/
  $url = '~(?:(https?)://([^\s<]+)|(www\.[^\s<]+?\.[^\s<]+))(?<![\.,:])~i';
  $reference = preg_replace($url, '<a href="$0" target="_blank" title="$0">$0</a>', $row->reference);
  /******************************/
  $form['reference'] = array(
    '#type' => 'item',
    '#markup' => $reference,
    '#title' => t('References')
  );
  $form['reason'] = array(
    '#type' => 'item',
    '#markup' => $row->reason,
    '#title' => t('Reasons')
  );
  /* get book preference */
  $preference_rows = array();
  // $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = :proposal_id ORDER BY pref_number ASC", array(':proposal_id' => $proposal_id));
  $preference_q = db_query(" SELECT * FROM list_of_category loc
                JOIN textbook_companion_preference tcp ON loc.category_id = tcp.category
                WHERE proposal_id = :proposal_id ORDER BY pref_number ASC
", array(
    ':proposal_id' => $proposal_id
  ));
  /*$query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->orderBy('pref_number', 'ASC');
  $preference_q = $query->execute();*/
  while ($preference_data = $preference_q->fetchObject())
  {
    $category_data = _tbc_list_of_category($preference_data->category);
    foreach ($category_data as $cat)
    {
      $preference_rows[$preference_data->id] = '
        <table class="book_prefrences">
          <tr class="even">
            <td><strong>Book</strong></td>
            <td>' . $preference_data->book . '</td>
          </tr>
          <tr class="odd">
            <td><strong>Author</strong></td>
            <td>' . $preference_data->author . '</td>
          </tr>
          <tr class="even">
            <td><strong>Edition</strong></td>
            <td>' . $preference_data->edition . '</td>
          </tr>
          <tr class="odd">
            <td><strong>Publisher</strong></td>
            <td>' . $preference_data->publisher . '</td>
          </tr>
          <tr class="even">
            <td><strong>Year</strong></td>
            <td>' . $preference_data->year . '</td>
          </tr>
          <tr class="odd">
            <td><strong>ISBN</strong></td>
            <td>' . $preference_data->isbn . '</td>
          </tr>
          <tr class="even">
            <td><strong>Category</strong></td>
            <td>' . $cat . '</td>
          </tr>
        </table><br><br>
      ';
    } //$category_data as $cat
  } //$preference_data = $preference_q->fetchObject()
  if ($row->proposal_type == 1)
  {
    $form['book_preference'] = array(
      '#type' => 'radios',
      '#options' => $preference_rows,
      '#title' => t('Book Preferences'),
      '#required' => TRUE
    );
  } //$row->proposal_type == 1
  else
  {
    $form['book_preference'] = array(
      '#type' => 'radios',
      '#title' => t('Book Preferences'),
      '#options' => $preference_rows,
      '#required' => TRUE
    );
  }
  if ($row->samplefilepath != "None")
  {
    $form['samplecode'] = array(
      '#type' => 'markup',
      '#markup' => l('Click here to download sample code', 'download/samplecode/' . $proposal_id) . "<br><br>"
    );
  } //$row->samplefilepath != "None"
  $form['disapprove'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disapprove all the above book preferences')
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason for disapproval'),
    '#states' => array(
      'visible' => array(
        ':input[name="disapprove"]' => array(
          'checked' => TRUE
        )
      ),
      'required' => array(
        ':input[name="disapprove"]' => array(
          'checked' => TRUE
        )
      )
    )
  );
  $form['proposal_type'] = array(
    '#type' => 'hidden',
    '#value' => $row->proposal_type
  );
  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal')
  );
  return $form;
}
function proposal_approval_form_submit($form, &$form_state)
{
  global $user;
  /* get current proposal */
  $proposal_id = $form_state['values']['proposal_id'];
  $result = db_query("SELECT * FROM {textbook_companion_proposal} WHERE new_proposal_status = 0 and id = :proposal_id", array(
    ':proposal_id' => $proposal_id
  ));
  if ($result)
  {
    if ($row = $result->fetchObject())
    {
      /* everything ok */
    } //$row = $result->fetchObject()
    else
    {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    }
  } //$result
  else
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }
  /* disapprove */
  if ($form_state['values']['disapprove'])
  {
    //var_dump(array(':approver_uid' => $user->uid,':approver_date'=> time(), ':message' => $form_state['values']['message'], ':id' =>$proposal_id));die;
    db_query("UPDATE {textbook_companion_proposal} SET approver_uid = :approver_uid, approval_date = :approval_date, proposal_status = 2,new_proposal_status = 2, message = :message WHERE id = :id", array(
      ':approver_uid' => $user->uid,
      ':approval_date' => time(),
      ':message' => $form_state['values']['message'],
      ':id' => $proposal_id
    ));
    db_query("UPDATE {textbook_companion_preference} SET approval_status = 2 WHERE proposal_id = :id", array(
      ':id' => $proposal_id
    ));
    /* unlock all the aicte books */
    if ($form_state['values']['proposal_type'] == 0)
    {
      $query = "
      UPDATE textbook_companion_aicte
      SET status = 0, uid = 0, proposal_id = 0, preference_id = 0
      WHERE proposal_id = {:proposal_id}
    ";
      db_query($query, array(
        ':proposal_id' => $proposal_id
      ));
    } //$form_state['values']['proposal_type'] == 0
    /* sending email */
    $book_user = user_load($row->uid);
    $email_to = $book_user->mail;
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $params['proposal_disapproved']['proposal_id'] = $proposal_id;
    $params['proposal_disapproved']['user_id'] = $row->uid;
    $params['proposal_disapproved']['headers'] = array(
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc
    );
    if (!drupal_mail('textbook_companion', 'proposal_disapproved', $email_to, language_default(), $params, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');
    drupal_set_message('Book proposal dis-approved. User has been notified of the dis-approval.', 'error');
    drupal_goto('manage_proposal');
    return;
  } //$form_state['values']['disapprove']
  /* get book preference and set the status */
  $preference_id = $form_state['values']['book_preference'];
  $query = db_update('textbook_companion_proposal');
  $query->fields(array(
    'approver_uid' => $user->uid,
    'approval_date' => time(),
    'proposal_status' => 1,
    'new_proposal_status' => 1,
  ));
  $query->condition('id', $proposal_id);
  $num_updated = $query->execute();
  /*db_query("UPDATE {textbook_companion_preference} SET approval_status = 1 WHERE id = %d", $preference_id);*/
  $query = db_update('textbook_companion_preference');
  $query->fields(array(
    'approval_status' => 1
  ));
  $query->condition('id', $preference_id);
  $num_updated = $query->execute();
  $query = "
    UPDATE textbook_companion_aicte
    SET status = 0, uid = 0, proposal_id = 0, preference_id = 0
    WHERE proposal_id = {:proposal_id} AND preference_id != {:preference_id}
  ";
  db_query($query, array(
    ':proposal_id' => $proposal_id,
    ':preference_id' => $preference_id
  ));
  /* sending email */
  $book_user = user_load($row->uid);
  $email_to = $book_user->mail;
  $from = variable_get('textbook_companion_from_email', '');
  $bcc = variable_get('textbook_companion_emails', '');
  $cc = variable_get('textbook_companion_cc_emails', '');
  $params['proposal_approved']['proposal_id'] = $proposal_id;
  $params['proposal_approved']['user_id'] = $row->uid;
  $params['proposal_approved']['headers'] = array(
    'From' => $from,
    'MIME-Version' => '1.0',
    'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    'Content-Transfer-Encoding' => '8Bit',
    'X-Mailer' => 'Drupal',
    'Cc' => $cc,
    'Bcc' => $bcc
  );
  if (!drupal_mail('textbook_companion', 'proposal_approved', $email_to, language_default(), $params, $from, TRUE))
    drupal_set_message('Error sending email message.', 'error');
  drupal_set_message('Book proposal approved. User has been notified of the approval', 'status');
  drupal_goto('manage_proposal');
  return;
}
/******************************************************************************/
/*************************** PROPOSAL STATUS FORM *****************************/
/******************************************************************************/
function proposal_status_form($form, &$form_state)
{
  global $user;
  /* get current proposal */
  $proposal_id = arg(2);
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $proposal_id);
  $proposal_q = $query->execute();
  if (!$proposal_data = $proposal_q->fetchObject())
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  } //!$proposal_data = $proposal_q->fetchObject()
  $form['full_name'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->full_name,
    '#title' => t('Contributor Name')
  );
  $form['email'] = array(
    '#type' => 'item',
    '#markup' => user_load($proposal_data->uid)->mail,
    '#title' => t('Email')
  );
  $form['mobile'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->mobile,
    '#title' => t('Mobile')
  );
  $form['how_project'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->how_project,
    '#title' => t('How did you come to know about this project')
  );
  $form['course'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->course,
    '#title' => t('Course')
  );
  $form['branch'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->branch,
    '#title' => t('Department/Branch')
  );
  $form['university'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->university,
    '#title' => t('University/Institute')
  );
  $form['city'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->city,
    '#title' => t('City')
  );
  $form['all_state'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->state,
    '#title' => t('State')
  );
  $form['country'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->country,
    '#title' => t('Country')
  );
  $form['pincode'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->pincode,
    '#title' => t('Pincode')
  );
  $form['faculty'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->faculty,
    '#title' => t('College Teacher/Professor')
  );
  $form['reviewer'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->reviewer,
    '#title' => t('Reviewer')
  );
  $form['proposed_completion_date'] = array(
    '#type' => 'item',
    '#markup' => date('d-m-Y', $proposal_data->proposed_completion_date),
    '#title' => t('Proposed Date of Completion')
  );
  $form['completion_date'] = array(
    '#type' => 'item',
    '#markup' => date('d-m-Y', $proposal_data->completion_date),
    '#title' => t('Actual Date of Completion')
  );
  $form['operating_system'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->operating_system,
    '#title' => t('Operating System')
  );
  $form['scilab_version'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->scilab_version,
    '#title' => t('Scilab Version')
  );
  if ($proposal_data->proposal_type == 1)
  {
    $form['reason'] = array(
      '#type' => 'item',
      '#markup' => $proposal_data->reason,
      '#title' => t('Reason')
    );
    $form['reference'] = array(
      '#type' => 'item',
      '#markup' => $proposal_data->reference,
      '#title' => t('References')
    );
  } //$proposal_data->proposal_type == 1
  /* get book preference */
  $preference_html = '<ul>';
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->orderBy('pref_number', 'ASC');
  $preference_q = $query->execute();
  while ($preference_data = $preference_q->fetchObject())
  {
    if ($preference_data->approval_status == 1)
      $preference_html .= '<li><strong>' . $preference_data->book . ' (Written by ' . $preference_data->author . ')  - Approved Book</strong></li>';
    else
      $preference_html .= '<li>' . $preference_data->book . ' (Written by ' . $preference_data->author . ')</li>';
  } //$preference_data = $preference_q->fetchObject()
  $preference_html .= '</ul>';
  $form['book_preference'] = array(
    '#type' => 'item',
    '#markup' => $preference_html,
    '#title' => t('Book Preferences')
  );
  $proposal_status = '';
  switch ($proposal_data->proposal_status)
  {
    case 0:
      $proposal_status = t('Pending');
      break;
    case 1:
      $proposal_status = t('Approved');
      break;
    case 2:
      $proposal_status = t('Dis-approved');
      break;
    case 3:
      $proposal_status = t('Completed');
      break;
    case 4:
      $proposal_status = t('External');
      break;
    default:
      $proposal_status = t('Unkown');
      break;
  } //$proposal_data->proposal_status
  $form['proposal_status'] = array(
    '#type' => 'item',
    '#markup' => $proposal_status,
    '#title' => t('Proposal Status')
  );
  if ($proposal_data->proposal_status == 2)
  {
    $form['message'] = array(
      '#type' => 'item',
      '#markup' => $proposal_data->message,
      '#title' => t('Reason for disapproval')
    );
  } //$proposal_data->proposal_status == 2
  if ($proposal_data->proposal_status == 1 || $proposal_data->proposal_status == 4)
  {
    $form['completed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Completed'),
      '#description' => t('Check if user has completed all the book examples.')
    );
  } //$proposal_data->proposal_status == 1 || $proposal_data->proposal_status == 4
  if ($proposal_data->proposal_status == 0)
  {
    $form['approve'] = array(
      '#type' => 'item',
      '#markup' => l('Click here', 'manage_proposal/approve/' . $proposal_id),
      '#title' => t('Approve')
    );
  } //$proposal_data->proposal_status == 0
  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal/all')
  );
  return $form;
}
function proposal_status_form_submit($form, &$form_state)
{
  global $user;
  /* get current proposal */
  $proposal_id = $form_state['values']['proposal_id'];
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $proposal_id);
  $proposal_q = $query->execute();
  if (!$proposal_data = $proposal_q->fetchObject())
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  } //!$proposal_data = $proposal_q->fetchObject()
  /* set the book status to completed */
  if ($form_state['values']['completed'] == 1)
  {
    /*db_query("UPDATE {textbook_companion_proposal} SET proposal_status = 3 WHERE id = %d", $proposal_id);*/
    $query = db_update('textbook_companion_proposal');
    $query->fields(array(
      'proposal_status' => 3,
      //book_status => 'completed'
      'completion_date' => time()
    ));
    $query->condition('id', $proposal_id);
    $num_updated = $query->execute();
    /* sending email */
    $book_user = user_load($proposal_data->uid);
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $email_to = $book_user->mail;
    $params['proposal_completed']['proposal_id'] = $proposal_id;
    $params['proposal_completed']['user_id'] = $proposal_data->uid;
    $params['proposal_completed']['headers'] = array(
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc
    );
    if (!drupal_mail('textbook_companion', 'proposal_completed', $email_to, language_default(), $params, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');
    drupal_set_message('Congratulations! Book proposal has been marked as completed. User has been notified of the completion.', 'status');
  } //$form_state['values']['completed'] == 1
  drupal_goto('manage_proposal');
  return;
}
/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/
function proposal_edit_form($form, &$form_state, $nonaicte_book)
{
  global $user;
  /* get current proposal */
  $proposal_id = arg(2);
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $proposal_id);
  $proposal_q = $query->execute();
  if ($proposal_q)
  {
    $proposal_data = $proposal_q->fetchObject();
    if (!$proposal_data)
    {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    } //!$proposal_data
  } //$proposal_q
  else
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }
  $user_data = user_load($proposal_data->uid);
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->condition('pref_number', 1);
  $query->range(0, 1);
  $preference1_q = $query->execute();
  $preference1_data = $preference1_q->fetchObject();
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->condition('pref_number', 2);
  $query->range(0, 1);
  $preference2_q = $query->execute();
  $preference2_data = $preference2_q->fetchObject();
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->condition('pref_number', 3);
  $query->range(0, 1);
  $preference3_q = $query->execute();
  $preference3_data = $preference3_q->fetchObject();
  $form['full_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full Name'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->full_name
  );
  $form['email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user_data->mail,
    '#disabled' => TRUE
  );
  $form['mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Mobile No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
    '#default_value' => $proposal_data->mobile
  );
  /*$form['how_project'] = array(
  '#type' => 'select',
  '#title' => t('How did you come to know about this project'),
  '#options' => array('Scilab Website' => 'Scilab Website',
  'Friend' => 'Friend',
  'Professor/Teacher' => 'Professor/Teacher',
  'Mailing List' => 'Mailing List',
  'Poster in my/other college' => 'Poster in my/other college',
  'Others' => 'Others'),
  '#required' => TRUE,
  '#default_value' => $proposal_data->how_project,
  );*/
  $form['course'] = array(
    '#type' => 'textfield',
    '#title' => t('Course'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->course
  );
  $form['branch'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => _tbc_list_of_departments(),
    '#required' => TRUE,
    '#default_value' => $proposal_data->branch
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->university
  );
  $form['country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => array(
      'India' => 'India',
      'Others' => 'Others'
    ),
    '#required' => TRUE,
    '#tree' => TRUE,
    '#validated' => TRUE,
    '#default_value' => $proposal_data->country
  );
  $form['other_country'] = array(
    '#type' => 'textfield',
    '#title' => t('Other than India'),
    '#size' => 100,
    '#attributes' => array(
      'placeholder' => t('Enter your country name')
    ),
    '#default_value' => $proposal_data->country,
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array(
          'value' => 'Others'
        )
      )
    )
  );
  $form['other_state'] = array(
    '#type' => 'textfield',
    '#title' => t('State other than India'),
    '#size' => 100,
    '#attributes' => array(
      'placeholder' => t('Enter your state/region name')
    ),
    '#default_value' => $proposal_data->state,
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array(
          'value' => 'Others'
        )
      )
    )
  );
  $form['other_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City other than India'),
    '#size' => 100,
    '#attributes' => array(
      'placeholder' => t('Enter your city name')
    ),
    '#default_value' => $proposal_data->city,
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array(
          'value' => 'Others'
        )
      )
    )
  );
  $form['all_state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#options' => _tbc_list_of_states(),
    '#validated' => TRUE,
    '#default_value' => $proposal_data->state,
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array(
          'value' => 'India'
        )
      )
    )
  );
  $form['city'] = array(
    '#type' => 'select',
    '#title' => t('City'),
    '#options' => _tbc_list_of_cities(),
    '#default_value' => $proposal_data->city,
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array(
          'value' => 'India'
        )
      )
    )
  );
  $form['pincode'] = array(
    '#type' => 'textfield',
    '#title' => t('Pincode'),
    '#size' => 30,
    '#maxlength' => 6,
    '#default_value' => $proposal_data->pincode,
    '#required' => False,
    '#attributes' => array(
      'placeholder' => 'Enter pincode....'
    )
  );
  $form['faculty'] = array(
    '#type' => 'textfield',
    '#title' => t('College Teacher/Professor'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->faculty
  );
  $form['reviewer'] = array(
    '#type' => 'textfield',
    '#title' => t('Reviewer'),
    '#size' => 30,
    '#maxlength' => 100,
    '#default_value' => $proposal_data->reviewer
  );
  /*
   $form['completion_date'] = array(
     '#type' => 'textfield',
     '#title' => t('Expected Date of Completion'),
     '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
     '#size' => 10,
     '#maxlength' => 10,
     '#default_value' => date('d-m-Y', $proposal_data->completion_date)
  );
  */
  //#default_value is in form yyyy-mm-dd
  //date range is yet to set
  $form['completion_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Expected Date of Completion'),
    '#date_increment' => 15,
    '#date_year_range' => 'now:+3',
    '#default_value' => date('Y-m-d', $proposal_data->completion_date),
    '#date_format' => 'd-m-Y',
    '#attributes' => array(
      'autocomplete' => 'off',
      'readonly' => 'readonly'
    ),
    "#required" => TRUE
  );
  /*
  $form['proposed_completion_date'] = array(
    '#type' => 'textfield',
    '#title' => t('c'),
    '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
    '#size' => 10,
    '#maxlength' => 10,
  );
  '#default_value' => date('d-m-Y', $proposal_data->proposed_completion_date)
  */
  //date range is yet to set
  $form['proposed_completion_date'] = array(
    '#type' => 'date_popup',
    '#title' => t(' Proposal Comletion Date'),
    '#date_increment' => 15,
    '#date_year_range' => 'now:+3',
    '#default_value' => date('Y-m-d', $proposal_data->proposed_completion_date),
    '#date_format' => 'd-m-Y',
    '#attributes' => array(
        'autocomplete' => 'off',
       'readonly' => 'readonly'
    ),
    "#required" => TRUE
  );
  list($scilab, $version) = explode(' ', $proposal_data->scilab_version);
  $scilab_version = $version;
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Scilab Version'),
    '#size' => 10,
    '#maxlength' => 20,
    '#default_value' => $scilab_version
  );
  $form['operating_system'] = array(
    '#type' => 'textfield',
    '#title' => t('Operating System'),
    '#size' => 30,
    '#maxlength' => 50,
    '#default_value' => $proposal_data->operating_system
  );
  $form['preference1'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 1'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['preference1']['book1'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference1_data->book
  );
  $form['preference1']['author1'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference1_data->author
  );
  $form['preference1']['isbn1'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference1_data->isbn
  );
  $form['preference1']['publisher1'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference1_data->publisher
  );
  $form['preference1']['edition1'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference1_data->edition
  );
  $form['preference1']['year1'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference1_data->year
  );
  $form['preference1']['book_category_1'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => _tbc_list_of_category(),
    '#required' => TRUE,
    '#default_value' => $preference1_data->category
  );
  if ($preference2_data)
  {
    $form['preference2'] = array(
      '#type' => 'fieldset',
      '#title' => t('Book Preference 2'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE
    );
    $form['preference2']['book2'] = array(
      '#type' => 'textfield',
      '#title' => t('Title of the book'),
      '#size' => 30,
      '#maxlength' => 100,
      '#required' => TRUE,
      '#default_value' => $preference2_data->book
    );
    $form['preference2']['author2'] = array(
      '#type' => 'textfield',
      '#title' => t('Author Name'),
      '#size' => 30,
      '#maxlength' => 100,
      '#required' => TRUE,
      '#default_value' => $preference2_data->author
    );
    $form['preference2']['isbn2'] = array(
      '#type' => 'textfield',
      '#title' => t('ISBN No'),
      '#size' => 30,
      '#maxlength' => 25,
      '#required' => TRUE,
      '#default_value' => $preference2_data->isbn
    );
    $form['preference2']['publisher2'] = array(
      '#type' => 'textfield',
      '#title' => t('Publisher & Place'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
      '#default_value' => $preference2_data->publisher
    );
    $form['preference2']['edition2'] = array(
      '#type' => 'textfield',
      '#title' => t('Edition'),
      '#size' => 4,
      '#maxlength' => 2,
      '#required' => TRUE,
      '#default_value' => $preference2_data->edition
    );
    $form['preference2']['year2'] = array(
      '#type' => 'textfield',
      '#title' => t('Year of pulication'),
      '#size' => 4,
      '#maxlength' => 4,
      '#required' => TRUE,
      '#default_value' => $preference2_data->year
    );
    $form['preference2']['book_category_2'] = array(
      '#type' => 'select',
      '#title' => t('Category'),
      '#options' => _tbc_list_of_category(),
      '#required' => TRUE,
      '#default_value' => $preference2_data->category
    );
  } //$preference2_data
  if ($preference3_data)
  {
    $form['preference3'] = array(
      '#type' => 'fieldset',
      '#title' => t('Book Preference 3'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE
    );
    $form['preference3']['book3'] = array(
      '#type' => 'textfield',
      '#title' => t('Title of the book'),
      '#size' => 30,
      '#maxlength' => 100,
      '#required' => TRUE,
      '#default_value' => $preference3_data->book
    );
    $form['preference3']['author3'] = array(
      '#type' => 'textfield',
      '#title' => t('Author Name'),
      '#size' => 30,
      '#maxlength' => 100,
      '#required' => TRUE,
      '#default_value' => $preference3_data->author
    );
    $form['preference3']['isbn3'] = array(
      '#type' => 'textfield',
      '#title' => t('ISBN No'),
      '#size' => 30,
      '#maxlength' => 25,
      '#required' => TRUE,
      '#default_value' => $preference3_data->isbn
    );
    $form['preference3']['publisher3'] = array(
      '#type' => 'textfield',
      '#title' => t('Publisher & Place'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
      '#default_value' => $preference3_data->publisher
    );
    $form['preference3']['edition3'] = array(
      '#type' => 'textfield',
      '#title' => t('Edition'),
      '#size' => 4,
      '#maxlength' => 2,
      '#required' => TRUE,
      '#default_value' => $preference3_data->edition
    );
    $form['preference3']['year3'] = array(
      '#type' => 'textfield',
      '#title' => t('Year of pulication'),
      '#size' => 4,
      '#maxlength' => 4,
      '#required' => TRUE,
      '#default_value' => $preference3_data->year
    );
    $form['preference3']['book_category_3'] = array(
      '#type' => 'select',
      '#title' => t('Category'),
      '#options' => _tbc_list_of_category(),
      '#required' => TRUE,
      '#default_value' => $preference3_data->category
    );
  } //$preference3_data
  /* hidden fields */
  $form['hidden_proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal')
  );
  return $form;
}
function proposal_edit_form_validate($form, &$form_state)
{
  /* version */
  if (!preg_match('/^\d+(?:\.\d+)+/', $form_state['values']['version']))
    form_set_error('version', t('Please enter correct version in speacifed format'));
  /* mobile */
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
    form_set_error('mobile', t('Invalid mobile number'));

  //No need to validate date of completion as date_popup api handles it
  /* date of completion */
  //if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
  //   form_set_error('completion_date', t('Invalid expected date of completion'));
   list($y, $m, $d) = explode('-', $form_state['values']['completion_date']);
   $d = (int) $d;
   $m = (int) $m;
   $y = (int) $y;
   if (!checkdate($m, $d, $y))
     form_set_error('completion_date', t('Invalid expected date of completion'));
  if (mktime(0, 0, 0, $m, $d, $y) <= time())
  form_set_error('completion_date', t('Expected date of completion should be in future'));
  /* edition */
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition1']))
    form_set_error('edition1', t('Invalid edition for Book Preference 1'));
  if ($form_state['values']['book2'])
  {
    if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition2']))
    {
      form_set_error('edition2', t('Invalid edition for Book Preference 2'));
    } //!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition2'])
  } //$form_state['values']['book2']
  if ($form_state['values']['book3'])
  {
    if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition3']))
    {
      form_set_error('edition3', t('Invalid edition for Book Preference 3'));
    } //!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition3'])
  } //$form_state['values']['book3']
  /* year of publication */
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year1']))
    form_set_error('year1', t('Invalid year of pulication for Book Preference 1'));
  if ($form_state['values']['book2'])
  {
    if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year2']))
    {
      form_set_error('year2', t('Invalid year of pulication for Book Preference 2'));
    } //!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year2'])
  } //$form_state['values']['book2']
  if ($form_state['values']['book3'])
  {
    if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year3']))
    {
      form_set_error('year3', t('Invalid year of pulication for Book Preference 3'));
    } //!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year3'])
  } //$form_state['values']['book3']
  /* year of publication */
  $cur_year = date('Y');
  if ((int) $form_state['values']['year1'] > $cur_year)
    form_set_error('year1', t('Year of pulication should be not in the future for Book Preference 1'));
  if ($form_state['values']['book2'])
  {
    if ((int) $form_state['values']['year2'] > $cur_year)
    {
      form_set_error('year2', t('Year of pulication should be not in the future for Book Preference 2'));
    } //(int) $form_state['values']['year2'] > $cur_year
  } //$form_state['values']['book2']
  if ($form_state['values']['book3'])
  {
    if ((int) $form_state['values']['year3'] > $cur_year)
    {
      form_set_error('year3', t('Year of pulication should be not in the future for Book Preference 3'));
    } //(int) $form_state['values']['year3'] > $cur_year
  } //$form_state['values']['book3']
  /* isbn */
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn1']))
    form_set_error('isbn1', t('Invalid ISBN for Book Preference 1'));
  if ($form_state['values']['book2'])
  {
    if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn2']))
    {
      form_set_error('isbn2', t('Invalid ISBN for Book Preference 2'));
    } //!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn2'])
  } //$form_state['values']['book2']
  if ($form_state['values']['book3'])
  {
    if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn3']))
    {
      form_set_error('isbn3', t('Invalid ISBN for Book Preference 3'));
    } //!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn3'])
  } //$form_state['values']['book3']
  if ($form_state['values']['country'] == 'Others')
  {
    if ($form_state['values']['other_country'] == '')
    {
      form_set_error('other_country', t('Enter country name'));
      // $form_state['values']['country'] = $form_state['values']['other_country'];
    } //$form_state['values']['other_country'] == ''
    else
    {
      $form_state['values']['country'] = $form_state['values']['other_country'];
    }
    if ($form_state['values']['other_state'] == '')
    {
      form_set_error('other_state', t('Enter state name'));
      // $form_state['values']['country'] = $form_state['values']['other_country'];
    } //$form_state['values']['other_state'] == ''
    else
    {
      $form_state['values']['all_state'] = $form_state['values']['other_state'];
    }
    if ($form_state['values']['other_city'] == '')
    {
      form_set_error('other_city', t('Enter city name'));
      // $form_state['values']['country'] = $form_state['values']['other_country'];
    } //$form_state['values']['other_city'] == ''
    else
    {
      $form_state['values']['city'] = $form_state['values']['other_city'];
    }
  } //$form_state['values']['country'] == 'Others'
  else
  {
    if ($form_state['values']['country'] == '')
    {
      form_set_error('country', t('Select country name'));
      // $form_state['values']['country'] = $form_state['values']['other_country'];
    } //$form_state['values']['country'] == ''
    if ($form_state['values']['all_state'] == '')
    {
      form_set_error('all_state', t('Select state name'));
      // $form_state['values']['country'] = $form_state['values']['other_country'];
    } //$form_state['values']['all_state'] == ''
    if ($form_state['values']['city'] == '')
    {
      form_set_error('city', t('Select city name'));
      // $form_state['values']['country'] = $form_state['values']['other_country'];
    } //$form_state['values']['city'] == ''
  }
  return;
}
/***************** proposal edit from submit ********************/
function proposal_edit_form_submit($form, &$form_state)
{
  /* completion date to timestamp */
  //changed list($d, $m, $y) to list list($y, $m, $d) as output format of completion_date is yyyy-mm-dd
  list($y, $m, $d) = explode('-', $form_state['values']['completion_date']);
  $completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
  list($y, $m, $d) = explode('-', $form_state['values']['proposed_completion_date']);
  $proposed_completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
  $scilab_version = 'scilab ' . $form_state['values']['version'];
  $proposal_id = $form_state['values']['hidden_proposal_id'];
  $scilab_version = 'scilab ' . $form_state['values']['version'];
  $query = db_update('textbook_companion_proposal');
  $query->fields(array(
    'full_name' => $form_state['values']['full_name'],
    'mobile' => $form_state['values']['mobile'],
    'course' => $form_state['values']['course'],
    'branch' => $form_state['values']['branch'],
    'university' => $form_state['values']['university'],
    'country' => $form_state['values']['country'],
    'city' => $form_state['values']['city'],
    'pincode' => $form_state['values']['pincode'],
    'state' => $form_state['values']['all_state'],
    'faculty' => $form_state['values']['faculty'],
    'reviewer' => $form_state['values']['reviewer'],
    'completion_date' => $completion_date_timestamp,
    'operating_system' => $form_state['values']['operating_system'],
    'scilab_version' => $scilab_version,
    'proposed_completion_date' => $proposed_completion_date_timestamp,
  ));
  $query->condition('id', $proposal_id);
  $num_updated = $query->execute();
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->condition('pref_number', 1);
  $query->range(0, 1);
  $preference1_q = $query->execute();
  $preference1_data = $preference1_q->fetchObject();
  if ($preference1_data)
    $preference1_id = $preference1_data->id;
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->condition('pref_number', 2);
  $query->range(0, 1);
  $preference2_q = $query->execute();
  $preference2_data = $preference2_q->fetchObject();
  if ($preference2_data)
    $preference2_id = $preference2_data->id;
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_id);
  $query->condition('pref_number', 3);
  $query->range(0, 1);
  $preference3_q = $query->execute();
  $preference3_data = $preference3_q->fetchObject();
  if ($preference3_data)
    $preference3_id = $preference3_data->id;
  if ($preference1_data)
  {
    del_book_pdf($preference1_data->id);
    $query = db_update('textbook_companion_preference');
    $query->fields(array(
      'book' => $form_state['values']['book1'],
      'author' => $form_state['values']['author1'],
      'isbn' => $form_state['values']['isbn1'],
      'publisher' => $form_state['values']['publisher1'],
      'edition' => $form_state['values']['edition1'],
      'year' => $form_state['values']['year1'],
      'category' => $form_state['values']['book_category_1']
    ));
    $query->condition('id', $preference1_id);
    $num_updated = $query->execute();
    /**
    *Sending mail to user about updating proposal
    */
    $book_user = user_load($row->uid);
    $email_to = $book_user->mail;
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $params['proposal_edit']['proposal_id'] = $proposal_id;
    $params['proposal_edit']['user_id'] = $row->uid;
    $params['proposal_edit']['headers'] = array(
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc
    );
    if (!drupal_mail('textbook_companion', 'proposal_edit', $email_to, language_default(), $params, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');
    drupal_set_message('Book proposal Updated. User has been notified of the editing.', 'status');
  } //$preference1_data
  if ($preference2_data)
  {
    del_book_pdf($preference2_data->id);
    $query = db_update('textbook_companion_preference');
    $query->fields(array(
      'book' => $form_state['values']['book2'],
      'author' => $form_state['values']['author2'],
      'isbn' => $form_state['values']['isbn2'],
      'publisher' => $form_state['values']['publisher2'],
      'edition' => $form_state['values']['edition2'],
      'year' => $form_state['values']['year2'],
      'category' => $form_state['values']['book_category_2']
    ));
    $query->condition('id', $preference2_id);
    $num_updated = $query->execute();
    /**
    *Sending mail to user about updating proposal
    */
    $book_user = user_load($row->uid);
    $email_to = $book_user->mail;
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $params['proposal_edit']['proposal_id'] = $proposal_id;
    $params['proposal_edit']['user_id'] = $row->uid;
    $params['proposal_edit']['headers'] = array(
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc
    );
    if (!drupal_mail('textbook_companion', 'proposal_edit', $email_to, language_default(), $params, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');
    drupal_set_message('Book proposal Updated. User has been notified of the editing.', 'status');
  } //$preference2_data
  if ($preference3_data)
  {
    del_book_pdf($preference3_data->id);
    $query = db_update('textbook_companion_preference');
    $query->fields(array(
      'book' => $form_state['values']['book3'],
      'author' => $form_state['values']['author3'],
      'isbn' => $form_state['values']['isbn3'],
      'publisher' => $form_state['values']['publisher3'],
      'edition' => $form_state['values']['edition3'],
      'year' => $form_state['values']['year3'],
      'category' => $form_state['values']['book_category_3']
    ));
    $query->condition('id', $preference3_id);
    $num_updated = $query->execute();
    /**
    *Sending mail to user about updating proposal
    */
    $book_user = user_load($row->uid);
    $email_to = $book_user->mail;
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $params['proposal_edit']['proposal_id'] = $proposal_id;
    $params['proposal_edit']['user_id'] = $row->uid;
    $params['proposal_edit']['headers'] = array(
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc
    );
    if (!drupal_mail('textbook_companion', 'proposal_edit', $email_to, language_default(), $params, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');
    drupal_set_message('Book proposal Updated. User has been notified of the editing.', 'status');
  } //$preference3_data
  drupal_goto('manage_proposal/all');
  return;
}
/******************************************************************************/
/**************************** CATEGORY EDIT FORM ******************************/
/******************************************************************************/
function category_edit_form($form, &$form_state)
{
  /* get current proposal */
  $preference_id = arg(3);
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $preference_id);
  $preference_q = $query->execute();
  $preference_data = $preference_q->fetchObject();
  if (!$preference_data)
  {
    drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
    drupal_goto('manage_proposal/category');
    return;
  } //!$preference_data

  $form['book'] = array(
    '#type' => 'item',
    '#title' => t('Title of the book'),
    '#markup' => $preference_data->book
  );
  $form['author'] = array(
    '#type' => 'item',
    '#title' => t('Author Name'),
    '#markup' => $preference_data->author
  );
  $form['isbn'] = array(
    '#type' => 'item',
    '#title' => t('ISBN No'),
    '#markup' => $preference_data->isbn
  );
  $form['publisher'] = array(
    '#type' => 'item',
    '#title' => t('Publisher & Place'),
    '#markup' => $preference_data->publisher
  );
  $form['edition'] = array(
    '#type' => 'item',
    '#title' => t('Edition'),
    '#markup' => $preference_data->edition
  );
  $form['year'] = array(
    '#type' => 'item',
    '#title' => t('Year of pulication'),
    '#markup' => $preference_data->year
  );
  $form['category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => _tbc_list_of_category(),
    '#required' => TRUE,
    '#default_value' => $preference_data->category
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal/category')
  );
  return $form;
}
function category_edit_form_submit($form, &$form_state)
{
  /* get current proposal */
  $preference_id = (int) arg(3);
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $preference_id);
  $preference_data = db_fetch_object($preference_q);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $preference_id);
  $preference_q = $query->execute();
  $preference_data = $preference_q->fetchObject();
  if (!$preference_data)
  {
    drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
    drupal_goto('manage_proposal/category');
    return;
  } //!$preference_data
  /*db_query("UPDATE {textbook_companion_preference} SET category = %d WHERE id = %d", $form_state['values']['category'], $preference_data->id);*/
  $query = db_update('textbook_companion_preference');
  $query->fields(array(
    'category' => $form_state['values']['category']
  ));
  $query->condition('id', $preference_data->id);
  $num_updated = $query->execute();
  drupal_set_message(t('Book Category Updated'), 'status');
  drupal_goto('manage_proposal/category');
}
/****************************************************************/
/*                     Data entry forms                         */
/****************************************************************/
function _data_entry_proposal_all()
{
  /* get pending proposals to be approved */
  $proposal_rows = array();
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 ORDER BY book ASC");*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('approval_status', 1);
  $query->orderBy('book', 'ASC');
  $preference_q = $query->execute();
  $sno = 1;
  while ($preference_data = $preference_q->fetchObject())
  {
    $proposal_rows[] = array(
      $sno++,
      $preference_data->book,
      $preference_data->author,
      $preference_data->isbn,
      l('Edit', 'dataentry_edit/' . $preference_data->id)
    );
  } //$preference_data = $preference_q->fetchObject()
  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  } //!$proposal_rows
  $proposal_header = array(
    'SNO',
    'Title of the Book',
    'Author',
    'ISBN',
    ''
  );
  $output = theme('table', array(
    'header' => $proposal_header,
    'rows' => $proposal_rows
  ));
  return $output;
}
function dataentry_edit($id = NULL)
{
  if ($id)
  {
    return drupal_get_form('dataentry_edit_form', $id);
  } //$id
  else
  {
    return 'Access denied';
  }
}
function dataentry_edit_form($form, &$form_state, $id)
{
  global $user;
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $id);
  $preference_q = $query->execute();
  $preference_data = $preference_q->fetchObject();
  $form['id'] = array(
    '#type' => 'hidden',
    '#required' => TRUE,
    '#value' => $id
  );
  $form['book'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference_data->book
  );
  $form['author'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference_data->author
  );
  $form['isbn'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#attribute' => array(
      'readonly' => 'readonly'
    ),
    '#default_value' => $preference_data->isbn
  );
  $form['publisher'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference_data->publisher
  );
  $form['edition'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference_data->edition
  );
  $form['year'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference_data->year
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}
function dataentry_edit_form_submit($form, &$form_state)
{
  $query = db_update('textbook_companion_preference');
  $query->fields(array(
    'book' => $_POST['book'],
    'author' => $_POST['author'],
    'isbn' => $_POST['isbn'],
    'publisher' => $_POST['publisher'],
    'edition' => $_POST['edition'],
    'year' => $_POST['year']
  ));
  $query->condition('id', $_POST['id']);
  $num_updated = $query->execute();
  drupal_set_message('Book details updated successfully');
  drupal_goto('dataentry_book');
}
function _failed_all($preference_id = 0, $confirm = "")
{
  $page_content = "";
  if ($preference_id && $confirm == "yes")
  {
    $query = "
      SELECT *, pro.id as proposal_id FROM textbook_companion_proposal pro
      LEFT JOIN textbook_companion_preference pre ON pre.proposal_id = pro.id
      LEFT JOIN users usr ON usr.uid = pro.uid
      WHERE pre.id = {:preference_id}
    ";
    $result = db_query($query, array(
      ':preference_id' => $preference_id
    ));
    $row = $result->fetchObject();
    /* increment failed_reminder */
    $query = "
      UPDATE textbook_companion_proposal
      SET failed_reminder = failed_reminder + 1
      WHERE id = {:id}
    ";
    db_query($query, array(
      ':id' => $row->proposal_id
    ));
    /* sending mail */
    $to = $row->mail;
    $subject = "Failed to upload the TBC codes on time";
    $body = "
    <p>
      Dear {$row->full_name},<br><br>
      This is to inform you that you have failed to upload the TBC codes on time.<br>
      Please note that the time you have taken is way past the deadline as well.<br>
      Kindly upload the TBC codes on the interface within 5 days from now.<br>
      Failure to submit the same will result in disapproval of your work and cancellation of your internship.<br><br>
      Regards,<br>
      Scilab Team
    </p>
    ";
    $message = $body;
    //drupal_mail($message);
    //drupal_mail('', 'standard', $email_to, language_default(), $params, variable_get('textbook_companion_from_email', NULL), TRUE))
    $from = variable_get('textbook_companion_from_email', NULL);
    $bcc = variable_get('textbook_companion_emails_fail_rem_bcc', NULL);
    fail_rem_send_mail($from, $to, $bcc, $subject, $message);
    drupal_set_message("Reminder sent successfully.");
    drupal_goto("manage_proposal/failed");
  } //$preference_id && $confirm == "yes"
  else if ($preference_id)
  {
    $query = "
      SELECT * FROM textbook_companion_preference pre
      LEFT JOIN textbook_companion_proposal pro ON pro.id = pre.proposal_id
      WHERE pre.id = {:preference_id}
    ";
    $result = db_query($query, array(
      ':preference_id' => $preference_id
    ));
    /*$row = db_fetch_object($result);*/
    $row = $result->fetchObject();
    $page_content .= "Are you sure you want to notify?<br><br>";
    $page_content .= "Book: <b>{$row->book}</b><br>";
    $page_content .= "Author: <b>{$row->author}</b><br>";
    $page_content .= "Contributor: <b>{$row->full_name}</b><br>";
    $page_content .= "Expected Completion Date: <b>" . date("d-m-Y", $row->completion_date) . "</b><br><br>";
    $page_content .= l("Yes", "manage_proposal/failed/{$preference_id}/yes") . " | ";
    $page_content .= l("Cancel", "manage_proposal/failed");
  } //$preference_id
  else
  {
    $query = "
      SELECT * FROM textbook_companion_proposal pro
      LEFT JOIN textbook_companion_preference pre ON pre.proposal_id = pro.id
      LEFT JOIN users usr ON usr.uid = pro.uid
      WHERE pro.proposal_status = 1 AND pre.approval_status = 1 AND pro.completion_date < :completion_date
      ORDER BY failed_reminder

    ";
    $result = db_query($query, array(
      ':completion_date' => time()
    ));
    $headers = array(
      "Date of Submission",
      "Book",
      "Contributor Name",
      "Expected Completion Date",
      "Remainders",
      "Action"
    );
    $rows = array();
    while ($row = $result->fetchObject())
    {
      $item = array(
        date("d-m-Y", $row->creation_date),
        "{$row->book}<br><i>by</i> {$row->author}",
        $row->full_name,
        date("d-m-Y", $row->completion_date),
        $row->failed_reminder,
        l("Remind", "manage_proposal/failed/{$row->id}")
      );
      array_push($rows, $item);
    } //$row = $result->fetchObject()
    $page_content .= theme('table', array(
      'header' => $headers,
      'rows' => $rows
    ));
  }
  return $page_content;
}
/**
 * Simple wrapper function for drupal_mail() to avoid extraneous code.
 */
function fail_rem_send_mail($from, $to, $bcc, $subject, $message)
{
  $my_module = 'textbook_companion';
  $my_mail_token = microtime();
  $message = array(
    'id' => $my_module . '_' . $my_mail_token,
    'to' => $to,
    'subject' => $subject,
    'body' => array(
      $message
    ),
    'headers' => array(
      'From' => $from,
      'Sender' => $from,
      'Return-Path' => $from,
      'Bcc' => $bcc
    )
  );
  $system = drupal_mail_system($my_module, $my_mail_token);
  $message = $system->format($message);
  if ($system->mail($message))
  {
    return TRUE;
  } //$system->mail($message)
  else
  {
    return FALSE;
  }
}
