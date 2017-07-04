<?php
// $Id$
function list_chapters() {
  global $user;
  /************************ start approve book details ************************/
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('uid', $user->uid);
  $query->orderBy('id', 'DESC');
  $query->range(0, 1);
  $result = $query->execute();
  $proposal_data = $result->fetchObject();
  if (!$proposal_data) {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  } //!$proposal_data
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
  $preference_data = db_fetch_object($preference_q);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_data->id);
  $query->condition('approval_status', 1);
  $query->range(0, 1);
  $result = $query->execute();
  $preference_data = $result->fetchObject();
  if (!$preference_data) {
    drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
    drupal_goto('');
    return;
  } //!$preference_data
  // if ($proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4) {
  //   switch ($proposal_data->proposal_status) {
  //     case 0:
  //       drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
  //       drupal_goto('');
  //       return;
  //       break;
  //     case 2:
  //       drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
  //       drupal_goto('');
  //       return;
  //       break;
  //     case 3:
  //       drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
  //       drupal_goto('');
  //       return;
  //       break;
  //     default:
  //       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
  //       drupal_goto('');
  //       return;
  //       break;
  //   } //$proposal_data->proposal_status
  // } //$proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4
  /**
   *changed according to new_proposal_status and book_status
   */
  if ($proposal_data->new_proposal_status != 1) {
    switch ($proposal_data->new_proposal_status) {
      case 0:
        drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
        drupal_goto('');
        return;
        break;
      case 2:
        drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
        drupal_goto('');
        return;
        break;
      default:
        drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
        drupal_goto('');
        return;
        break;
    }
  } else {
    if ($preference_data->book_status != 0) {
      switch ($preference_data->book_status) {
        case 1:
          drupal_set_message(t('You book propsoal has been sent for review. We will get back to you soon.'), 'status');
          drupal_goto('');
          return;
          break;
        case 2:
          drupal_set_message(t('Your proposal has been approved. We will get back to you soon.'), 'status');
          drupal_goto('');
          return;
          break;
        case 3:
          drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
          drupal_goto('');
          return;
          break;
        case 4:
          drupal_set_message(t('You book propsoal has been dis-carded.'), 'error');
          drupal_goto('');
          return;
          break;
        default:
          drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
          drupal_goto('');
          return;
          break;
      }
    }
  }
  /************************ end approve book details **************************/
  if ($preference_data->submited_all_examples_code) {
    drupal_set_message('Book has sent for Review. You can not add or make changes to your codes.', 'warning');
    $return_html = '<br />';
    $return_html .= '<strong>Title of the Book:</strong><br />' . $preference_data->book . '<br /><br />';
    $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->full_name . '<br /><br />';
    $return_html .= 'Upload Example Code' . '<br />';
    /* get chapter list */
    $chapter_rows = array();
    /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_data->id);*/
    $query = db_select('textbook_companion_chapter');
    $query->fields('textbook_companion_chapter');
    $query->condition('preference_id', $preference_data->id);
    $query->orderBy('number', 'ASC');
    $chapter_q = $query->execute();
    while ($chapter_data = $chapter_q->fetchObject()) {
      /* get example list */
      /* $example_q = db_query("SELECT count(*) as example_count FROM {textbook_companion_example} WHERE chapter_id = %d", $chapter_data->id);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_example');
      $query->addExpression('count(*)', 'example_count');
      $query->condition('chapter_id', $chapter_data->id);
      $result = $query->execute();
      $example_data = $result->fetchObject();
      $chapter_rows[] = array(
        $chapter_data->number,
        $chapter_data->name . ' (' . 'Edit' . '|' . 'Delete' . ')',
        $example_data->example_count,
        'View'
      );
    } //$chapter_data = $chapter_q->fetchObject()
    /* check if there are any chapters */
    if (!$chapter_rows) {
      drupal_set_message(t('No uploads found.'), 'status');
      return $return_html;
    } //!$chapter_rows
    $chapter_header = array(
      'Chapter No.',
      'Title of the Chapter',
      'Uploaded Examples',
      'Actions'
    );
    $return_html .= theme('table', array(
      'header' => $chapter_header,
      'rows' => $chapter_rows
    )) . '<br/>';
    $return_html .= drupal_render($submited_all_example);
  } else {
    $return_html = '<br />';
    $return_html .= '<strong>Title of the Book:</strong><br />' . $preference_data->book . '<br /><br />';
    $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->full_name . '<br /><br />';
    $return_html .= l('Upload Example Code', 'textbook_companion/code/upload') . '<br />';
    /* get chapter list */
    $chapter_rows = array();
    /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_data->id);*/
    $query = db_select('textbook_companion_chapter');
    $query->fields('textbook_companion_chapter');
    $query->condition('preference_id', $preference_data->id);
    $query->orderBy('number', 'ASC');
    $chapter_q = $query->execute();
    while ($chapter_data = $chapter_q->fetchObject()) {
      /* get example list */
      /* $example_q = db_query("SELECT count(*) as example_count FROM {textbook_companion_example} WHERE chapter_id = %d", $chapter_data->id);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_example');
      $query->addExpression('count(*)', 'example_count');
      $query->condition('chapter_id', $chapter_data->id);
      $result = $query->execute();
      $example_data = $result->fetchObject();
      $chapter_rows[] = array(
        $chapter_data->number,
        $chapter_data->name . ' (' . l('Edit', 'textbook_companion/code/chapter/edit/' . $chapter_data->id) . '|' . l('Delete', 'textbook_companion/code/chapter/delete/' . $chapter_data->id, array(
          'attributes' => array(
            'onClick' => 'return confirm("Are you sure you want to delete the Chapter? Once you delete chapter, all the codes related to chapter will be deleted.")'
          )
        )) . ')',
        $example_data->example_count,
        l('View', 'textbook_companion/code/list_examples/' . $chapter_data->id)
      );
    } //$chapter_data = $chapter_q->fetchObject()
    /* check if there are any chapters */
    if (!$chapter_rows) {
      drupal_set_message(t('No uploads found.'), 'status');
      return $return_html;
    } //!$chapter_rows
    $chapter_header = array(
      'Chapter No.',
      'Title of the Chapter',
      'Uploaded Examples',
      'Actions'
    );
    $return_html .= theme('table', array(
      'header' => $chapter_header,
      'rows' => $chapter_rows
    )) . '<br/>';
    $submited_all_example = drupal_get_form("all_example_submitted_check_form", $preference_data->id);
    $return_html .= drupal_render($submited_all_example);
  }
  return $return_html;
}
function list_examples() {
  global $user;
  /************************ start approve book details ************************/
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('uid', $user->uid);
  $query->orderBy('id', 'DESC');
  $query->range(0, 1);
  $result = $query->execute();
  $proposal_data = $result->fetchObject();
  if (!$proposal_data) {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  } //!$proposal_data
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
  $preference_data = db_fetch_object($preference_q);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('proposal_id', $proposal_data->id);
  $query->condition('approval_status', 1);
  $query->range(0, 1);
  $result = $query->execute();
  $preference_data = $result->fetchObject();
  if (!$preference_data) {
    drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
    drupal_goto('');
    return;
  } //!$preference_data
  // if ($proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4) {
  //   switch ($proposal_data->proposal_status) {
  //     case 0:
  //       drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
  //       drupal_goto('');
  //       return;
  //       break;
  //     case 2:
  //       drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
  //       drupal_goto('');
  //       return;
  //       break;
  //     case 3:
  //       drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
  //       drupal_goto('');
  //       return;
  //       break;
  //     default:
  //       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
  //       drupal_goto('');
  //       return;
  //       break;
  //   } //$proposal_data->proposal_status
  // } //$proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4
  /**
   *changed according to new_proposal_status and book_status
   */
  if ($proposal_data->new_proposal_status != 1) {
    switch ($proposal_data->new_proposal_status) {
      case 0:
        drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
        drupal_goto('');
        return;
        break;
      case 2:
        drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
        drupal_goto('');
        return;
        break;
      default:
        drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
        drupal_goto('');
        return;
        break;
    }
  } else {
    if ($preference_data->book_status != 0) {
      switch ($preference_data->book_status) {
        case 1:
          drupal_set_message(t('You book propsoal has been sent for review. We will get back to you soon.'), 'status');
          drupal_goto('');
          return;
          break;
        case 2:
          drupal_set_message(t('Your proposal has been approved. We will get back to you soon.'), 'status');
          drupal_goto('');
          return;
          break;
        case 3:
          drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
          drupal_goto('');
          return;
          break;
        case 4:
          drupal_set_message(t('You book propsoal has been dis-carded.'), 'error');
          drupal_goto('');
          return;
          break;
        default:
          drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
          drupal_goto('');
          return;
          break;
      }
    }
  }
  /************************ end approve book details **************************/
  /* get chapter details */
  $chapter_id = arg(3);
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d LIMIT 1", $chapter_id, $preference_data->id);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $chapter_id);
  $query->condition('preference_id', $preference_data->id);
  $query->range(0, 1);
  $chapter_q = $query->execute();
  if ($chapter_data = $chapter_q->fetchObject()) {
    $return_html = '<br />';
    $return_html .= '<strong>Title of the Book:</strong><br />' . $preference_data->book . '<br /><br />';
    $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->full_name . '<br /><br />';
    $return_html .= '<strong>Chapter Number:</strong><br />' . $chapter_data->number . '<br /><br />';
    $return_html .= '<strong>Title of the Chapter:</strong><br />' . $chapter_data->name . '<br />';
  } //$chapter_data = $chapter_q->fetchObject()
  else {
    drupal_set_message(t('Invalid chapter.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  $return_html .= '<br />' . l('Back to Chapter List', 'textbook_companion/code');
  /* get example list */
  $example_rows = array();
  /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d ORDER BY
  CAST(SUBSTRING_INDEX(number, '.', 1) AS BINARY) ASC,
  CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', 2), '.', -1) AS UNSIGNED) ASC,
  CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', -1), '.', 1) AS UNSIGNED) ASC", $chapter_id);*/
  $query = db_select('textbook_companion_example');
  $query->fields('textbook_companion_example');
  $query->condition('chapter_id', $chapter_id);
  $example_q = $query->execute();
  while ($example_data = $example_q->fetchObject()) {
    /* approval status */
    $approval_status = '';
    switch ($example_data->approval_status) {
      case 0:
        $approval_status = 'Pending';
        break;
      case 1:
        $approval_status = 'Approved';
        break;
      case 2:
        $approval_status = 'Rejected';
        break;
    } //$example_data->approval_status
    /* example files */
    $example_files = '';
    /*$example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d ORDER BY filetype", $example_data->id);*/
    $query = db_select('textbook_companion_example_files');
    $query->fields('textbook_companion_example_files');
    $query->condition('example_id', $example_data->id);
    $query->orderBy('filetype', 'ASC');
    $example_files_q = $query->execute();
    while ($example_files_data = $example_files_q->fetchObject()) {
      $file_type = '';
      switch ($example_files_data->filetype) {
        case 'S':
          $file_type = 'Main or Source';
          break;
        case 'R':
          $file_type = 'Result';
          break;
        case 'X':
          $file_type = 'xcos';
          break;
        default:
      } //$example_files_data->filetype
      $example_files .= l($example_files_data->filename, 'download/file/' . $example_files_data->id) . ' (' . $file_type . ')<br />';
    } //$example_files_data = $example_files_q->fetchObject()
    /* dependency files */
    $dependency_files = '';
    $dependency_files_q = db_query("SELECT dependency.id as dependency_id, dependency.filename as dependency_filename
      FROM {textbook_companion_example_dependency} example_dependency JOIN {textbook_companion_dependency_files} dependency
      ON example_dependency.dependency_id = dependency.id
      WHERE example_dependency.example_id = :example_id", array(
      ':example_id' => $example_data->id
    ));
    /*$query = db_select('textbook_companion_example_dependency', 'example_dependency');
    $query->fields('dependency', array('id', 'filename'));
    $query->innerJoin('textbook_companion_dependency_files', 'dependency', 'example_dependency.dependency_id = dependency.id');
    $query->condition('example_dependency.example_id', $example_data->id);
    $dependency_files_q = $query->execute();*/
    while ($dependency_files_data = $dependency_files_q->fetchObject()) {
      $dependency_files .= l($dependency_files_data->dependency_filename, 'download/dependency/' . $dependency_files_data->dependency_id) . ' (Dependency)<br />';
    } //$dependency_files_data = $dependency_files_q->fetchObject()
    $example_files .= $dependency_files;
    if ($example_data->approval_status == 0) {
      $example_rows[] = array(
        'data' => array(
          $example_data->number,
          $example_data->caption,
          $approval_status,
          $example_files,
          l('Edit', 'textbook_companion/code/edit/' . $example_data->id)
          /*l('Edit', 'textbook_companion/code/edit/' . $example_data->id) . ' | ' . l('Delete', 'textbook_companion/code/delete/' . $example_data->id, array(
          'attributes' => array(
          'onClick' => 'return confirm("Are you sure you want to delete the example?")'
          )
          ))*/
        ),
        'valign' => 'top'
      );
    } //$example_data->approval_status == 0
    else {
      $example_rows[] = array(
        'data' => array(
          $example_data->number,
          $example_data->caption,
          $approval_status,
          $example_files,
          l('Download', 'download/example/' . $example_data->id)
        ),
        'valign' => 'top'
      );
    }
  } //$example_data = $example_q->fetchObject()
  $example_header = array(
    'Example No.',
    'Caption',
    'Status',
    'Files',
    'Action'
  );
  $return_html .= theme('table', array(
    'header' => $example_header,
    'rows' => $example_rows
  ));
  return $return_html;
}
/*Form to confirm book completion*/
function all_example_submitted_check_form($form, &$form_state, $preference_id) {
  $form = array();
  $form['all_example_submitted'] = array(
    '#type' => 'checkbox',
    '#title' => t('I have submitted codes for all the examples'),
    '#description' => 'Once you have submited this option you are not able to upload more examples.',
    '#required' => TRUE
  );
  $form['preference_id'] = array(
    '#type' => 'hidden',
    '#value' => $preference_id
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}
/*Validation of all_example_submitted_check_form*/
function all_example_submitted_check_form_validate($form, &$form_state) {
  /*check for all_example_submitted = 1*/
  if ($form_state['values']['all_example_submitted'] != 1) {
    form_set_error('all_example_submitted', t('Please check the field if you are intrested to submit the all uploaded examples for review!'));
  }
  /*check for empty book*/
  if (!check_book($form_state['values']['preference_id'])) {
    form_set_error('all_example_submitted', t('You have not uploaded single code'));
  }
  return;
}
/**
 *Run bash script to check for errors in code
 *If error occurs send mail to user and keep submited_all_examples_code = 0
 *If no error found lock the book, submited_all_examples_code = 1, and mail to user, cc, bcc
 */
function all_example_submitted_check_form_submit($form, &$form_state) {
  global $user;
  if ($form_state['values']['all_example_submitted'] == 1) {
    db_query('UPDATE {textbook_companion_preference} SET submited_all_examples_code = 0  WHERE id = :preference_id', array(
      ':preference_id' => $form_state['values']['preference_id']
    ));
    /**
     *Executing bash script to check errors in uploaded examples
     *If errors are there, send mail to user and halt locking procedure
     */
#    $root_path = textbook_companion_path();
#    $book_id = $form_state['values']['preference_id'];
#    //$book_id = 3839;
#    $book_path = $root_path . $book_id;
#    //var_dump($root_path."auto_check/");die;
#    chdir($root_path . "auto_check");
#    exec("chmod u+x ./auto.sh");
#    $sh_command = "./auto.sh" . " " . $book_path . " " . $book_id;
#    //exec("chmod -R 777"." ".$book_path);
#    $return = shell_exec($sh_command);
#    var_dump($return);
#    die;
    $root_path = textbook_companion_path();
    $book_id = $form_state['values']['preference_id'];
    $book_path = $root_path . $book_id;
    $auto_check_path = $root_path.'auto_check/';
    $scihome = $auto_check_path.'scilab/';
    $sci_path = $scihome.'bin/scilab-cli';
    $sci_graph_path = $auto_check_path . $book_id . '/tbc_graphs';
    if(file_exists($sci_path)){
#     drupal_set_message('Code will check'
      $module_path = drupal_get_path("module", "textbook_companion");
      copy($module_path.'/bash/auto.sh', $auto_check_path.'auto.sh');
      chdir($auto_check_path);
      exec('chmod u+x ./auto.sh');
      if(file_exists($auto_check_path.$book_id)){
        exec('rm -rf '.$book_id);  
      }
      $sh_cmd = "export SCIHOME='".$scihome."';";
      $sh_cmd .= "export SCI_PATH='".$sci_path."';";
      $sh_cmd .= "export SCI_GRAPH_PATH='".$sci_graph_path."';";
      $sh_cmd .= "./auto.sh ".$book_path." ".$book_id.";";
      $bash_output = shell_exec($sh_cmd);
#      var_dump($bash_output);
#      die;
    }
    /**
     *Sending mail to user about Submission Locking
     */
    $query = db_select('textbook_companion_proposal');
    $query->fields('textbook_companion_proposal');
    $query->condition('id', $form_state['values']['preference_id']);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    $book_user = user_load($proposal_data->uid);
    $email_to = $book_user->mail;
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $params['all_example_submitted']['preference_id'] = $form_state['values']['preference_id'];
    $params['all_example_submitted']['user_id'] = $propasal_data->uid;
    $params['all_example_submitted']['headers'] = array(
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc
    );
    if (!drupal_mail('textbook_companion', 'all_example_submitted', $email_to, language_default(), $params, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');
    drupal_set_message('Code Submission is locked. You can not upload or change code anymore. Mail is sent to you.', 'status');
  } //$form_state['values']['all_example_submitted'] == 1
  return;
}
/*function to check book Directory exists or not*/
function check_book($book_id) {
  $status = FALSE;
  $root_path = textbook_companion_path();
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $book_id);
  $preference_data = db_fetch_object($preference_q);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $book_id);
  $preference_q = $query->execute();
  $preference_data = $preference_q->fetchObject();
  if (!$preference_data) {
    drupal_set_message(t('Invalid book.'), 'error');
    return FALSE;
  } //!$preference_data
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d", $preference_data->id);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('preference_id', $preference_data->id);
  $chapter_q = $query->execute();
  while ($chapter_data = $chapter_q->fetchObject())
    $status = ($status || check_chapter($chapter_data->id));
  return $status;
}
/*function to check wheather chapter Directory exists or not*/
function check_chapter($chapter_id) {
  $status = FALSE;
  $root_path = textbook_companion_path();
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $chapter_id);
  $chapter_data = db_fetch_object($chapter_q);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $chapter_id);
  $chapter_q = $query->execute();
  $chapter_data = $chapter_q->fetchObject();
  if (!$chapter_data) {
    drupal_set_message(t('Invalid chapter.'), 'error');
    return FALSE;
  } //!$chapter_data
  /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d", $chapter_id);*/
  $query = db_select('textbook_companion_example');
  $query->fields('textbook_companion_example');
  $query->condition('chapter_id', $chapter_id);
  $example_q = $query->execute();
  while ($example_data = $example_q->fetchObject())
    $status = ($status || check_example($example_data->id));
  return $status;
}
/*function to check wheather example contains example file or not*/
function check_example($example_id) {
  global $user;
  $root_path = textbook_companion_path();
  $status = FALSE;
  /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d", $example_id);
  $example_data = db_fetch_object($example_q);*/
  $query = db_select('textbook_companion_example');
  $query->fields('textbook_companion_example');
  $query->condition('id', $example_id);
  $example_q = $query->execute();
  $example_data = $example_q->fetchObject();
  if (!$example_data) {
    drupal_set_message(t('Invalid example.'), 'error');
    return FALSE;
  } //!$example_data
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $example_data->chapter_id);
  $chapter_q = $query->execute();
  $chapter_data = $chapter_q->fetchObject();
  if (!$chapter_data) {
    drupal_set_message(t('Invalid example chapter.'), 'error');
    return FALSE;
  } //!$chapter_data
  /*$examples_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);*/
  $query = db_select('textbook_companion_example_files');
  $query->fields('textbook_companion_example_files');
  $query->condition('example_id', $example_id);
  $examples_files_q = $query->execute();
  while ($examples_files_data = $examples_files_q->fetchObject())
    $status = ($status || file_exists($root_path . $examples_files_data->filepath));
  return $status;
}
