<?php
// $Id$
function upload_examples()
{
	return drupal_get_form('upload_examples_form');
}
function upload_examples_form($form, &$form_state)
{
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
	if (!$proposal_data)
	{
		drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
		drupal_goto('');
	} //!$proposal_data
	//need to change according to new_proposal_status and book_status
	if ($proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4)
	{
		switch ($proposal_data->proposal_status)
		{
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
			case 3:
				drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
				drupal_goto('');
				return;
				break;
			default:
				drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
				drupal_goto('');
				return;
				break;
		} //$proposal_data->proposal_status
	} //$proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4
	/*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
	$preference_data = db_fetch_object($preference_q);*/
	$query = db_select('textbook_companion_preference');
	$query->fields('textbook_companion_preference');
	$query->condition('proposal_id', $proposal_data->id);
	$query->condition('approval_status', 1);
	$query->range(0, 1);
	$result = $query->execute();
	$preference_data = $result->fetchObject();
	if (!$preference_data)
	{
		drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
		drupal_goto('');
		return;
	} //!$preference_data
	/************************ end approve book details **************************/
	/* add javascript for automatic book title, check if example uploaded, dependency selection effects */
	$chapter_name_js = "(function ($) {

$(document).ready(function(){

    $('#edit-number').change(function() {
  var ed = $('#edit-number').val();



      $.get('" . base_path() . "textbook_companion/ajax/chapter_title/' + $('#edit-number').val() + '/' + " . $preference_data->id . ", function(data) {
        $('#edit-name').val(data);
      });
    });
    $('#edit-example-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/example_exists/' + $('#edit-number').val() + '/' + " . $preference_data->id . " + '/' + $('#edit-example-number').val(), function(data) {
        if (data) {
         //alert(data);
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

       });
 })(jQuery);";
	//drupal_add_js($chapter_name_js, 'inline', 'header');
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
	/*
	$options = array(
		'' => '(Select)'
	);
	for ($i = 1; $i <= 100; $i++)
	{
		$options[$i] = $i;
	} //$i = 1; $i <= 100; $i++
	*/
	/*existing chapters saved in $options*/
	$options = array(
		'' => 'select',
	);
	/*'New Chpater' option for entering new chapter*/
	$options[0] = 'New Chapter';
	$chapter_description = array(
		'chapter_number' => 'chapter_name'
	);
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('preference_id', $preference_data->id);
	//$query->condition('number', $form_state['values']['number']);
	$chapter_result = $query->execute();
	while($data = $chapter_result->fetchObject()){
		$options[$data->number] = $data->number;
		$chapter_description[$data->number] = $data->name;
	}
	/*If existing chapter selected, automatically 'name' field will be filled by chapter name*/
	/*Else textfiled field will be popup, for entering chapter */
	$form['number'] = array(
		'#type' => 'select',
		'#title' => t('Chapter No'),
		'#empty_option' => '-Select-',
		'#options' => $options,
		'#ajax' => array(
      'callback' => 'display_chapter_name_callback',
      'wrapper' => 'display-chapter-name-wrapper',
     ),

		'#multiple' => FALSE,
		'#size' => 1,
		'#required' => TRUE,
	);
	$form['chapter_not_exist'] = array(
		'#type' => 'textfield',
		'#title' => t('Chapter No.'),
		'#size' => 48,
		//'#description' => t("<Strong style='color:red' >Upload new Main or Source file above, if you want to replace the existing file. Leave blank if you want to keep using the existing file.</strong> <br />") . t('Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', ''),
		'#states' => array(
			'visible' => array(
				':input[name="number"]' => array(
					'value' => '0'
				)
			)
		)
	);
	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('Title of the Chapter'),
		//'#value' => "",
		'#size' => 40,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#prefix' => '<div id = "display-chapter-name-wrapper">',
		'#suffix' => '</div>',
	);
	if(isset($form_state['values']['number']))
	{
		if($form_state['values']['number'] != "0")
		{
			$form['name']['#value'] = $chapter_description[$form_state['values']['number']];
			$form['name']['#description'] = t("<div style='color:red'><strong>Note:</strong> Chapter name will be according to existing chapter no. seleted.</div>");
		}
	}
	$form['example_number'] = array(
		'#type' => 'textfield',
		'#title' => t('Example No'),
		'#size' => 5,
		'#maxlength' => 10,
		'#description' => t("Example number should be separated by dots only.<br />Example: 1.1.a &nbsp;or&nbsp; 1.1.1"),
		'#required' => TRUE
	);
	$form['example_caption'] = array(
		'#type' => 'textfield',
		'#title' => t('Caption'),
		'#size' => 40,
		'#maxlength' => 255,
		'#description' => t('Example caption should contain only alphabets, numbers and spaces.'),
		'#required' => TRUE
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
	$form['sourcefile']['sourcefile1'] = array(
		'#type' => 'file',
		'#title' => t('Upload main or source file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', '')
	);
	$form['dep_files'] = array(
		'#type' => 'item',
		'#title' => t('Dependency Files')
	);
	/************ START OF EXISTING DEPENDENCIES **************/
	/* existing dependencies */
	$form['existing_depfile'] = array(
		'#type' => 'fieldset',
		'#title' => t('Use Already Existing Dependency Files'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
		'#prefix' => '<div id="existing-depfile-wrapper">',
		'#suffix' => '</div>',
		'#tree' => TRUE
	);
	/* existing dependencies */
	$form['existing_depfile']['selected'] = array(
		'#type' => 'item',
		'#title' => t('Existing Dependency Files Selected'),
		'#markup' => '<div id="existing_depfile_selected"></div>'
	);
	/* $form['existing_depfile']['dep_book_title'] = array(
	'#type' => 'select',
	'#title' => t('Title of the Book'),
	'#options' => _list_of_book_titles(),
	);*/
	list($files_options, $files_options_class) = _list_of_book_dependency_files($preference_data->id);
	$form['existing_depfile']['dep_book'] = array(
		'#type' => 'item',
		'#title' => t('Dependency Files')
	);
	$form['existing_depfile']['dep_chapter_example_files'] = array(
		'#type' => 'checkboxes',
		//'#title' => t('Dependency Files'),
		'#options' => $files_options,
		'#options_class' => $files_options_class,
		'#prefix' => '<div id="ckb" style="height:500px;overflow:auto;">',
		'#suffix' => '</div>',
		'#multiple' => TRUE
	);
	$form['existing_depfile']['dep_upload'] = array(
		'#type' => 'item',
		'#markup' => '<hr style="color:red">' . l('Upload New Depedency Files', 'textbook_companion/code/upload_dep')
	);
	/************ END OF EXISTING DEPENDENCIES **************/
	$form['result'] = array(
		'#type' => 'fieldset',
		'#title' => t('Result Files'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE
	);
	$form['result']['result1'] = array(
		'#type' => 'file',
		'#title' => t('Upload result file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('Allowed file extensions : ') . variable_get('textbook_companion_result_extensions', '')
	);
	$form['result']['result2'] = array(
		'#type' => 'file',
		'#title' => t('Upload result file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('Allowed file extensions : ') . variable_get('textbook_companion_result_extensions', '')
	);
	$form['xcos'] = array(
		'#type' => 'fieldset',
		'#title' => t('XCOS Files'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE
	);
	$form['xcos']['xcos1'] = array(
		'#type' => 'file',
		'#title' => t('Upload xcos file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('Allowed file extensions : ') . variable_get('textbook_companion_xcos_extensions', '')
	);
	$form['xcos']['xcos2'] = array(
		'#type' => 'file',
		'#title' => t('Upload xcos file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('Allowed file extensions : ') . variable_get('textbook_companion_xcos_extensions', '')
	);
	$form['preference_id'] = array(
		'#type' => 'hidden',
		'#value' => $preference_data->id,
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
/*upload exanple form validation*/
function upload_examples_form_validate($form, &$form_state)
{
	if($form_state['values']['number'] == '0') {
		if(!is_numeric($form_state['values']['chapter_not_exist']))
			form_set_error('chapter_not_exist', t('Chapter No. can contains only numbers.'));
		if (!check_name($form_state['values']['name']))
			form_set_error('name', t('Title of the Chapter can contain only alphabets, numbers and spaces.'));
		if(check_chapter_exist($form_state['values']['chapter_not_exist'], $form_state['values']['preference_id']))
			form_set_error('chapter_not_exist', t('Chapter Exist. Plese enter new chapter.'));
	}
	else {
		if(!check_chapter_name($form_state['values']['number'], $form_state['values']['name'], $form_state['values']['preference_id']))
			form_set_error('name', 'Chapter name can not be changed. You can change chapter name in section.');
	}
	if (!check_name($form_state['values']['example_caption']))
		form_set_error('example_caption', t('Example Caption can contain only alphabets, numbers and spaces.'));
	if (!check_chapter_number($form_state['values']['example_number']))
		form_set_error('example_number', t('Invalid Example Number. Example Number can contain only alphabets and numbers sepereated by dot.'));
	if (isset($_FILES['files']))
	{
		/* check if atleast one source or result file is uploaded */
		if (!($_FILES['files']['name']['sourcefile1'] || $_FILES['files']['name']['xcos1']))
			form_set_error('sourcefile1', t('Please upload atleast one main or source file or xcos file.'));
		/* check for valid filename extensions */
		foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
		{
			if ($file_name)
			{
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
				switch ($file_type)
				{
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
	$chapter_name_js = "(function ($) {
    $('#edit-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/chapter_title/' + $('#edit-number').val() + '/' + " . $row->pre_id . ", function(data) {
        $('#edit-name').val(data);
      });
    });
    $('#edit-example-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/example_exists/' + $('#edit-number').val() + '/' + $('#edit-example-number').val(), function(data) {
        if (data) {
          // alert(data);
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
          dep_fed += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
   })(jQuery);";
	drupal_add_js($chapter_name_js, 'inline', 'header');
}
function upload_examples_form_submit($form, &$form_state)
{
	global $user;
	$root_path = textbook_companion_path();
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
	if (!$proposal_data)
	{
		drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
		drupal_goto('');
	} //!$proposal_data
	if ($proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4)
	{
		switch ($proposal_data->proposal_status)
		{
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
			case 3:
				drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'textbook_companion/proposal') . '.'), 'status');
				drupal_goto('');
				return;
				break;
			default:
				drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
				drupal_goto('');
				return;
				break;
		} //$proposal_data->proposal_status
	} //$proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4
	/*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
	$preference_data = db_fetch_object($preference_q);*/
	$query = db_select('textbook_companion_preference');
	$query->fields('textbook_companion_preference');
	$query->condition('proposal_id', $proposal_data->id);
	$query->condition('approval_status', 1);
	$query->range(0, 1);
	$result = $query->execute();
	$preference_data = $result->fetchObject();
	if (!$preference_data)
	{
		drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
		drupal_goto('');
		return;
	} //!$preference_data
	/************************ end approve book details **************************/
	$preference_id = $preference_data->id;
	$dest_path = $preference_id . '/';
	if (!is_dir($root_path . $dest_path))
		mkdir($root_path . $dest_path);
	/* inserting chapter details */
	$chapter_id = 0;
	/*$chapter_result = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d AND number = %d", $preference_id, $form_state['values']['number']);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('preference_id', $preference_id);
	$query->condition('number', $form_state['values']['number']);
	$chapter_result = $query->execute();
	if (!$chapter_row = $chapter_result->fetchObject())
	{
		/*db_query("INSERT INTO {textbook_companion_chapter} (preference_id, number, name) VALUES (%d, '%s', '%s')",
		$preference_id,
		$form_state['values']['number'],
		$form_state['values']['name']
		);
		$chapter_id = db_last_insert_id('textbook_companion_chapter', 'id'); */
		$query = "INSERT INTO {textbook_companion_chapter} (preference_id, number, name) VALUES(:preference_id, :number, :name)";
		if($form_state['values']['number']) {
			$args = array(
				":preference_id" => $preference_id,
				":number" => $form_state['values']['number'],
				":name" => $form_state['values']['name']
			);
		}
		else {
			$args = array(
				":preference_id" => $preference_id,
				":number" => $form_state['values']['chapter_not_exist'],
				":name" => $form_state['values']['name']
			);
		}
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$chapter_id = $result;
	} //!$chapter_row = $chapter_result->fetchObject()
	else
	{
		$chapter_id = $chapter_row->id;
		/*db_query("UPDATE {textbook_companion_chapter} SET name = '%s' WHERE id = %d", $form_state['values']['name'], $chapter_id);*/
		$query = db_update('textbook_companion_chapter');
		$query->fields(array(
			'name' => $form_state['values']['name']
		));
		$query->condition('id', $chapter_id);
		$num_updated = $query->execute();
	}
	/*  get example details - dont allow if already example present */
	/*$cur_example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND number = '%s'", $chapter_id, $form_state['values']['example_number']);*/
	$query = db_select('textbook_companion_example');
	$query->fields('textbook_companion_example');
	$query->condition('chapter_id', $chapter_id);
	$query->condition('number', $form_state['values']['example_number']);
	$cur_example_q = $query->execute();
	if ($cur_example_d = $cur_example_q->fetchObject())
	{
		if ($cur_example_d->approval_status == 1)
		{
			drupal_set_message(t("Example already approved. Cannot overwrite it."), 'error');
			drupal_goto('textbook_companion/code');
			return;
		} //$cur_example_d->approval_status == 1
		else if ($cur_example_d->approval_status == 0)
		{
			drupal_set_message(t("Example is under pending review. Delete the example and reupload it."), 'error');
			drupal_goto('textbook_companion/code');
			return;
		} //$cur_example_d->approval_status == 0
		else
		{
			drupal_set_message(t("Error uploading example. Please contact administrator."), 'error');
			drupal_goto('textbook_companion/code');
			return;
		}
	} //$cur_example_d = $cur_example_q->fetchObject()
	/* creating directories */
	$dest_path .= 'CH' . $form_state['values']['number'] . '/';
	if (!is_dir($root_path . $dest_path))
		mkdir($root_path . $dest_path);
	$dest_path .= 'EX' . $form_state['values']['example_number'] . '/';
	if (!is_dir($root_path . $dest_path))
		mkdir($root_path . $dest_path);
	/* creating example database entry */
	/*db_query("INSERT INTO {textbook_companion_example} (chapter_id, number, caption, approval_status, timestamp) VALUES (%d, '%s', '%s', %d, %d)",
	$chapter_id,
	$form_state['values']['example_number'],
	$form_state['values']['example_caption'],
	0,
	time()
	);
	$example_id = db_last_insert_id('textbook_companion_example', 'id');*/
	$query = "INSERT INTO {textbook_companion_example} (approver_uid, chapter_id, number, caption, approval_date, approval_status, timestamp) VALUES 	(:approver_uid, :chapter_id, :number, :caption, :approval_date,:approval_status, :timestamp)";
	$args = array(
		":approver_uid" => 0,
		":chapter_id" => $chapter_id,
		":number" => $form_state['values']['example_number'],
		":caption" => $form_state['values']['example_caption'],
		":approval_date" => time(),
		":approval_status" => 0,
		":timestamp" => time()
	);
	$result = db_query($query, $args, array(
		'return' => Database::RETURN_INSERT_ID
	));
	$example_id = $result;
	/* linking existing dependencies */
	foreach ($form_state['values']['existing_depfile']['dep_chapter_example_files'] as $row)
	{
		if ($row > 0)
		{
			/* insterting into database */
			/*db_query("INSERT INTO {textbook_companion_example_dependency} (example_id, dependency_id, approval_status, timestamp)
			VALUES (%d, %d, %d, %d)",
			$example_id,
			$row,
			0,
			time()
			);*/
			$query = "INSERT INTO {textbook_companion_example_dependency} (example_id, dependency_id, approval_status, timestamp)
        VALUES  (:example_id, :dependency_id, :approval_status, :timestamp)";
			$args = array(
				":example_id" => $example_id,
				":dependency_id" => $row,
				":approval_status" => 0,
				":timestamp" => time()
			);
			$result = db_query($query, $args, array(
				'return' => Database::RETURN_INSERT_ID
			));
		} //$row > 0
	} //$form_state['values']['existing_depfile']['dep_chapter_example_files'] as $row
	/* uploading files */
	foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
	{
		if ($file_name)
		{
			/* checking file type */
			if (strstr($file_form_name, 'source'))
				$file_type = 'S';
			else if (strstr($file_form_name, 'result'))
				$file_type = 'R';
			else if (strstr($file_form_name, 'xcos'))
				$file_type = 'X';
			else
				$file_type = 'U';
			if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
			{
				drupal_set_message(t("Error uploading file. File !filename already exists.", array(
					'!filename' => $_FILES['files']['name'][$file_form_name]
				)), 'error');
				return;
			} //file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name])
			/* uploading file */
			if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
			{
				/* for uploaded files making an entry in the database */
				/*db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
				VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
				$example_id,
				$_FILES['files']['name'][$file_form_name],
				$dest_path . $_FILES['files']['name'][$file_form_name],
				$_FILES['files']['type'][$file_form_name],
				$_FILES['files']['size'][$file_form_name],
				$file_type,
				time()
				);*/
				$query = "INSERT INTO {textbook_companion_example_files} (example_id, caption, filename, filepath,filemime, filesize, filetype, timestamp)
          VALUES (:example_id, :caption, :filename, :filepath,:filemime, :filesize, :filetype, :timestamp)";
				$args = array(
					":example_id" => $example_id,
					":caption" => 'None',
					":filename" => $_FILES['files']['name'][$file_form_name],
					":filepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
					":filemime" => $_FILES['files']['type'][$file_form_name],
					":filesize" => $_FILES['files']['size'][$file_form_name],
					":filetype" => $file_type,
					":timestamp" => time()
				);
				$result = db_query($query, $args, array(
					'return' => Database::RETURN_INSERT_ID
				));
				drupal_set_message($file_name . ' uploaded successfully.', 'status');
			} //move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])
			else
			{
				drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
			}
		} //$file_name
	} //$_FILES['files']['name'] as $file_form_name => $file_name
	drupal_set_message('Example uploaded successfully.', 'status');
	/* sending email */
	$email_to = $user->mail;
	$from = variable_get('textbook_companion_from_email', '');
	$bcc = variable_get('textbook_companion_emails', '');
	$cc = variable_get('textbook_companion_cc_emails', '');
	$params['example_uploaded']['example_id'] = $example_id;
	$params['example_uploaded']['user_id'] = $user->uid;
	$params['example_uploaded']['headers'] = array(
		'From' => $from,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
	if (!drupal_mail('textbook_companion', 'example_uploaded', $email_to, language_default(), $params, $from, TRUE))
		drupal_set_message('Error sending email message.', 'error');
	drupal_goto('textbook_companion/code');
}
/******************************************************************************/
/***************************** DELETE CHAPTER *********************************/
/******************************************************************************/
function _chapter_delete()
{
	global $user;
	$chapter_id = arg(4);
	delete_chapter($chapter_id);
	drupal_set_message(t("Chapter deleted successfully."), 'status');
	drupal_goto('textbook_companion/code');
}
/******************************************************************************/
/***************************** DELETE EXAMPLE *********************************/
/******************************************************************************/
function _upload_examples_delete()
{
	global $user;
	$root_path = textbook_companion_path();
	$example_id = arg(3);
	/* check example */
	/*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
	$example_data = db_fetch_object($example_q);*/
	$query = db_select('textbook_companion_example');
	$query->fields('textbook_companion_example');
	$query->condition('id', $example_id);
	$query->range(0, 1);
	$result = $query->execute();
	$example_data = $result->fetchObject();
	if (!$example_data)
	{
		drupal_set_message('Invalid example.', 'error');
		drupal_goto('textbook_companion/code');
		return;
	} //!$example_data
	if ($example_data->approval_status != 0)
	{
		drupal_set_message('You cannnot delete an example after it has been approved. Please contact site administrator if you want to delete this example.', 'error');
		drupal_goto('textbook_companion/code');
		return;
	} //$example_data->approval_status != 0
	/*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d LIMIT 1", $example_data->chapter_id);
	$chapter_data = db_fetch_object($chapter_q);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('id', $example_data->chapter_id);
	$query->range(0, 1);
	$result = $query->execute();
	$chapter_data = $result->fetchObject();
	if (!$chapter_data)
	{
		drupal_set_message('You do not have permission to delete this example.', 'error');
		drupal_goto('textbook_companion/code');
		return;
	} //!$chapter_data
	/*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d LIMIT 1", $chapter_data->preference_id);
	$preference_data = db_fetch_object($preference_q);*/
	$query = db_select('textbook_companion_preference');
	$query->fields('textbook_companion_preference');
	$query->condition('id', $chapter_data->preference_id);
	$query->range(0, 1);
	$result = $query->execute();
	$preference_data = $result->fetchObject();
	if (!$preference_data)
	{
		drupal_set_message('You do not have permission to delete this example.', 'error');
		drupal_goto('textbook_companion/code');
		return;
	} //!$preference_data
	/*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d AND uid = %d LIMIT 1", $preference_data->proposal_id, $user->uid);
	$proposal_data = db_fetch_object($proposal_q);*/
	$query = db_select('textbook_companion_proposal');
	$query->fields('textbook_companion_proposal');
	$query->condition('id', $preference_data->proposal_id);
	$query->condition('uid', $user->uid);
	$query->range(0, 1);
	$result = $query->execute();
	$proposal_data = $result->fetchObject();
	if (!$proposal_data)
	{
		drupal_set_message('You do not have permission to delete this example.', 'error');
		drupal_goto('textbook_companion/code');
		return;
	} //!$proposal_data
	/* deleting example files */
	if (delete_example($example_data->id))
	{
		drupal_set_message('Example deleted.', 'status');-
		/* sending email */
		$email_to = $user->mail;
		$from = variable_get('textbook_companion_from_email', '');
		$bcc = variable_get('textbook_companion_emails', '');
		$cc = variable_get('textbook_companion_cc_emails', '');
		$params['example_deleted_user']['book_title'] = $preference_data->book;
		$params['example_deleted_user']['chapter_title'] = $chapter_data->name;
		$params['example_deleted_user']['example_number'] = $example_data->number;
		$params['example_deleted_user']['example_caption'] = $example_data->caption;
		$params['example_deleted_user']['user_id'] = $user->uid;
		$params['example_deleted_user']['headers'] = array(
			'From' => $from,
			'MIME-Version' => '1.0',
			'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
			'Content-Transfer-Encoding' => '8Bit',
			'X-Mailer' => 'Drupal',
			'Cc' => $cc,
			'Bcc' => $bcc
		);
		if (!drupal_mail('textbook_companion', 'example_deleted_user', $email_to, language_default(), $params, $from, TRUE))
			drupal_set_message('Error sending email message.', 'error');
	} //delete_example($example_data->id)
	else
	{
		drupal_set_message('Error deleting example.', 'status');
	}
	drupal_goto('textbook_companion/code');
	return;
}
/******************************************************************************/
/************************** GENERAL FUNCTIONS *********************************/
/******************************************************************************/
function _list_of_book_titles()
{
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
	while ($book_titles_data = $book_titles_q->fetchObject())
	{
		$book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
	} //$book_titles_data = $book_titles_q->fetchObject()
	return $book_titles;
}
function _list_of_book_dependency_files($book_id)
{
	global $user;
	$book_dependency_files = array();
	$book_dependency_files_class = array();
	$query = db_select('textbook_companion_dependency_files');
	$query->fields('textbook_companion_dependency_files');
	$query->condition('preference_id', $book_id);
	$query->orderBy('filename', 'ASC');
	$book_dependency_files_q = $query->execute();
	while ($book_dependency_files_data = $book_dependency_files_q->fetchObject())
	{
		$temp_caption = '';
		if ($book_dependency_files_data->caption)
		{
			$book_dependency_files[$book_dependency_files_data->id] = l($book_dependency_files_data->filename, 'download/dependency/' . $book_dependency_files_data->id, array(
				'attributes' => array(
					'class' => 'title'
				)
			)) . $temp_caption;
			$book_dependency_files_class[$book_dependency_files_data->id] = $book_dependency_files_data->preference_id;
		} //$book_dependency_files_data->caption
	} //$book_dependency_files_data = $book_dependency_files_q->fetchObject()
	return array(
		$book_dependency_files,
		$book_dependency_files_class
	);
}
function display_chapter_name_callback($form, &$form_state) {
	return $form['name'];
}
/*function returns wheather chapter no. exists or not*/
function check_chapter_exist($number, $preference_id) {
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('preference_id', $preference_id);
	$query->condition('number', $number);
	$chapter_result = $query->execute();
	if($data = $chapter_result->fetchObject()){
		if($number == $data->number)
			return TRUE;
	}
	return FALSE;
}
/*function returns wheather existing chapter name is changed or not*/
function check_chapter_name($number, $name, $preference_id) {
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('preference_id', $preference_id);
	$query->condition('number', $number);
	$chapter_result = $query->execute();
	if($data = $chapter_result->fetchObject())
	{
		if($name == $data->name)
			return TRUE;
	}
	return FALSE;
}
