<?php
// You can find all functions regarding manage books
/*
textbook companion  table column "book_status" field values means
0 = ‘In progress’
1 = 'Under Review'
2 = ‘Approved’
3 = 'Completed'
4 = ‘Disabled’
*/
function _book_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  /*$pending_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 ORDER BY id DESC");*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('book_status', 0);
  $query->orderBy('id', 'DESC');
  $pending_q = $query->execute();
  while ($pending_data = $pending_q->fetchObject())
  {
    $query = db_select('textbook_companion_proposal');
    $query->fields('textbook_companion_proposal');
    $query->condition('id', $pending_data->proposal_id);
    $pending_data_p = $query->execute();
    $pending_data_p = $pending_data_p->fetchObject();
    $pending_rows[$pending_data->id] = array(
      "{$pending_data->book} <br> <em>by {$pending_data->author}</em>",
      l($pending_data_p->full_name, 'user/' . $pending_data_p->uid),
      l($pending_data->edition, 'user/' . $pending_data_p->uid),
      l($pending_data->year, 'user/' . $pending_data_p->uid),
      l('Approve', 'manage_books/approve/' . $pending_data->id) . ' | ' . l('Edit', 'manage_books/edit/' . $pending_data->id)
    );
  } //$pending_data = $pending_q->fetchObject()
  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no pending Books.'), 'status');
    return '';
  } //!$pending_rows
  $pending_header = array(
    'Book',
    'Contributor',
    'Edition',
    'Year',
    'Action'
  );
  $output = theme('table', array(
    'header' => $pending_header,
    'rows' => $pending_rows
  ));
  return $output;
}
function _book_all()
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
  /* get pending proposals to be approved */
  $book_rows = array();
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} ORDER BY id DESC");*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->orderBy('id', 'DESC');
  $book_q = $query->execute();
  while ($book_data = $book_q->fetchObject())
  {
    /* get preference */
    /*$preference_q = db_query("SELECT * FROM textbook_companion_preference WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data = db_fetch_object($preference_q);*/
    $book_status = '';
    switch ($book_data->book_status)
    {
      case 0:
        $book_status = 'In Progress';
        break;
      case 1:
        $book_status = 'Under Review';
        break;
      case 2:
        $book_status = 'Approved';
        break;
      case 3:
        $book_status = 'Completed';
        break;
      case 4:
        $book_status = 'Disabled';
        break;
      default:
        $book_status = 'Unknown';
        break;
    } //book_status
    $book_rows[] = array(
      "{$book_data->book} <br> <em>by {$book_data->author}</em>",
      l($book_data->publisher, 'user/' . $book_data->uid),
      l($book_data->edition, 'user/' . $book_data->uid),
      l($book_data->year, 'user/' . $book_data->uid),
      $book_status,
      l('Status', 'manage_books/status/' . $book_data->id) . ' | ' . l('Edit', 'manage_books/edit/' . $book_data->id) . _tbc_ext($book_status, $book_data->id)
    );
  } //$proposal_data = $proposal_q->fetchObject()
  /* check if there are any pending proposals */
  if (!$book_rows)
  {
    drupal_set_message(t('There are no Books available.'), 'status');
    return '';
  } //!$proposal_rows
  $book_header = array(
    'Book',
    'Publication',
    'Edition',
    'Year',
    'Status',
    'Action'
  );
  $output = theme('table', array(
    'header' => $book_header,
    'rows' => $book_rows
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
/************************** BOOK APPROVAL FORM ********************************/
/******************************************************************************/
function book_approval_form($form, &$form_state)
{
  global $user;
  /* get current book */
  $book_id = arg(2);
  /*$result = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 and id = %d", $proposal_id);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('book_status', 0);
  $query->condition('id', $book_id);
  $result = $query->execute();
  if ($result)
  {
    if ($book_data = $result->fetchObject())
    {
      /* everything ok */
    } //$row = $result->fetchObject()
    else
    {
      drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
      drupal_goto('manage_books');
      return;
    }
  } //$result
  else
  {
    drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
    drupal_goto('manage_books');
    return;
  }
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $book_data->proposal_id);
  $row = $query->execute();
  $row = $row->fetchObject();
  $book_status = '';
  switch ($book_data->book_status)
  {
    case 0:
      $book_status = 'In Progress';
      break;
    case 1:
      $book_status = 'Under Review';
      break;
    case 2:
      $book_status = 'Approved';
      break;
    case 3:
      $book_status = 'Completed';
      break;
    case 4:
      $book_status = 'Disabled';
      break;
    default:
      $book_status = 'Unknown';
      break;
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
  $form['preference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Details'),
  );
  $form['preference']['book'] = array(
    '#type' => 'item',
    '#title' => t('Title of the book'),
    '#markup' => $book_data->book
  );
  $form['preference']['author'] = array(
    '#type' => 'item',
    '#title' => t('Author Name'),
    '#markup' => $book_data->author
  );
  $form['preference']['isbn'] = array(
    '#type' => 'item',
    '#title' => t('ISBN No'),
    '#markup' => $book_data->isbn
  );
  $form['preference']['publisher'] = array(
    '#type' => 'item',
    '#title' => t('Publisher & Place'),
    '#markup' => $book_data->publisher
  );
  $form['preference']['edition'] = array(
    '#type' => 'item',
    '#title' => t('Edition'),
    '#markup' => $book_data->edition
  );
  $form['preference']['year'] = array(
    '#type' => 'item',
    '#title' => t('Year of pulication'),
    '#markup' => $book_data->year
  );
  $status = array(
    '0' => 'In progress',
    '1' => 'Under Review',
    '2' => 'Approved',
    '3' => 'Completed',
    '4' => 'Disabled',
  );
  $form['book_status_'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => $status,
    '#required' => TRUE,
    '#default_value' => $book_status
  );
  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $book_data->proposal_id
  );
  $form['book_id'] = array(
    '#type' => 'hidden',
    '#value' => $book_data->id
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
function book_approval_form_submit($form, &$form_state)
{
  $preference_id = $form_state['values']['book_id'];
  $query = db_update('textbook_companion_preference');
  $query->fields(array(
    'book_status' => $form_state['values']['book_status_'],
  ));
  $query->condition('id', $preference_id);
  $updated = $query->execute();
  /**
   *Email for updating book status
   */
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $updated->propasal_id);
  $result = $query->execute();
  $proposal_data = $result->fetchObject();
  $book_user = user_load($proposal_data->uid);
  $email_to = $book_user->mail;
  $from = variable_get('textbook_companion_from_email', '');
  $bcc = variable_get('textbook_companion_emails', '');
  $cc = variable_get('textbook_companion_cc_emails', '');
  $params['book_status_updated']['preference_id'] = $preference_id;
  $params['book_status_updated']['user_id'] = $proposal_data->uid;
  $params['book_status_updated']['headers'] = array(
    'From' => $from,
    'MIME-Version' => '1.0',
    'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    'Content-Transfer-Encoding' => '8Bit',
    'X-Mailer' => 'Drupal',
    'Cc' => $cc,
    'Bcc' => $bcc
  );
  if (!drupal_mail('textbook_companion', 'book_status_updated', $email_to, language_default(), $params, $from, TRUE))
    drupal_set_message('Error sending email message.', 'error');
  drupal_set_message('Book Status has been Changed. User has been notified.', 'status');
  drupal_goto('manage_books');
  return;
}
/******************************************************************************/
/*************************** BOOK STATUS FORM *****************************/
/******************************************************************************/
function book_status_form($form, &$form_state)
{
  global $user;
  /* get current book */
  $book_id = arg(2);
  /*$result = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 and id = %d", $proposal_id);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $book_id);
  $result = $query->execute();
  if ($result)
  {
    if ($book_data = $result->fetchObject())
    {
      /* everything ok */
    } //$row = $result->fetchObject()
    else
    {
      drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
      drupal_goto('manage_books');
      return;
    }
  } //$result
  else
  {
    drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
    drupal_goto('manage_books');
    return;
  }
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $book_data->proposal_id);
  $row = $query->execute();
  $row = $row->fetchObject();
  $book_status = '';
  switch ($book_data->book_status)
  {
    case 0:
      $book_status = 'In Progress';
      break;
    case 1:
      $book_status = 'Under Review';
      break;
    case 2:
      $book_status = 'Approved';
      break;
    case 3:
      $book_status = 'Completed';
      break;
    case 4:
      $book_status = 'Disabled';
      break;
    default:
      $book_status = 'Unknown';
      break;
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
  $form['preference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Details'),
  );
  $form['preference']['book'] = array(
    '#type' => 'item',
    '#title' => t('Title of the book'),
    '#markup' => $book_data->book
  );
  $form['preference']['author'] = array(
    '#type' => 'item',
    '#title' => t('Author Name'),
    '#markup' => $book_data->author
  );
  $form['preference']['isbn'] = array(
    '#type' => 'item',
    '#title' => t('ISBN No'),
    '#markup' => $book_data->isbn
  );
  $form['preference']['publisher'] = array(
    '#type' => 'item',
    '#title' => t('Publisher & Place'),
    '#markup' => $book_data->publisher
  );
  $form['preference']['edition'] = array(
    '#type' => 'item',
    '#title' => t('Edition'),
    '#markup' => $book_data->edition
  );
  $form['preference']['year'] = array(
    '#type' => 'item',
    '#title' => t('Year of pulication'),
    '#markup' => $book_data->year
  );
  $form['preference']['book_status_'] = array(
    '#type' => 'item',
    '#title' => t('Status'),
    '#markup' => $book_status
  );
  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $book_data->proposal_id
  );
  $form['book_id'] = array(
    '#type' => 'hidden',
    '#value' => $book_data->id
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal')
  );
  return $form;
}

/******************************************************************************/
/**************************** BOOK EDIT FORM ******************************/
/******************************************************************************/
function book_edit_form($form, &$form_state, $nonaicte_book)
{
  global $user;
  /* get current proposal */
  $book_id = arg(2);
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $book_id);
  $book_q = $query->execute();
  if ($book_q)
  {
    $book_data = $book_q->fetchObject();
    //Var_dump($proposal_data);die;
    if (!$book_data)
    {
      drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
      drupal_goto('manage_books');
      return;
    } //!$proposal_data
  } //$proposal_q
  else
  {
    drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
    drupal_goto('manage_books');
    return;
  }
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $book_data->proposal_id);
  $proposal_data = $query->execute();
  $proposal_data = $proposal_data->fetchObject();
  $user_data = user_load($proposal_data->uid);
  $form['full_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full Name'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->full_name
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

  $form['proposed_completion_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Proposed Date of Completion'),
    '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => date('d-m-Y', $proposal_data->proposed_completion_date)
  );
  */
  /**
  *Added date_popup for date selection
  */
  $form['completion_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Expected Date of Completion'),
    '#date_increment' => 15,
    //'#date_year_range' => '2017:+3',
    '#default_value' => date('Y-m-d', $proposal_data->completion_date),
    '#date_format' => 'd-m-Y',
    '#attributes' => array(
      'autocomplete' => 'off',
      'readonly' => 'readonly'
    ),
    "#required" => TRUE
  );
  $form['proposed_completion_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Propsed Completion Date'),
    '#date_increment' => 15,
    //'#date_year_range' => '2017:+3',
    '#default_value' => date('Y-m-d', $proposal_data->proposed_completion_date),
    '#date_format' => 'd-m-Y',
    '#attributes' => array(
        'autocomplete' => 'off',
       'readonly' => 'readonly'
    ),
    "#required" => TRUE
  );
  $form['preference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Details'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['preference']['book'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $book_data->book
  );
  $form['preference']['author'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $book_data->author
  );
  $form['preference']['isbn'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $book_data->isbn
  );
  $form['preference']['publisher'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $book_data->publisher
  );
  $form['preference']['edition'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $book_data->edition
  );
  $form['preference']['year'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $book_data->year
  );
  $form['preference ']['book_category_'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => _tbc_list_of_category(),
    '#required' => TRUE,
    '#default_value' => $book_data->category
  );
  /* hidden fields */
  $form['hidden_book_id'] = array(
    '#type' => 'hidden',
    '#value' => $book_data->id
  );
  $form['hidden_proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $book_data->proposal_id
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_books')
  );
  return $form;
}
function book_edit_form_validate($form, &$form_state)
{
  /* date of completion */
  // if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
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
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition']))
    form_set_error('edition1', t('Invalid edition for Book'));
  /* year of publication */
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year']))
    form_set_error('year', t('Invalid year of pulication for Book'));
  /* year of publication */
  $cur_year = date('Y');
  if ((int) $form_state['values']['year'] > $cur_year)
    form_set_error('year1', t('Year of pulication should be not in the future for Book'));
  /* isbn */
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn']))
    form_set_error('isbn1', t('Invalid ISBN for Book'));
  return;
}
/***************** book edit from submit ********************/
function book_edit_form_submit($form, &$form_state)
{
  /* completion date to timestamp */
  list($y, $m, $d) = explode('-', $form_state['values']['completion_date']);
  $completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
  list($y, $m, $d) = explode('-', $form_state['values']['proposed_completion_date']);
  $proposed_completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
  $proposal_id = $form_state['values']['hidden_proposal_id'];
  $query = db_update('textbook_companion_proposal');
  $query->fields(array(
    'full_name' => $form_state['values']['full_name'],
    'reviewer' => $form_state['values']['reviewer'],
    'completion_date' => $completion_date_timestamp,
    'proposed_completion_date' => $proposed_completion_date_timestamp,
  ));
  $query->condition('id', $proposal_id);
  $num_updated = $query->execute();
  $book_id = $form_state['values']['hidden_book_id'];
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $book_id);
  $query->range(0, 1);
  $preference_q = $query->execute();
  $preference_data = $preference_q->fetchObject();
  $preference_id = $preference_data->id;
  if ($preference_data)
  {
    del_book_pdf($preference_data->id);
    $query = db_update('textbook_companion_preference');
    $query->fields(array(
      'book' => $form_state['values']['book'],
      'author' => $form_state['values']['author'],
      'isbn' => $form_state['values']['isbn'],
      'publisher' => $form_state['values']['publisher'],
      'edition' => $form_state['values']['edition'],
      'year' => $form_state['values']['year'],
      'category' => $form_state['values']['book_category_']
    ));
    $query->condition('id', $preference1_id);
    $num_updated = $query->execute();
  } //$preference1_data
  /**
  *Sending mail to user about editing book details
  */
  $book_user = user_load($row->uid);
  $email_to = $book_user->mail;
  $from = variable_get('textbook_companion_from_email', '');
  $bcc = variable_get('textbook_companion_emails', '');
  $cc = variable_get('textbook_companion_cc_emails', '');
  $params['book_edit']['proposal_id'] = $proposal_id;
  $params['book_edit']['preference_id'] = $preference_id;
  $params['book_edit']['user_id'] = $row->uid;
  $params['book_edit']['headers'] = array(
    'From' => $from,
    'MIME-Version' => '1.0',
    'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    'Content-Transfer-Encoding' => '8Bit',
    'X-Mailer' => 'Drupal',
    'Cc' => $cc,
    'Bcc' => $bcc
  );
  if (!drupal_mail('textbook_companion', 'book_edit', $email_to, language_default(), $params, $from, TRUE))
    drupal_set_message('Error sending email message.', 'error');
  drupal_set_message('Book Details Updated. User has been notified of the updating.', 'status');
  drupal_goto('manage_books/');
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
