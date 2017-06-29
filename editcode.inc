<?php
// $Id$
/******************************************************************************/
/***************************** EDIT EXAMPLE ***********************************/
/******************************************************************************/
function upload_examples_edit() {
  return drupal_get_form('upload_examples_edit_form');
}
function upload_examples_edit_form($form, &$form_state) {
  $form = array();
  global $user;
  if ($form_state['values']['example_id'])
    $example_id = $form_state['values']['example_id'];
  else
    $example_id = arg(3);
  $form['example_id'] = array(
    '#type' => 'hidden',
    '#value' => $example_id
  );
  /* get example details */
  /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
  $example_data = db_fetch_object($example_q);*/
  $query = db_select('textbook_companion_example');
  $query->fields('textbook_companion_example');
  $query->condition('id', $example_id);
  $query->range(0, 1);
  $example_q = $query->execute();
  $example_data = $example_q->fetchObject();
  if (!$example_data) {
    drupal_set_message(t("Invalid example selected."), 'error');
    drupal_goto('');
    return;
  } //!$example_q
  if ($example_data->approval_status != 0) {
    drupal_set_message(t("You cannot edit an example after it has been approved or dis-approved. Please contact site administrator if you want to edit this example."), 'error');
    drupal_goto('');
    return;
  } //$example_data->approval_status != 0
  /* get examples files */
  $source_file = "";
  $source_id = 0;
  $result1_file = "";
  $result1_id = 0;
  $result2_file = "";
  $result2_id = 0;
  $xcos1_file = "";
  $xcos1_id = 0;
  $xcos2_file = "";
  $xcos2_id = 0;
  /*$example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);*/
  $query = db_select('textbook_companion_example_files');
  $query->fields('textbook_companion_example_files');
  $query->condition('example_id', $example_id);
  $example_files_q = $query->execute();
  while ($example_files_data = $example_files_q->fetchObject()) {
    if ($example_files_data->filetype == "S") {
      $source_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
      $source_file_id = $example_files_data->id;
    } //$example_files_data->filetype == "S"
    if ($example_files_data->filetype == "R") {
      if (strlen($result1_file) == 0) {
        $result1_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
        $result1_file_id = $example_files_data->id;
      } //strlen($result1_file) == 0
      else {
        $result2_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
        $result2_file_id = $example_files_data->id;
      }
    } //$example_files_data->filetype == "R"
    if ($example_files_data->filetype == "X") {
      if (strlen($xcos1_file) <= 0) {
        $xcos1_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
        $xcos1_file_id = $example_files_data->id;
      } //strlen($xcos1_file) <= 0
      else {
        $xcos2_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
        $xcos2_file_id = $example_files_data->id;
      }
    } //$example_files_data->filetype == "X"
  } //$example_files_data = $example_files_q->fetchObject()
  /* get chapter details */
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $example_data->chapter_id);
  $result = $query->execute();
  $chapter_data = $result->fetchObject();
  if (!$chapter_data) {
    drupal_set_message(t("Invalid chapter selected."), 'error');
    drupal_goto('');
    return;
  } //!$chapter_data
  /* get preference details */
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $chapter_data->preference_id);
  $preference_data = db_fetch_object($preference_q);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $chapter_data->preference_id);
  $result = $query->execute();
  $preference_data = $result->fetchObject();
  if (!$preference_data) {
    drupal_set_message(t("Invalid book selected."), 'error');
    drupal_goto('');
    return;
  } //!$preference_data
  if ($preference_data->approval_status != 1) {
    drupal_set_message(t("Cannot edit example. Either the book proposal has not been approved or it has been rejected."), 'error');
    drupal_goto('');
    return;
  } //$preference_data->approval_status != 1
  /* get proposal details */
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
  $proposal_data = db_fetch_object($proposal_q);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $preference_data->proposal_id);
  //$query->condition('proposal_status', 3);
  $result = $query->execute();
  $proposal_data = $result->fetchObject();
  if (!$proposal_data) {
    //drupal_set_message(t("Invalid proposal selected."), 'error');
    //drupal_goto('');
    die("Invalid proposal selected." . $proposal_data . "Invalid proposal selected.");
    return;
  } //!$proposal_data
  if ($proposal_data->uid != $user->uid) {
    drupal_set_message(t("You do not have permissions to edit this example."), 'error');
    drupal_goto('');
    return;
  } //$proposal_data->uid != $user->uid
  $user_data = user_load($proposal_data->uid);
  /* add javascript for automatic book title, check if example uploaded, dependency selection effects */

  $chapter_name_js = " (function ($) {
      $(document).ready(function() {
    $('#edit-existing-depfile-dep-book-title').change(function() {
      var dep_selected = '';
      /* showing and hiding relevant files *
    $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-book-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
     $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
  });
})(jQuery);";
  drupal_add_js($chapter_name_js, 'inline', 'header');
  $form['#redirect'] = 'textbook_companion/code';
  $form['#attributes'] = array(
    'enctype' => "multipart/form-data"
  );
  $form['book_details']['book'] = array(
    '#type' => 'item',
    '#markup' => $preference_data->book,
    '#title' => t('Title of the Book')
  );
  $form['contributor_name'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->full_name,
    '#title' => t('Contributor Name')
  );
  $form['number'] = array(
    '#type' => 'item',
    '#title' => t('Chapter No'),
    '#markup' => $chapter_data->number
  );
  $form['name'] = array(
    '#type' => 'item',
    '#title' => t('Title of the Chapter'),
    '#markup' => $chapter_data->name
  );
  $form['example_number'] = array(
    '#type' => 'item',
    '#title' => t('Example No'),
    '#markup' => $example_data->number
  );
  $form['example_caption'] = array(
    '#type' => 'textfield',
    '#title' => t('Caption'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $example_data->caption
  );
  $form['example_warning'] = array(
    '#type' => 'item',
    '#title' => t('You should upload all the files (main or source files, result files, executable file if any)'),
    '#prefix' => '<div style="color:red">',
    '#suffix' => '</div>'
  );
  $form['sourcefile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Main or Source Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );
  if ($source_file) {
    $form['sourcefile']['cur_source'] = array(
      '#type' => 'item',
      '#title' => t('Existing Main or Source File'),
      '#markup' => $source_file
    );
    $form['sourcefile']['source_file_action_note'] = array(
      '#type' => 'item',
      '#title' => t('Note:'),
      '#markup' => t("<Strong style='color:red' >Use Replace option to upload your corrected code. Do not delete the example if your book is under review, you can always edit and upload it by the Replace option.</strong>")
    );
    $form['sourcefile']['source_file_action'] = array(
      '#type' => 'radios',
      '#options' => array(
        '1' => "Replace Existing Main or Source File<br>",
        '2' => "Delete Existing Main or Source File"
      ),
      '#title' => t('Select Action'),
      '#required' => TRUE
    );
    $form['sourcefile']['sourcefile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload New Main or Source File'),
      '#size' => 48,
      '#description' => t("<Strong style='color:red' >Upload new Main or Source file above, if you want to replace the existing file. Leave blank if you want to keep using the existing file.</strong> <br />") . t('Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', ''),
      '#states' => array(
        'visible' => array(
          ':input[name="source_file_action"]' => array(
            'value' => '1'
          )
        )
      )
    );
    /*
    $form['sourcefile']['cur_source_checkbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete Existing Main or Source File'),
    '#description' => 'Check to delete the existing Main or Source file.',
    '#states' => array(
    'visible' => array(
    ':input[name="source_file_action"]' => array('value' => '2'),
    ),
    ),
    );
    */
    $form['sourcefile']['cur_source_file_id'] = array(
      '#type' => 'hidden',
      '#value' => $source_file_id
    );
  } //$source_file
  else {
    $form['sourcefile']['sourcefile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload New Main or Source File'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', '')
    );
  }
  $form['dep_files'] = array(
    '#type' => 'item',
    '#title' => t('Dependency Files')
  );
  /***********************START OF EXISTING DEPENDENCIES***********************/
  $form['existing_depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use Already Existing Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="existing-depfile-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE
  );
  $form['existing_depfile']['dep_chapter_example_files'] = array(
    '#type' => 'select',
    '#title' => t('Completed Books'),
    //  #empty_value' => 0,
    '#empty_option' => "-Select-",
    '#options' => completed_book(),
    '#ajax' => array(
      'callback' => 'ajax_checkboxes_callback',
      'wrapper' => 'checkboxes-div'
    ),
    '#multiple' => FALSE
  );
  $form['existing_depfile']['checkboxes_fieldset'] = array(
    '#prefix' => '<div id = "checkboxes-div">',
    '#suffix' => '</div>',
    '#type' => 'checkboxes',
    '#title' => 'Dependency files for selected book.'
  );
  if (isset($form_state['values']['existing_depfile']['dep_chapter_example_files'])) {
    $val = $form_state['values']['existing_depfile']['dep_chapter_example_files'];
    if ($val != 0) {
      $query = db_query('SELECT * FROM {textbook_companion_dependency_files} WHERE preference_id = (:preference_id)', array(
        ':preference_id' => $val
      ));
      $option_list = array();
      $row = $query->rowCount();
      if ($row == 0) {
        $form['existing_depfile']['checkboxes_fieldset']['empty'] = array(
          '#type' => 'item',
          '#markup' => 'This lab has no dependency files.'
        );
      } else {
        $dependency_options_list = array();
        while ($curr_val = $query->fetchObject()) {
          // $form['checkboxes_fieldset']["checkbox$cnt"] = array(
          //   '#type' => 'checkbox',
          //   '#title' => $curr_val->filename,
          //   '#return_value' => $curr_val->id,
          // );
          $dependency_options_list[$curr_val->id] = $curr_val->filename;
        }
      }
      $form['existing_depfile']['checkboxes_fieldset']['#options'] = $dependency_options_list;
    }
  }
  /**************************END OF EXISTIING DEPENDENCIES**888****************/
  $form['result'] = array(
    '#type' => 'fieldset',
    '#title' => t('Result Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );
  if ($result1_file) {
    $form['result']['cur_result1'] = array(
      '#type' => 'item',
      '#title' => t('Existing Result File 1'),
      '#markup' => $result1_file
    );
    $form['result']['cur_result1_checkbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete Existing Result File 1'),
      '#description' => 'Check to delete the existing Result file.'
    );
    $form['result']['result1'] = array(
      '#type' => 'file',
      '#title' => t('Upload New Result File 1'),
      '#size' => 48,
      '#description' => t("Upload new Result file above if you want to replace the existing file, leave blank if you want to keep using the existing file. <br />") . t('Allowed file extensions : ') . variable_get('textbook_companion_result_extensions', '')
    );
    $form['result']['cur_result1_file_id'] = array(
      '#type' => 'hidden',
      '#value' => $result1_file_id
    );
  } //$result1_file
  else {
    $form['result']['result1'] = array(
      '#type' => 'file',
      '#title' => t('Upload New Result File 1'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_result_extensions', '')
    );
  }
  $form['result']['br'] = array(
    '#type' => 'item',
    '#markup' => "<br />"
  );
  if ($result2_file) {
    $form['result']['cur_result2'] = array(
      '#type' => 'item',
      '#title' => t('Existing Result File 2'),
      '#markup' => $result2_file
    );
    $form['result']['cur_result2_checkbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete Existing Result File 2'),
      '#description' => 'Check to delete the existing Result file.'
    );
    $form['result']['result2'] = array(
      '#type' => 'file',
      '#title' => t('Upload New Result file 2'),
      '#size' => 48,
      '#description' => t("Upload new Result file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('Allowed file extensions : ') . variable_get('textbook_companion_result_extensions', '')
    );
    $form['result']['cur_result2_file_id'] = array(
      '#type' => 'hidden',
      '#value' => $result2_file_id
    );
  } //$result2_file
  else {
    $form['result']['result2'] = array(
      '#type' => 'file',
      '#title' => t('Upload New Result file 2'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_result_extensions', '')
    );
  }
  $form['xcos'] = array(
    '#type' => 'fieldset',
    '#title' => t('XCOS Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );
  if ($xcos1_file) {
    $form['xcos']['cur_xcos1'] = array(
      '#type' => 'item',
      '#title' => t('Existing xcos File 1'),
      '#markup' => $xcos1_file
    );
    $form['xcos']['cur_xcos1_checkbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete Existing xcos File 1'),
      '#description' => 'Check to delete the existing xcos file.'
    );
    $form['xcos']['xcos1'] = array(
      '#type' => 'file',
      '#title' => t('Upload New xcos file 1'),
      '#size' => 48,
      '#description' => t("Upload new xcos file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('Allowed file extensions : ') . variable_get('textbook_companion_xcos_extensions', '')
    );
    $form['sourcefile']['cur_xcos1_file_id'] = array(
      '#type' => 'hidden',
      '#value' => $xcos1_file_id
    );
  } //$xcos1_file
  else {
    $form['xcos']['xcos1'] = array(
      '#type' => 'file',
      '#title' => t('Upload New xcos file 1'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_xcos_extensions', '')
    );
  }
  $form['xcos']['br'] = array(
    '#type' => 'item',
    '#markup' => "<br />"
  );
  if ($xcos2_file) {
    $form['xcos']['cur_xcos2'] = array(
      '#type' => 'item',
      '#title' => t('Existing xcos File 2'),
      '#markup' => $xcos2_file
    );
    $form['xcos']['cur_xcos2_checkbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete Existing xcos File 2'),
      '#description' => 'Check to delete the existing xcos file.'
    );
    $form['xcos']['xcos2'] = array(
      '#type' => 'file',
      '#title' => t('Upload New xcos file 2'),
      '#size' => 48,
      '#description' => t("Upload new xcos file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('Allowed file extensions : ') . variable_get('textbook_companion_xcos_extensions', '')
    );
    $form['xcos']['cur_xcos2_file_id'] = array(
      '#type' => 'hidden',
      '#value' => $xcos2_file_id
    );
  } //$xcos2_file
  else {
    $form['xcos']['xcos2'] = array(
      '#type' => 'file',
      '#title' => t('Upload New xcos file 2'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_xcos_extensions', '')
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'textbook_companion/code')
  );
  return $form;
}
function upload_examples_edit_form_validate($form, &$form_state) {
  if (!check_name($form_state['values']['example_caption']))
    form_set_error('example_caption', t('Example Caption can contain only alphabets, numbers and spaces.'));
  if (isset($_FILES['files'])) {
    /* check for valid filename extensions */
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name) {
      if ($file_name) {
        /* checking file type */
        if (strstr($file_form_name, 'source'))
          $file_type = 'S';
        else if (strstr($file_form_name, 'result'))
          $file_type = 'R';
        else if (strstr($file_form_name, 'xcos'))
          $file_type = 'X';
        else
          $file_type = 'U';
        $allowed_extensions_str = '';
        switch ($file_type) {
          case 'S':
            $allowed_extensions_str = variable_get('textbook_companion_source_extensions', '');
            break;
          case 'R':
            $allowed_extensions_str = variable_get('textbook_companion_result_extensions', '');
            break;
          case 'X':
            $allowed_extensions_str = variable_get('textbook_companion_xcos_extensions', '');
            break;
        } //$file_type
        $allowed_extensions = explode(',', $allowed_extensions_str);
        $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
        if (!in_array($temp_extension, $allowed_extensions))
          form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
        if ($_FILES['files']['size'][$file_form_name] <= 0)
          form_set_error($file_form_name, t('File size cannot be zero.'));
        /* check if valid file name */
        if (!textbook_companion_check_valid_filename($_FILES['files']['name'][$file_form_name]))
          form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
      } //$file_name
    } //$_FILES['files']['name'] as $file_form_name => $file_name
  } //isset($_FILES['files'])
  /* add javascript again for automatic book title, check if example uploaded, dependency selection effects */
  $chapter_name_js = " $(document).ready(function() {
    $('#edit-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/chapter_title/' + $('#edit-number').val() + '/' + " . $row->pre_id . ", function(data) {
        $('#edit-name').val(data);
      });
    });
    $('#edit-example-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/example_exists/' + $('#edit-number').val() + '/' + $('#edit-example-number').val(), function(data) {
        if (data) {
          alert(data);
        }
      });
    });
    $('#edit-existing-depfile-dep-book-title').change(function() {
      var dep_selected = '';
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-book-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
  });";
  drupal_add_js($chapter_name_js, 'inline', 'header');
}
function upload_examples_edit_form_submit($form, &$form_state) {
  global $user;
  $example_id = arg(3);
  /* get example details */
  /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
  $example_data = db_fetch_object($example_q);*/
  $query = db_select('textbook_companion_example');
  $query->fields('textbook_companion_example');
  $query->condition('id', $example_id);
  $query->range(0, 1);
  $example_q = $query->execute();
  $example_data = $example_q->fetchObject();
  if (!$example_q) {
    drupal_set_message(t("Invalid example selected."), 'error');
    drupal_goto('');
    return;
  } //!$example_q
  if ($example_data->approval_status != 0) {
    drupal_set_message(t("You cannot edit an example after it has been approved or dis-approved. Please contact site administrator if you want to edit this example."), 'error');
    drupal_goto('');
    return;
  } //$example_data->approval_status != 0
  /* get chapter details */
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $example_data->chapter_id);
  $result = $query->execute();
  $chapter_data = $result->fetchObject();
  if (!$chapter_data) {
    drupal_set_message(t("Invalid chapter selected."), 'error');
    drupal_goto('');
    return;
  } //!$chapter_data
  /* get preference details */
  /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $chapter_data->preference_id);
  $preference_data = db_fetch_object($preference_q);*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $query->condition('id', $chapter_data->preference_id);
  $result = $query->execute();
  $preference_data = $result->fetchObject();
  if (!$preference_data) {
    drupal_set_message(t("Invalid book selected."), 'error');
    drupal_goto('');
    return;
  } //!$preference_data
  if ($preference_data->approval_status != 1) {
    drupal_set_message(t("Cannot edit example. Either the book proposal has not been approved or it has been rejected."), 'error');
    drupal_goto('');
    return;
  } //$preference_data->approval_status != 1
  /* get proposal details */
  /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
  $proposal_data = db_fetch_object($proposal_q);*/
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('id', $preference_data->proposal_id);
  $result = $query->execute();
  $proposal_data = $result->fetchObject();
  if (!$proposal_data) {
    drupal_set_message(t("Invalid proposal selected."), 'error');
    drupal_goto('');
    return;
  } //!$proposal_data
  if ($proposal_data->uid != $user->uid) {
    drupal_set_message(t("You do not have permissions to edit this example."), 'error');
    drupal_goto('');
    return;
  } //$proposal_data->uid != $user->uid
  /* creating directories */
  $root_path = textbook_companion_path();
  $dest_path = $preference_data->id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);
  $dest_path .= 'CH' . $chapter_data->number . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);
  $dest_path .= 'EX' . $example_data->number . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);
  /* updating example caption */
  /*db_query("UPDATE {textbook_companion_example} SET caption = '%s' WHERE id = %d", $form_state['values']['example_caption'], $example_id);*/
  $query = db_update('textbook_companion_example');
  $query->fields(array(
    'caption' => $form_state['values']['example_caption']
  ));
  $query->condition('id', $example_id);
  $num_updated = $query->execute();
  /* handling dependencies */
  /*db_query("DELETE FROM {textbook_companion_example_dependency} WHERE example_id = %d", $example_data->id);*/
  //$query = db_delete('textbook_companion_example_dependency');
  //$query->condition('example_id', $example_data->id);
  //$num_deleted = $query->execute();
  if ($form_state['values']['existing_depfile']['dep_chapter_example_files'] != "0") {
    foreach ($form_state['values']['existing_depfile']['checkboxes_fieldset'] as $row) {
      if ($row > 0) {
        $query = "INSERT INTO {textbook_companion_example_dependency} (example_id, dependency_id, approval_status, timestamp)
          VALUES (:example_id, :dependency_id, :approval_status, :timestamp)";
        $args = array(
          ":example_id" => $example_data->id,
          ":dependency_id" => $row,
          ":approval_status" => 0,
          ":timestamp" => time()
        );
        $result = db_query($query, $args, array(
          'return' => Database::RETURN_INSERT_ID
        ));
      } //$row > 0
    }
    drupal_set_message(t('Dependency Added successfully.'), 'status');
  }
  /* handle source file */
  $cur_file_id = $form_state['values']['cur_source_file_id'];
  if ($cur_file_id > 0) {
    /*$file_q = db_query("SELECT * FROM  {textbook_companion_example_files} WHERE id = %d AND example_id = %d", $cur_file_id, $example_data->id);
    $file_data = db_fetch_object($file_q);*/
    $query = db_select('textbook_companion_example_files');
    $query->fields('textbook_companion_example_files');
    $query->condition('id', $cur_file_id);
    $query->condition('example_id', $example_data->id);
    $result = $query->execute();
    $file_data = $result->fetchObject();
    if (!$file_data) {
      drupal_set_message("Error deleting example source file. File not present in database.", 'error');
      return;
    } //!$file_data
    if (($form_state['values']['source_file_action'] == 2) && (!$_FILES['files']['name']['sourcefile1'])) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error deleting example source file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //($form_state['values']['cur_source_checkbox'] == 1) && (!$_FILES['files']['name']['sourcefile1'])
  } //$cur_file_id > 0
  if ($_FILES['files']['name']['sourcefile1']) {
    if ($cur_file_id > 0) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error removing previous example source file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //$cur_file_id > 0
    if (file_exists($root_path . $dest_path . $_FILES['files']['name']['sourcefile1'])) {
      drupal_set_message(t("Error uploading source file. File !filename already exists.", array(
        '!filename' => $_FILES['files']['name']['sourcefile1']
      )), 'error');
      return;
    } //file_exists($root_path . $dest_path . $_FILES['files']['name']['sourcefile1'])
    /* uploading file */
    if (move_uploaded_file($_FILES['files']['tmp_name']['sourcefile1'], $root_path . $dest_path . $_FILES['files']['name']['sourcefile1'])) {
      /* for uploaded files making an entry in the database */
      /*db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
      VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
      $example_data->id,
      $_FILES['files']['name']['sourcefile1'],
      $dest_path . $_FILES['files']['name']['sourcefile1'],
      $_FILES['files']['type']['sourcefile1'],
      $_FILES['files']['size']['sourcefile1'],
      'S',
      time()
      );*/
      var_dump('ok');
      $query = "INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype,  timestamp)
     VALUES (:example_id, :filename, :filepath, :filemime, :filesize, :filetype,:timestamp)";
      $args = array(
        ":example_id" => $example_data->id,
        ":filename" => $_FILES['files']['name']['sourcefile1'],
        ":filepath" => $dest_path . $_FILES['files']['name']['sourcefile1'],
        ":filemime" => $_FILES['files']['type']['sourcefile1'],
        ":filesize" => $_FILES['files']['size']['sourcefile1'],
        ":filetype" => 'S',
        ":timestamp" => time()
      );
      /*var_dump( ":example_id=>". $example_data->id .",". ":filename=>". $_FILES['files']['name']['sourcefile1'] .",".
      ":filepath=>". $dest_path . $_FILES['files']['name']['sourcefile1'].",".
      ":filemime=>". $_FILES['files']['type']['sourcefile1'].",".
      ":filesize=>" .$_FILES['files']['size']['sourcefile1']
      );die;
      */
      $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
      ));
      drupal_set_message($_FILES['files']['name']['sourcefile1'] . ' uploaded successfully.', 'status');
    } //move_uploaded_file($_FILES['files']['tmp_name']['sourcefile1'], $root_path . $dest_path . $_FILES['files']['name']['sourcefile1'])
    else {
      drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name']['sourcefile1'], 'error');
    }
  } //$_FILES['files']['name']['sourcefile1']
  /* handle result1 file */
  $cur_file_id = $form_state['values']['cur_result1_file_id'];
  if ($cur_file_id > 0) {
    /*$file_q = db_query("SELECT * FROM  {textbook_companion_example_files} WHERE id = %d AND example_id = %d", $cur_file_id, $example_data->id);
    $file_data = db_fetch_object($file_q);*/
    $query = db_select('textbook_companion_example_files');
    $query->fields('textbook_companion_example_files');
    $query->condition('id', $cur_file_id);
    $query->condition('example_id', $example_data->id);
    $result = $query->execute();
    $file_data = $result->fetchObject();
    if (!$file_data) {
      drupal_set_message("Error deleting example result 1 file. File not present in database.", 'error');
      return;
    } //!$file_data
    if (($form_state['values']['cur_result1_checkbox'] == 1) && (!$_FILES['files']['name']['result1'])) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error deleting example result 1 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //($form_state['values']['cur_result1_checkbox'] == 1) && (!$_FILES['files']['name']['result1'])
  } //$cur_file_id > 0
  if ($_FILES['files']['name']['result1']) {
    if ($cur_file_id > 0) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error removing previous example result 1 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //$cur_file_id > 0
    if (file_exists($root_path . $dest_path . $_FILES['files']['name']['result1'])) {
      drupal_set_message(t("Error uploading result 1 file. File !filename already exists.", array(
        '!filename' => $_FILES['files']['name']['result1']
      )), 'error');
      return;
    } //file_exists($root_path . $dest_path . $_FILES['files']['name']['result1'])
    /* uploading file */
    if (move_uploaded_file($_FILES['files']['tmp_name']['result1'], $root_path . $dest_path . $_FILES['files']['name']['result1'])) {
      /* for uploaded files making an entry in the database */
      /*db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
      VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
      $example_data->id,
      $_FILES['files']['name']['result1'],
      $dest_path . $_FILES['files']['name']['result1'],
      $_FILES['files']['type']['result1'],
      $_FILES['files']['size']['result1'],
      'R',
      time()
      );*/
      $query = "INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype,     timestamp) VALUES (:example_id, :filename, :filepath, :filemime, :filesize, :filetype,:timestamp)";
      $args = array(
        ":example_id" => $example_data->id,
        ":filename" => $_FILES['files']['name']['result1'],
        ":filepath" => $dest_path . $_FILES['files']['name']['result1'],
        ":filemime" => $_FILES['files']['type']['result1'],
        ":filesize" => $_FILES['files']['size']['result1'],
        ":filetype" => 'R',
        ":timestamp" => time()
      );
      $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
      ));
      drupal_set_message($_FILES['files']['name']['result1'] . ' uploaded successfully.', 'status');
    } //move_uploaded_file($_FILES['files']['tmp_name']['result1'], $root_path . $dest_path . $_FILES['files']['name']['result1'])
    else {
      drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name']['result1'], 'error');
    }
  } //$_FILES['files']['name']['result1']
  /* handle result2 file */
  $cur_file_id = $form_state['values']['cur_result2_file_id'];
  if ($cur_file_id > 0) {
    /*$file_q = db_query("SELECT * FROM  {textbook_companion_example_files} WHERE id = %d AND example_id = %d", $cur_file_id, $example_data->id);
    $file_data = db_fetch_object($file_q);*/
    $query = db_select('textbook_companion_example_files');
    $query->fields('textbook_companion_example_files');
    $query->condition('id', $cur_file_id);
    $query->condition('example_id', $example_data->id);
    $result = $query->execute();
    $file_data = $result->fetchObject();
    if (!$file_data) {
      drupal_set_message("Error deleting example result 2 file. File not present in database.", 'error');
      return;
    } //!$file_data
    if (($form_state['values']['cur_result2_checkbox'] == 1) && (!$_FILES['files']['name']['result2'])) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error deleting example result 2 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //($form_state['values']['cur_result2_checkbox'] == 1) && (!$_FILES['files']['name']['result2'])
  } //$cur_file_id > 0
  if ($_FILES['files']['name']['result2']) {
    if ($cur_file_id > 0) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error removing previous example result 2 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //$cur_file_id > 0
    if (file_exists($root_path . $dest_path . $_FILES['files']['name']['result2'])) {
      drupal_set_message(t("Error uploading result 2 file. File !filename already exists.", array(
        '!filename' => $_FILES['files']['name']['result2']
      )), 'error');
      return;
    } //file_exists($root_path . $dest_path . $_FILES['files']['name']['result2'])
    /* uploading file */
    if (move_uploaded_file($_FILES['files']['tmp_name']['result2'], $root_path . $dest_path . $_FILES['files']['name']['result2'])) {
      /* for uploaded files making an entry in the database */
      /*db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
      VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
      $example_data->id,
      $_FILES['files']['name']['result2'],
      $dest_path . $_FILES['files']['name']['result2'],
      $_FILES['files']['type']['result2'],
      $_FILES['files']['size']['result2'],
      'R',
      time()
      );*/
      $query = "INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype,     timestamp) VALUES (:example_id, :filename, :filepath, :filemime, :filesize, :filetype,:timestamp)";
      $args = array(
        ":example_id" => $example_data->id,
        ":filename" => $_FILES['files']['name']['result2'],
        ":filepath" => $dest_path . $_FILES['files']['name']['result2'],
        ":filemime" => $_FILES['files']['type']['result2'],
        ":filesize" => $_FILES['files']['size']['result2'],
        ":filetype" => 'R',
        ":timestamp" => time()
      );
      $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
      ));
      drupal_set_message($_FILES['files']['name']['result2'] . ' uploaded successfully.', 'status');
    } //move_uploaded_file($_FILES['files']['tmp_name']['result2'], $root_path . $dest_path . $_FILES['files']['name']['result2'])
    else {
      drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name']['result2'], 'error');
    }
  } //$_FILES['files']['name']['result2']
  /* handle xcos1 file */
  $cur_file_id = $form_state['values']['cur_xcos1_file_id'];
  if ($cur_file_id > 0) {
    /*$file_q = db_query("SELECT * FROM  {textbook_companion_example_files} WHERE id = %d AND example_id = %d", $cur_file_id, $example_data->id);
    $file_data = db_fetch_object($file_q);*/
    $query = db_select('textbook_companion_example_files');
    $query->fields('textbook_companion_example_files');
    $query->condition('id', $cur_file_id);
    $query->condition('example_id', $example_data->id);
    $result = $query->execute();
    $file_data = $result->fetchObject();
    if (!$file_data) {
      drupal_set_message("Error deleting example xcos 1 file. File not present in database.", 'error');
      return;
    } //!$file_data
    if (($form_state['values']['cur_xcos1_checkbox'] == 1) && (!$_FILES['files']['name']['xcos1'])) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error deleting example xcos 1 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //($form_state['values']['cur_xcos1_checkbox'] == 1) && (!$_FILES['files']['name']['xcos1'])
  } //$cur_file_id > 0
  if ($_FILES['files']['name']['xcos1']) {
    if ($cur_file_id > 0) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error removing previous example xcos 1 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //$cur_file_id > 0
    if (file_exists($root_path . $dest_path . $_FILES['files']['name']['xcos1'])) {
      drupal_set_message(t("Error uploading xcos 1 file. File !filename already exists.", array(
        '!filename' => $_FILES['files']['name']['xcos1']
      )), 'error');
      return;
    } //file_exists($root_path . $dest_path . $_FILES['files']['name']['xcos1'])
    /* uploading file */
    if (move_uploaded_file($_FILES['files']['tmp_name']['xcos1'], $root_path . $dest_path . $_FILES['files']['name']['xcos1'])) {
      /* for uploaded files making an entry in the database */
      /*db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
      VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
      $example_data->id,
      $_FILES['files']['name']['xcos1'],
      $dest_path . $_FILES['files']['name']['xcos1'],
      $_FILES['files']['type']['xcos1'],
      $_FILES['files']['size']['xcos1'],
      'X',
      time()
      );*/
      $query = "INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype,     timestamp) VALUES (:example_id, :filename, :filepath, :filemime, :filesize, :filetype,:timestamp)";
      $args = array(
        ":example_id" => $example_data->id,
        ":filename" => $_FILES['files']['name']['xcos1'],
        ":filepath" => $dest_path . $_FILES['files']['name']['xcos1'],
        ":filemime" => $_FILES['files']['type']['xcos1'],
        ":filesize" => $_FILES['files']['size']['xcos1'],
        ":filetype" => 'X',
        ":timestamp" => time()
      );
      $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
      ));
      drupal_set_message($_FILES['files']['name']['xcos1'] . ' uploaded successfully.', 'status');
    } //move_uploaded_file($_FILES['files']['tmp_name']['xcos1'], $root_path . $dest_path . $_FILES['files']['name']['xcos1'])
    else {
      drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name']['xcos1'], 'error');
    }
  } //$_FILES['files']['name']['xcos1']
  /* handle xcos2 file */
  $cur_file_id = $form_state['values']['cur_xcos2_file_id'];
  if ($cur_file_id > 0) {
    /*$file_q = db_query("SELECT * FROM  {textbook_companion_example_files} WHERE id = %d AND example_id = %d", $cur_file_id, $example_data->id);
    $file_data = db_fetch_object($file_q);*/
    $query = db_select('textbook_companion_example_files');
    $query->fields('textbook_companion_example_files');
    $query->condition('id', $cur_file_id);
    $query->condition('example_id', $example_data->id);
    $result = $query->execute();
    $file_data = $result->fetchObject();
    if (!$file_data) {
      drupal_set_message("Error deleting example xcos 2 file. File not present in database.", 'error');
      return;
    } //!$file_data
    if (($form_state['values']['cur_xcos2_checkbox'] == 1) && (!$_FILES['files']['name']['xcos2'])) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error deleting example xcos 2 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //($form_state['values']['cur_xcos2_checkbox'] == 1) && (!$_FILES['files']['name']['xcos2'])
  } //$cur_file_id > 0
  if ($_FILES['files']['name']['xcos2']) {
    if ($cur_file_id > 0) {
      if (!delete_file($cur_file_id)) {
        drupal_set_message("Error removing previous example xcos 2 file.", 'error');
        return;
      } //!delete_file($cur_file_id)
    } //$cur_file_id > 0
    if (file_exists($root_path . $dest_path . $_FILES['files']['name']['xcos2'])) {
      drupal_set_message(t("Error uploading xcos 2 file. File !filename already exists.", array(
        '!filename' => $_FILES['files']['name']['xcos2']
      )), 'error');
      return;
    } //file_exists($root_path . $dest_path . $_FILES['files']['name']['xcos2'])
    /* uploading file */
    if (move_uploaded_file($_FILES['files']['tmp_name']['xcos2'], $root_path . $dest_path . $_FILES['files']['name']['xcos2'])) {
      /* for uploaded files making an entry in the database */
      /*db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
      VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
      $example_data->id,
      $_FILES['files']['name']['xcos2'],
      $dest_path . $_FILES['files']['name']['xcos2'],
      $_FILES['files']['type']['xcos2'],
      $_FILES['files']['size']['xcos2'],
      'X',
      time()
      );*/
      $query = "INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype,     timestamp) VALUES (:example_id, :filename, :filepath, :filemime, :filesize, :filetype,:timestamp)";
      $args = array(
        ":example_id" => $example_data->id,
        ":filename" => $_FILES['files']['name']['xcos2'],
        ":filepath" => $dest_path . $_FILES['files']['name']['xcos2'],
        ":filemime" => $_FILES['files']['type']['xcos2'],
        ":filesize" => $_FILES['files']['size']['xcos2'],
        ":filetype" => 'X',
        ":timestamp" => time()
      );
      $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
      ));
      drupal_set_message($_FILES['files']['name']['xcos2'] . ' uploaded successfully.', 'status');
    } //move_uploaded_file($_FILES['files']['tmp_name']['xcos2'], $root_path . $dest_path . $_FILES['files']['name']['xcos2'])
    else {
      drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name']['xcos2'], 'error');
    }
  } //$_FILES['files']['name']['xcos2']
  /* sending email */
  $email_to = $user->mail;
  $param['example_updated']['example_id'] = $example_id;
  $param['example_updated']['user_id'] = $user->uid;
  if (!drupal_mail('textbook_companion', 'example_updated', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');
  drupal_set_message(t("Example successfully udpated."), 'status');
}
/******************************************************************************/
/**************************** EDIT CHAPTER TITLE ******************************/
/******************************************************************************/
function edit_chapter_title_form($form, &$form_state) {
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
  $chapter_id = arg(4);
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d", $chapter_id, $preference_data->id);
  $chapter_data = db_fetch_object($chapter_q);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $chapter_id);
  $query->condition('preference_id', $preference_data->id);
  $result = $query->execute();
  $chapter_data = $result->fetchObject();
  if (!$chapter_data) {
    drupal_set_message(t('Invalid chapter.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  } //!$chapter_data
  $form['#redirect'] = 'textbook_companion/code';
  $form['book_details']['book'] = array(
    '#type' => 'item',
    '#markup' => $preference_data->book,
    '#title' => t('Title of the Book')
  );
  $form['contributor_name'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->full_name,
    '#title' => t('Contributor Name')
  );
  $form['number'] = array(
    '#type' => 'item',
    '#title' => t('Chapter No'),
    '#markup' => $chapter_data->number
  );
  $form['chapter_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Chapter'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $chapter_data->name
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'textbook_companion/code')
  );
  return $form;
}
function edit_chapter_title_form_validate($form, &$form_state) {
  if (!check_name($form_state['values']['chapter_title']))
    form_set_error('chapter_title', t('Title of the Chapter can contain only alphabets, numbers and spaces.'));
}
function edit_chapter_title_form_submit($form, &$form_state) {
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
  $chapter_id = arg(4);
  /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d", $chapter_id, $preference_data->id);
  $chapter_data = db_fetch_object($chapter_q);*/
  $query = db_select('textbook_companion_chapter');
  $query->fields('textbook_companion_chapter');
  $query->condition('id', $chapter_id);
  $query->condition('preference_id', $preference_data->id);
  $result = $query->execute();
  $chapter_data = $result->fetchObject();
  if (!$chapter_data) {
    drupal_set_message(t('Invalid chapter.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  } //!$chapter_data
  /*db_query("UPDATE {textbook_companion_chapter} SET name = '%s' WHERE id = %d", $form_state['values']['chapter_title'], $chapter_id);*/
  $query = db_update('textbook_companion_chapter');
  $query->fields(array(
    'name' => $form_state['values']['chapter_title']
  ));
  $query->condition('id', $chapter_id);
  $num_updated = $query->execute();
  /* sending email */
  $email_to = $user->mail;
  $from = variable_get('textbook_companion_from_email', '');
  $bcc = variable_get('textbook_companion_emails', '');
  $cc = variable_get('textbook_companion_cc_emails', '');
  $param['chapter_title_updated']['chapter_id'] = $chapter_data->id;
  $param['chapter_title_updated']['user_id'] = $user->uid;
  if (!drupal_mail('textbook_companion', 'chapter_title_updated', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');
  drupal_set_message(t('Title of the Chapter updated.'), 'status');
  drupal_goto('textbook_companion/code');
}
/******************************************************************************/
/************************** GENERAL FUNCTIONS *********************************/
/******************************************************************************/
function _list_of_book_titles() {
  $book_titles = array(
    '0' => 'Please select...'
  );
  /*$book_titles_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");*/
  $query = db_select('textbook_companion_preference');
  $query->fields('textbook_companion_preference');
  $or = db_or();
  $or->condition('approval_status', 1);
  $or->condition('approval_status', 3);
  $query->condition($or);
  $query->orderBy('book', 'ASC');
  $book_titles_q = $query->execute();
  while ($book_titles_data = $book_titles_q->fetchObject()) {
    $book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
  } //$book_titles_data = $book_titles_q->fetchObject()
  return $book_titles;
}
function _list_of_book_dependency_files() {
  $book_dependency_files = array();
  $book_dependency_files_class = array();
  /*$book_dependency_files_q = db_query("SELECT * FROM {textbook_companion_dependency_files} ORDER BY filename ASC");*/
  $query = db_select('textbook_companion_dependency_files', 'tcdp');
  $query->fields('tcdp');
  $query->fields('tcp', array(
    'book',
    'author',
    'publisher',
    'edition',
    'year'
  ));
  $query->leftJoin('textbook_companion_preference', 'tcp', 'tcdp.preference_id = tcp.id');
  $book_dependency_files_q = $query->execute();
  while ($book_dependency_files_data = $book_dependency_files_q->fetchObject()) {
    $temp_caption = '';
    if ($book_dependency_files_data->caption)
      $temp_caption .= ' ===>[Caption: ' . $book_dependency_files_data->caption . ' ' . ', Book: ' . $book_dependency_files_data->book . ', Author: ' . $book_dependency_files_data->author . ', Publisher: ' . $book_dependency_files_data->publisher . ', Edition: ' . $book_dependency_files_data->edition . ', Year: ' . $book_dependency_files_data->year . ']';
    $book_dependency_files[$book_dependency_files_data->id] = l($book_dependency_files_data->filename, 'download/dependency/' . $book_dependency_files_data->id, array(
      'attributes' => array(
        'class' => 'title'
      )
    )) . $temp_caption;
    $book_dependency_files_class[$book_dependency_files_data->id] = $book_dependency_files_data->preference_id;
  } //$book_dependency_files_data = $book_dependency_files_q->fetchObject()
  return array(
    $book_dependency_files,
    $book_dependency_files_class
  );
}
/**
 *function to return list of completed books
 */
function completed_book() {
  $book_options_list = array();
  $preference_q = db_query("SELECT DISTINCT preference_id FROM {textbook_companion_dependency_files}");
  while ($preference_data = $preference_q->fetchObject()) {
    $query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $query->condition('id', $preference_data->preference_id);
    $query->orderBy('book', 'ASC');
    $query = $query->execute();
    $book_data = $query->fetchObject();
    if ($book_data->book)
      $book_options_list[$preference_data->preference_id] = $book_data->book;
  }
  return $book_options_list;
}
function ajax_checkboxes_callback($form, &$form_state) {
  return $form['existing_depfile']['checkboxes_fieldset'];
}
