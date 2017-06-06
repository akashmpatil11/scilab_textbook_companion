<?php
// $Id$
function code_approval()
{
	/* get a list of unapproved chapters */
	/*$pending_chapter_q = db_query("SELECT c.id as c_id, c.number as c_number, c.name as c_name, c.preference_id as c_preference_id FROM {textbook_companion_example} as e JOIN {textbook_companion_chapter} as c ON c.id = e.chapter_id WHERE e.approval_status = 0");*/
	$query = db_select('textbook_companion_example', 'e');
	$query->fields('c', array(
		'id',
		'number',
		'name',
		'preference_id'
	));
	$query->addField('c', 'id', 'c_id');
	$query->addField('c', 'number', 'c_number');
	$query->addField('c', 'name', 'c_name');
	$query->addField('c', 'preference_id', 'c_preference_id');
	$query->innerJoin('textbook_companion_chapter', 'c', 'c.id = e.chapter_id');
	$query->condition('e.approval_status', 0);
	$pending_chapter_q = $query->execute();
	if (!$pending_chapter_q)
	{
		drupal_set_message(t('There are no pending code approvals.'), 'status');
		return '';
	} //!$pending_chapter_q
	$rows = array();
	while ($row = $pending_chapter_q->fetchObject())
	{
		/* get preference data */
		/*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $row->c_preference_id);
		$preference_data = db_fetch_object($preference_q);*/
		$query = db_select('textbook_companion_preference');
		$query->fields('textbook_companion_preference');
		$query->condition('id', $row->c_preference_id);
		$result = $query->execute();
		$preference_data = $result->fetchObject();
		/* get proposal data */
		/*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
		$proposal_data = db_fetch_object($proposal_q);*/
		$query = db_select('textbook_companion_proposal');
		$query->fields('textbook_companion_proposal');
		$query->condition('id', $preference_data->proposal_id);
		$result = $query->execute();
		$proposal_data = $result->fetchObject();
		/* setting table row information */
		$rows[] = array(
			$preference_data->book,
			$row->c_number,
			$row->c_name,
			$proposal_data->full_name,
			l('Edit', 'code_approval/approve/' . $row->c_id)
		);
	} //$row = $pending_chapter_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$rows)
	{
		drupal_set_message(t('There are no pending proposals'), 'status');
		return '';
	} //!$rows
	$header = array(
		'Title of the Book',
		'Chapter Number',
		'Title of the Chapter',
		'Contributor Name',
		'Actions'
	);
	$output = theme('table', array(
		'header' => $header,
		'rows' => $rows
	));
	return $output;
}
function code_approval_form($form_state)
{
	/* get a list of unapproved chapters */
	$chapter_id = arg(2);
	/*$pending_chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $chapter_id);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('id', $chapter_id);
	$pending_chapter_q = $query->execute();
	if ($pending_chapter_data = $pending_chapter_q->fetchObject())
	{
		/* get preference data */
		/*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $pending_chapter_data->preference_id);
		$preference_data = db_fetch_object($preference_q);*/
		$query = db_select('textbook_companion_preference');
		$query->fields('textbook_companion_preference');
		$query->condition('id', $pending_chapter_data->preference_id);
		$result = $query->execute();
		$preference_data = $result->fetchObject();
		/* get proposal data */
		/*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
		$proposal_data = db_fetch_object($proposal_q);*/
		$query = db_select('textbook_companion_proposal');
		$query->fields('textbook_companion_proposal');
		$query->condition('id', $preference_data->proposal_id);
		$result = $query->execute();
		$proposal_data = $result->fetchObject();
	} //$pending_chapter_data = $pending_chapter_q->fetchObject()
	else
	{
		drupal_set_message(t('Invalid chapter selected.'), 'error');
		drupal_goto('code_approval');
		return;
	}
	$form['#tree'] = TRUE;
	$form['contributor'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->full_name,
		'#title' => t('Contributor Name')
	);
	$form['book_details']['book'] = array(
		'#type' => 'item',
		'#markup' => $preference_data->book,
		'#title' => t('Title of the Book')
	);
	$form['book_details']['number'] = array(
		'#type' => 'item',
		'#markup' => $pending_chapter_data->number,
		'#title' => t('Chapter Number')
	);
	$form['book_details']['name'] = array(
		'#type' => 'item',
		'#markup' => $pending_chapter_data->name,
		'#title' => t('Title of the Chapter')
	);
	$form['book_details']['back_to_list'] = array(
		'#type' => 'item',
		'#markup' => l('Back to Code Approval List', 'code_approval')
	);
	/* get example data */
	/*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND approval_status = 0", $chapter_id);*/
	$query = db_select('textbook_companion_example');
	$query->fields('textbook_companion_example');
	$query->condition('chapter_id', $chapter_id);
	$query->condition('approval_status', 0);
	$example_q = $query->execute();
	while ($example_data = $example_q->fetchObject())
	{
		$form['example_details'][$example_data->id] = array(
			'#type' => 'fieldset',
			'#collapsible' => FALSE,
			'#collapsed' => TRUE
		);
		$form['example_details'][$example_data->id]['example_number'] = array(
			'#type' => 'item',
			'#markup' => $example_data->number,
			'#title' => t('Example Number')
		);
		$form['example_details'][$example_data->id]['example_caption'] = array(
			'#type' => 'item',
			'#markup' => $example_data->caption,
			'#title' => t('Example Caption')
		);
		$form['example_details'][$example_data->id]['download'] = array(
			'#type' => 'markup',
			'#markup' => l('Download Example', 'download/example/' . $example_data->id)
		);
		$form['example_details'][$example_data->id]['approved'] = array(
			'#type' => 'radios',
			'#options' => array(
				'Approved',
				'Dis-approved'
			)
		);
		$form['example_details'][$example_data->id]['message'] = array(
			'#type' => 'textfield',
			'#title' => t('Reason for dis-approval')
		);
		$form['example_details'][$example_data->id]['example_id'] = array(
			'#type' => 'hidden',
			'#value' => $example_data->id
		);
	} //$example_data = $example_q->fetchObject()
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}
function code_approval_form_submit($form, &$form_state)
{
	global $user;
	foreach ($form_state['values']['example_details'] as $ex_id => $ex_data)
	{
		/*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $ex_data['example_id']);
		$example_data = db_fetch_object($example_q);*/
		$query = db_select('textbook_companion_example');
		$query->fields('textbook_companion_example');
		$query->condition('id', $ex_data['example_id']);
		$query->range(0, 1);
		$result = $query->execute();
		$example_data = $result->fetchObject();
		/*$chapter_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d LIMIT 1", $example_data->chapter_id));*/
		$query = db_select('textbook_companion_chapter');
		$query->fields('textbook_companion_chapter');
		$query->condition('id', $example_data->chapter_id);
		$query->range(0, 1);
		$result = $query->execute();
		$chapter_data = $result->fetchObject();
		/*$preference_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d LIMIT 1", $chapter_data->preference_id));*/
		$query = db_select('textbook_companion_preference');
		$query->fields('textbook_companion_preference');
		$query->condition('id', $chapter_data->preference_id);
		$query->range(0, 1);
		$result = $query->execute();
		$preference_data = $result->fetchObject();
		/*$proposal_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d LIMIT 1", $preference_data->proposal_id));*/
		$query = db_select('textbook_companion_proposal');
		$query->fields('textbook_companion_proposal');
		$query->condition('id', $preference_data->proposal_id);
		$query->range(0, 1);
		$result = $query->execute();
		$proposal_data = $result->fetchObject();
		$user_data = user_load($proposal_data->uid);
		del_book_pdf($preference_data->id);
		if ($ex_data['approved'] == "0")
		{
			/*db_query("UPDATE {textbook_companion_example} SET approval_status = 1, approver_uid = %d, approval_date = %d WHERE id = %d", $user->uid, time(), $ex_data['example_id']);*/
			$query = db_update('textbook_companion_example');
			$query->fields(array(
				'approval_status' => 1,
				'approver_uid' => $user->uid,
				'approval_date' => time()
			));
			$query->condition('id', $ex_data['example_id']);
			$num_updated = $query->execute();
			/* sending email */
			$email_to = $user_data->mail;
			$from = variable_get('textbook_companion_from_email', '');
			$bcc = variable_get('textbook_companion_emails', '');
			$cc = variable_get('textbook_companion_cc_emails', '');
			$params['example_approved']['example_id'] = $ex_data['example_id'];
			$params['example_approved']['user_id'] = $user_data->uid;
			$params['example_approved']['headers'] = array(
				'From' => $from,
				'MIME-Version' => '1.0',
				'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
				'Content-Transfer-Encoding' => '8Bit',
				'X-Mailer' => 'Drupal',
				'Cc' => $cc,
				'Bcc' => $bcc
			);
			if (!drupal_mail('textbook_companion', 'example_approved', $email_to, language_default(), $params, $from, TRUE))
				drupal_set_message('Error sending email message.', 'error');
		} //$ex_data['approved'] == "0"
		else if ($ex_data['approved'] == "1")
		{
			if (delete_example($ex_data['example_id']))
			{
				/* sending email */
				$email_to = $user_data->mail;
				$from = variable_get('textbook_companion_from_email', '');
				$bcc = variable_get('textbook_companion_emails', '');
				$cc = variable_get('textbook_companion_cc_emails', '');
				$params['example_disapproved']['preference_id'] = $chapter_data->preference_id;
				$params['example_disapproved']['chapter_id'] = $example_data->chapter_id;
				$params['example_disapproved']['example_number'] = $example_data->number;
				$params['example_disapproved']['example_caption'] = $example_data->caption;
				$params['example_disapproved']['user_id'] = $user_data->uid;
				$params['example_disapproved']['message'] = $ex_data['message'];
				$params['example_disapproved']['headers'] = array(
					'From' => $from,
					'MIME-Version' => '1.0',
					'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
					'Content-Transfer-Encoding' => '8Bit',
					'X-Mailer' => 'Drupal',
					'Cc' => $cc,
					'Bcc' => $bcc
				);
				if (!drupal_mail('textbook_companion', 'example_disapproved', $email_to, language_default(), $params, $from, TRUE))
					drupal_set_message('Error sending email message.', 'error');
			} //delete_example($ex_data['example_id'])
			else
			{
				drupal_set_message('Error disapproving and deleting example. Please contact administrator.', 'error');
			}
		} //$ex_data['approved'] == "1"
	} //$form_state['values']['example_details'] as $ex_id => $ex_data
	drupal_set_message('Updated successfully.', 'status');
	drupal_goto('code_approval');
}
/******************************************************************************/
/********************************* BULK APPROVAL ******************************/
/******************************************************************************/
function bulk_approval_form($form, &$form_state)
{
	$options_first = _bulk_list_of_books();
	$options_two = _ajax_bulk_get_chapter_list();
	$selected = isset($form_state['values']['book']) ? $form_state['values']['book'] : key($options_first);
	$select_two = isset($form_state['values']['chapter']) ? $form_state['values']['chapter'] : key($options_two);
	$form['book'] = array(
		'#type' => 'select',
		'#title' => t('Title of the Book'),
		'#options' => _bulk_list_of_books(),
		'#default_value' => $selected,
		//'#tree' => TRUE,
		'#ajax' => array(
			'callback' => 'ajax_bulk_chapter_list_callback'
		),
		'#validated' => TRUE
	);
	$form['download_book'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_selected_book"></div>'
	);
	$form['download_pdf'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_selected_book_pdf"></div>'
	);
	$form['regenrate_book'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_selected_book_regenerate_pdf"></div>'
	);
	$form['notes_book'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_selected_book_notes"></div>'
	);
	$form['book_actions'] = array(
		'#type' => 'select',
		'#title' => t('Please select action for selected book'),
		'#options' => _bulk_list_book_actions(),
		//'#default_value' => isset($form_state['values']['lab_actions']) ? $form_state['values']['lab_actions'] : 0,
		'#prefix' => '<div id="ajax_selected_book_action" style="color:red;">',
		'#suffix' => '</div>',
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		),
		'#validated' => TRUE
	);
	$form['chapter'] = array(
		'#type' => 'select',
		'#title' => t('Title of the Chapter'),
		'#options' => _ajax_bulk_get_chapter_list($selected),
		//'#default_value' => $chapter_default_value,
		'#prefix' => '<div id="ajax_select_chapter_list">',
		'#suffix' => '</div>',
		'#validated' => TRUE,
		// '#tree' => TRUE,
		'#ajax' => array(
			'callback' => 'ajax_bulk_example_list_callback'
		),
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		)
	);
	$form['download_chapter'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_download_chapter"></div>'
	);
	$form['chapter_actions'] = array(
		'#type' => 'select',
		'#title' => t('Please select action for selected chapter'),
		'#options' => _bulk_list_chapter_actions(),
		//'#default_value' => isset($form_state['values']['lab_actions']) ? $form_state['values']['lab_actions'] : 0,
		'#prefix' => '<div id="ajax_selected_chapter_action" style="color:red;">',
		'#suffix' => '</div>',
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		),
		'#ajax' => array(
			'callback' => 'ajax_bulk_chapter_actions_callback'
		)
	);
	$form['example'] = array(
		'#type' => 'select',
		'#title' => t('Example No. (Caption)'),
		'#options' => _ajax_bulk_get_examples($chapter_default_value),
		// '#default_value' => $example_default_value,
		'#validated' => TRUE,
		'#prefix' => '<div id="ajax_selected_example">',
		'#suffix' => '</div>',
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		),
		'#ajax' => array(
			'callback' => 'ajax_bulk_example_files_callback'
		)
	);
	$form['download_example'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_download_selected_example"></div>'
	);
	$form['edit_example'] = array(
		'#type' => 'item',
		'#markup' => '<div id="ajax_edit_selected_example"></div>'
	);
	$form['example_files'] = array(
		'#type' => 'item',
		'#markup' => '',
		'#prefix' => '<div id="ajax_example_files_list">',
		'#suffix' => '</div>'
	);
	$form['example_actions'] = array(
		'#type' => 'select',
		'#title' => t('Please select action for selected example'),
		'#options' => _bulk_list_example_actions(),
		//'#default_value' => isset($form_state['values']['lab_actions']) ? $form_state['values']['lab_actions'] : 0,
		'#prefix' => '<div id="ajax_selected_example_action" style="color:red;">',
		'#suffix' => '</div>',
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		)
	);
	$form['message'] = array(
		'#type' => 'textarea',
		'#title' => t('If Dis-Approved please specify reason for Dis-Approval'),
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		)
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
		'#validate' => array(
			'bulk_approval_submit_validate'
		),
		'#states' => array(
			'invisible' => array(
				':input[name="book"]' => array(
					'value' => 0
				)
			)
		)
	);
	return $form;
}
function bulk_approval_submit_validate($form, &$form_state)
{
	if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0))
		drupal_set_message("Wrong selection of action. Please select any one action1", 'error');
	return;
	if (($form_state['values']['book_actions'] != 0) && ($form_state['values']['chapter_actions'] != 0) && ($form_state['values']['example_actions'] != 0))
		drupal_set_message("Wrong selection of action. Please select any one action2");
	return;
	if (($form_state['values']['book_actions'] != 0) && ($form_state['values']['chapter_actions'] != 0) && ($form_state['values']['example_actions'] == 0))
		drupal_set_message("Wrong selection of action. Please select any one action3", 'error');
	return;
	if (($form_state['values']['book_actions'] != 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] != 0))
		drupal_set_message("Wrong selection of action. Please select any one action4", 'error');
	return;
	if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] != 0) && ($form_state['values']['lab_experiment_solution_actions'] != 0))
		drupal_set_message("Wrong selection of action5. Please select any one action", 'error');
	return;
}
function bulk_approval_form_submit($form, &$form_state)
{
	global $user;
	$root_path = textbook_companion_path();

	if ($form_state['clicked_button']['#value'] == 'Submit')
	{
		$book_name_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = :book_id",array(":book_id"=>$form_state['values']['book']));
		$book_name= $book_name_q->fetchObject();
		if ($form_state['values']['book'])
			del_book_pdf($form_state['values']['book']);
		if (user_access('bulk manage code'))
		{
			if (($form_state['values']['book_actions'] == 1) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0))
			{
				/* approving entire book */
				/* $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d",
				$form_state['values']['book']);*/
				$query = db_select('textbook_companion_chapter');
				$query->fields('textbook_companion_chapter');
				$query->condition('preference_id', $form_state['values']['book']);
				$chapter_q = $query->execute();
				while ($chapter_data = $chapter_q->fetchObject())
				{
					/*db_query("UPDATE {textbook_companion_example} SET approval_status = 1, approver_uid = %d WHERE chapter_id = %d AND approval_status = 0", $user->uid, $chapter_data->id);*/
					$query = db_update('textbook_companion_example');
					$query->fields(array(
						'approval_status' => 1,
						'approver_uid' => $user->uid
					));
					$query->condition('chapter_id', $chapter_data->id);
					$query->condition('approval_status', 0);
					$num_updated = $query->execute();
				} //$chapter_data = $chapter_q->fetchObject()
				drupal_set_message(t('Approved Entire Book.'), 'status');
				/* email */
				$email_subject = t('[scilab.in] Your uploaded examples have been approved');
				$email_body = array(
					0 => t('Your all the uploaded examples for the book '. $book_name->book .' have been approved.')
				);
			} //($form_state['values']['book_actions'] == 1) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0)
			else if (($form_state['values']['book_actions'] == 2) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0))
			{
				/* approving entire book */
				/*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d", $form_state['values']['book']);*/
				$query = db_select('textbook_companion_chapter');
				$query->fields('textbook_companion_chapter');
				$query->condition('preference_id', $form_state['values']['book']);
				$result = $query->execute();
				while ($chapter_data = $chapter_q->fetchObject())
				{
					/*db_query("UPDATE {textbook_companion_example} SET approval_status = 0 WHERE chapter_id = %d", $chapter_data->id);*/
					$query = db_update('textbook_companion_example');
					$query->fields(array(
						'approval_status' => 0
					));
					$query->condition('chapter_id', $chapter_data->id);
					$num_updated = $query->execute();
				} //$chapter_data = $chapter_q->fetchObject()
				drupal_set_message(t('Pending Review Entire Book.'), 'status');
				/* email */
				$email_subject = t('[scilab.in] Your uploaded examples have been marked as pending');
				$email_body = array(
					0 => t('Your all the uploaded examples for the book '. $book_name->book .' have been marked as pending to be review. You will be able to see the exmaples after they have been approved by one of our reviewers.')
				);
			} //($form_state['values']['book_actions'] == 2) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0)
			else if (($form_state['values']['book_actions'] == 3) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0))
			{
				if (!user_access('bulk delete code'))
				{
					drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Book.'), 'error');
					return;
				} //!user_access('bulk delete code')
				if (delete_book($form_state['values']['book']))
				{
					drupal_set_message(t('Dis-Approved and Deleted Entire Book.'), 'status');
				} //delete_book($form_state['values']['book'])
				else
				{
					drupal_set_message(t('Error Dis-Approving and Deleting Entire Book.'), 'error');
				}
				/* email */
				$email_subject = t('[scilab.in] Your uploaded examples have been marked as dis-approved');
				$email_body = array(
					0 => t('Your all the uploaded examples for the whole book '. $book_name->book .' have been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['message'])
				);
			} //($form_state['values']['book_actions'] == 3) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0)
			elseif (($form_state['values']['book_actions'] == 4) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0))
			{
				if (!user_access('bulk delete code'))
				{
					drupal_set_message(t('You do not have permission to Bulk Delete Entire Book Including Proposal.'), 'error');
					return;
				} //!user_access('bulk delete code')
				/* check if dependency files are present */
				/*$dep_q = db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE preference_id = %d", $form_state['values']['book']);*/
				/*	$query = db_select('textbook_companion_dependency_files');
				$query->fields('textbook_companion_dependency_files');
				$query->condition('preference_id', $form_state['values']['book']);
				$dep_q = $query->execute();

				if ($dep_data =$dep_q->fetchObject())
				{
				drupal_set_message(t("Cannot delete book since it has dependency files that can be used by others. First delete the dependency files before deleing the Book."), 'error');
				return;
				}
				*/
				if (delete_book($form_state['values']['book']))
				{
					drupal_set_message(t('Dis-Approved and Deleted Entire Book examples.'), 'status');
					$dir_path = $root_path . $form_state['values']['book'];
					if (is_dir($dir_path))
					{
						$res = rmdir($dir_path);
						if (!$res)
						{
							drupal_set_message(t("Cannot delete Book directory : " . $dir_path . ". Please contact administrator."), 'error');
							return;
						} //!$res
					} //is_dir($dir_path)
					else
					{
						drupal_set_message(t("Book directory not present : " . $dir_path . ". Skipping deleting book directory."), 'status');
					}
					/* deleting preference and proposal */
					/*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $form_state['values']['book']);
					$preference_data = db_fetch_object($preference_q);
					*/
					$query = db_select('textbook_companion_preference');
					$query->fields('textbook_companion_preference');
					$query->condition('id', $form_state['values']['book']);
					$result = $query->execute();
					$preference_data = $result->fetchObject();
					$book_name = $preference_data->book;
					$proposal_id = $preference_data->proposal_id;

					drupal_set_message(t('Deleted Book Proposal.'), 'status');
					/* email */
					$email_subject = t('[scilab.in] Your uploaded examples including the book '. $book_name->book .' proposal have been deleted');
					$email_body = array(
						0 => t('Your all the uploaded examples including the book have been deleted permanently.

Reason for deletion:

' . $form_state['values']['message'])
					);
					/*db_query("DELETE FROM {textbook_companion_preference} WHERE proposal_id = %d", $proposal_id);*/
					$query = db_delete('textbook_companion_preference');
					$query->condition('proposal_id', $proposal_id);
					$num_deleted = $query->execute();

					/*db_query("DELETE FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);*/
					$query = db_delete('textbook_companion_proposal');
					$query->condition('id', $proposal_id);
					$num_deleted = $query->execute();
				} //delete_book($form_state['values']['book'])
				else
				{
					drupal_set_message(t('Error Dis-Approving and Deleting Entire Book.'), 'error');
				}
			} //($form_state['values']['book_actions'] == 4) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 0)
			elseif (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 1) && ($form_state['values']['example_actions'] == 0))
			{
				/*db_query("UPDATE {textbook_companion_example} SET approval_status = 1, approver_uid = %d WHERE chapter_id = %d AND approval_status = 0", $user->uid, $form_state['values']['chapter']);*/
				$query = db_update('textbook_companion_example');
				$query->fields(array(
					'approval_status' => 1,
					'approver_uid' => $user->uid
				));
				$query->condition('chapter_id', $form_state['values']['chapter']);
				$query->condition('approval_status', 0);
				$num_updated = $query->execute();
				drupal_set_message(t('Approved Entire Chapter.'), 'status');
				/* email */
				$email_subject = t('[scilab.in] Your uploaded examples have been approved');
				$email_body = array(
					0 => t('Your all the uploaded examples for the chapter have been approved.')
				);
			} //($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 1) && ($form_state['values']['example_actions'] == 0)
			else if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 2) && ($form_state['values']['example_actions'] == 0))
			{
				/*db_query("UPDATE {textbook_companion_example} SET approval_status = 0 WHERE chapter_id = %d", $form_state['values']['chapter']);*/
				$query = db_update('textbook_companion_example');
				$query->fields(array(
					'approval_status' => 0
				));
				$query->condition('chapter_id', $form_state['values']['chapter']);
				$num_updated = $query->execute();
				drupal_set_message(t('Entire Chapter marked as Pending Review.'), 'status');
				/* email */
				$email_subject = t('[scilab.in] Your uploaded examples have been marked as pending');
				$email_body = array(
					0 => t('Your all the uploaded examples for the chapter have been marked as pending to be review.')
				);
			} //($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 2) && ($form_state['values']['example_actions'] == 0)
			else if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 3) && ($form_state['values']['example_actions'] == 0))
			{
				if (!user_access('bulk delete code'))
				{
					drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Chapter.'), 'error');
					return;
				} //!user_access('bulk delete code')
				if (delete_chapter($form_state['values']['chapter']))
				{
					drupal_set_message(t('Dis-Approved and Deleted Entire Chapter.'), 'status');
				} //delete_chapter($form_state['values']['chapter'])
				else
				{
					drupal_set_message(t('Error Dis-Approving and Deleting Entire Chapter.'), 'error');
				}
				/* email */
				$email_subject = t('[scilab.in] Your uploaded example have been marked as dis-approved');
				$email_body = array(
					0 => t('Your uploaded example for the entire chapter have been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['message'])
				);
			} //($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 3) && ($form_state['values']['example_actions'] == 0)
			else if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 1))
			{
				/*db_query("UPDATE {textbook_companion_example} SET approval_status = 1, approver_uid = %d WHERE id = %d", $user->uid, $form_state['values']['example']);*/
				$query = db_update('textbook_companion_example');
				$query->fields(array(
					'approval_status' => 1,
					'approver_uid' => $user->uid
				));
				$query->condition('id', $form_state['values']['example']);
				$num_updated = $query->execute();
				drupal_set_message(t('Example approved.'), 'status');
				/* email */
				$email_subject = t('[scilab.in] Your uploaded example has been approved');
				$email_body = array(
					0 => t('Your uploaded example has been approved.')
				);
			} //($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 1)
			else if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 2))
			{
				/*db_query("UPDATE {textbook_companion_example} SET approval_status = 0 WHERE id = %d", $form_state['values']['example']);*/
				$query = db_update('textbook_companion_example');
				$query->fields(array(
					'approval_status' => 0
				));
				$query->condition('id', $form_state['values']['example']);
				$num_updated = $query->execute();
				drupal_set_message(t('Example marked as Pending Review.'), 'status');
				/* email */
				$email_subject = t('[scilab.in] Your uploaded example has been marked as pending');
				$email_body = array(
					0 => t('Your uploaded example has been marked as pending to be review.')
				);
			} //($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 2)
			else if (($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 3))
			{
				if (delete_example($form_state['values']['example']))
				{
					drupal_set_message(t('Example Dis-Approved and Deleted.'), 'status');
				} //delete_example($form_state['values']['example'])
				else
				{
					drupal_set_message(t('Error Dis-Approving and Deleting Example.'), 'error');
				}
				/* email */
				$email_subject = t('[scilab.in] Your uploaded example has been marked as dis-approved');
				$email_body = array(
					0 => t('Your uploaded example has been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['message'])
				);
			} //($form_state['values']['book_actions'] == 0) && ($form_state['values']['chapter_actions'] == 0) && ($form_state['values']['example_actions'] == 3)
			else if($form_state['values']['book_actions'] == 5)
			{
				db_query('UPDATE textbook_companion_preference SET submited_all_examples_code = 0 WHERE id = :preference_id', array(
					':preference_id' => $form_state['values']['book']
				));
				/* sending email */
				// $query                = ("SELECT proposal_id FROM textbook_companion_preference WHERE id= :preference_id");
				// $args                 = array(
				// 	":preference_id" => $form_state['values'][':preference_id']
				// );
				// $proposal_data        = db_query($query, $args);
				// $proposal_data_result = $proposal_data->fetchObject();
				//
				// $email_to                                   = $user->mail;
				// $from                                       = variable_get('textbook_companion_from_email', '');
				// $bcc                                        = variable_get('textbook_companion_emails', '');
				// $cc                                         = variable_get('textbook_companion_cc_emails', '');
				// $params['all_code_submitted']['proposal_id'] = $proposal_data_result->proposal_id;
				// $params['all_code_submitted']['user_id']     = $user->uid;
				// $params['all_code_submitted']['headers']     = array(
				// 	'From' => $from,
				// 	'MIME-Version' => '1.0',
				// 	'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
				// 	'Content-Transfer-Encoding' => '8Bit',
				// 	'X-Mailer' => 'Drupal',
				// 	'Cc' => $cc,
				// 	'Bcc' => $bcc
				// );
				// if (!drupal_mail('textbook_companion', 'all_code_submitted', $email_to, language_default(), $params, $from, TRUE))
				// 	drupal_set_message('Error sending email message.', 'error');
				drupal_set_message('Book is unlocked for code submission.', 'status');
			}
			/****** sending email when everything done ******/
			if ($email_subject)
			{
				$email_to = $user->mail;
				$from = variable_get('textbook_companion_from_email', '');
				$bcc = variable_get('textbook_companion_emails', '');
				$cc = variable_get('textbook_companion_cc_emails', '');
				//$params['proposal_disapproved']['proposal_id'] = $proposal_id;
				//$params['proposal_disapproved']['user_id'] = $row->uid;
				$params['standard']['headers'] = array(
				'From' => $from,
				'MIME-Version' => '1.0',
				'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
				'Content-Transfer-Encoding' => '8Bit',
				'X-Mailer' => 'Drupal',
				'Cc' => $cc,
				'Bcc' => $bcc
		);
				$params['standard']['subject'] = $email_subject;
				$params['standard']['body'] = $email_body;
				if (!drupal_mail('textbook_companion', 'standard', $email_to, language_default(), $params, variable_get('textbook_companion_from_email', NULL), TRUE))
					drupal_set_message('Error sending email message.', 'error');
			} //$email_subject
		} //user_access('bulk manage code')
		else
		{
			drupal_set_message(t('You do not have permission to bulk manage code.'), 'error');
		}
	} //$form_state['clicked_button']['#value'] == 'Submit'
}
function _bulk_list_of_books()
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
function _ajax_bulk_get_chapter_list($preference_id = 0)
{
	$book_chapters = array(
		'0' => 'Please select...'
	);
	/*$book_chapters_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_id);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('preference_id', $preference_id);
	$query->orderBy('number', 'ASC');
	$book_chapters_q = $query->execute();
	while ($book_chapters_data = $book_chapters_q->fetchObject())
	{
		$book_chapters[$book_chapters_data->id] = $book_chapters_data->number . '. ' . $book_chapters_data->name;
	} //$book_chapters_data = $book_chapters_q->fetchObject()
	return $book_chapters;
}
function _ajax_bulk_get_examples($chapter_id = 0)
{
	$book_examples = array(
		'0' => 'Please select...'
	);
	/*$book_examples_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d ORDER BY
	CAST(SUBSTRING_INDEX(number, '.', 1) AS BINARY) ASC,
	CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', 2), '.', -1) AS UNSIGNED) ASC,
	CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', -1), '.', 1) AS UNSIGNED) ASC", $chapter_id);*/
	$query = db_select('textbook_companion_example');
	$query->fields('textbook_companion_example');
	$query->condition('chapter_id', $chapter_id);
	//$query->orderBy('CAST', 'ASC');
	//$query->orderBy('CAST', 'ASC');
	//$query->orderBy('CAST', 'ASC');
	$book_examples_q = $query->execute();
	while ($book_examples_data = $book_examples_q->fetchObject())
	{
		$book_examples[$book_examples_data->id] = $book_examples_data->number . ' (' . $book_examples_data->caption . ')';
	} //$book_examples_data = $book_examples_q->fetchObject()
	return $book_examples;
}
function _bulk_list_book_actions()
{
	$book_actions = array(
		0 => 'Please select...'
	);
	$book_actions[1] = 'Approve Entire Book';
	$book_actions[2] = 'Pending Review Entire Book';
	$book_actions[3] = 'Dis-Approve Entire Book (This will delete all the examples in the book)';
	$book_actions[4] = 'Delete Entire Book Including Proposal';
	$book_actions[5] = 'Unlock the Book for code submission';
	return $book_actions;
}
function _bulk_list_chapter_actions()
{
	$chapter_actions = array(
		0 => 'Please select...'
	);
	$chapter_actions[1] = 'Approve Entire Chapter';
	$chapter_actions[2] = 'Pending Review Entire Chapter';
	$chapter_actions[3] = 'Dis-Approve Entire Chapter (This will delete all the examples in the chapter)';
	return $chapter_actions;
}
function _bulk_list_example_actions()
{
	$example_actions = array(
		0 => 'Please select...'
	);
	$example_actions[1] = 'Approve  Approve Example';
	$example_actions[2] = 'Pending Review Example';
	$example_actions[3] = 'Dis-approve Example (This will delete the example)';
	return $example_actions;
}
/****************************** Ajax Callback function ***************************/
function ajax_bulk_chapter_list_callback($form, $form_state)
{
	$commands = array();
	$book_default_value = $form_state['values']['book'];
	if ($book_default_value > 0)
	{
		$commands[] = ajax_command_html('#ajax_selected_book', l('Download', 'full_download/book/' . $book_default_value) . ' ' . t('(Download all the approved and unapproved examples of the entire book)'));
		$commands[] = ajax_command_html('#ajax_selected_book_pdf', l('Download PDF', 'textbook_companion/generate_book/' . $book_default_value . '/1') . ' ' . t('(Download PDF of all the approved and unapproved examples of the entire book)'));
		$commands[] = ajax_command_html('#ajax_selected_book_regenerate_pdf', l('Regenerate PDF', 'textbook_companion/delete_book/' . $book_default_value) . ' ' . t('(Manually Regenerate PDF of the entire book)'));
		$commands[] = ajax_command_html('#ajax_selected_book_notes', l('Notes for Reviewers', 'code_approval/notes/' . $book_default_value));
		$form['book_actions']['#options'] = _bulk_list_book_actions();
		$commands[] = ajax_command_replace('#ajax_selected_book_action', drupal_render($form['book_actions']));
		$form['chapter']['#options'] = _ajax_bulk_get_chapter_list($book_default_value);
		$commands[] = ajax_command_replace('#ajax_select_chapter_list', drupal_render($form['chapter']));
		$commands[] = ajax_command_html('#ajax_download_chapter', '');
		$form['chapter_actions']['#options'] = _bulk_list_book_actions();
		$commands[] = ajax_command_replace('#ajax_selected_chapter_action', drupal_render($form['chapter_actions']));
		$commands[] = ajax_command_html('#ajax_selected_chapter_action', '');
		$commands[] = ajax_command_html('#ajax_selected_example', '');
		$form['example_actions']['#options'] = _bulk_list_example_actions();
		$commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
		$commands[] = ajax_command_html('#ajax_selected_example_action', '');
		$commands[] = ajax_command_html('#ajax_download_selected_example', '');
		$commands[] = ajax_command_html('#ajax_edit_selected_example', '');
		$form['example_files']['#title'] = '';
		$form['example_files']['#markup'] = '';
		$commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
	} //$book_default_value > 0
	else
	{
		$commands[] = ajax_command_html('#ajax_selected_book', '');
		$commands[] = ajax_command_html('#ajax_selected_book_pdf', '');
		$commands[] = ajax_command_html('#ajax_selected_book_regenerate_pdf', '');
		$commands[] = ajax_command_html('#ajax_selected_book_notes', '');
		$form['chapter']['#options'] = _ajax_bulk_get_chapter_list();
		$commands[] = ajax_command_replace('#ajax_select_chapter_list', drupal_render($form['chapter']));
		$commands[] = ajax_command_html('#ajax_select_chapter_list', '');
		$form['book_actions']['#options'] = _bulk_list_book_actions();
		$commands[] = ajax_command_replace('#ajax_selected_book_action', drupal_render($form['book_actions']));
		$commands[] = ajax_command_html('#ajax_selected_book_action', '');
		$form['chapter_actions']['#options'] = _bulk_list_chapter_actions();
		$commands[] = ajax_command_replace('#ajax_selected_chapter_action', drupal_render($form['chapter_actions']));
		$commands[] = ajax_command_html('#ajax_selected_chapter_action', '');
		$commands[] = ajax_command_html('#ajax_download_chapter', '');
		$commands[] = ajax_command_html('#ajax_selected_example', '');
		$form['example_actions']['#options'] = _bulk_list_example_actions();
		$commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
		$commands[] = ajax_command_html('#ajax_selected_example_action', '');
		$commands[] = ajax_command_html('#ajax_download_selected_example', '');
		$commands[] = ajax_command_html('#ajax_edit_selected_example', '');
		$form['example_files']['#title'] = '';
		$form['example_files']['#markup'] = '';
		$commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
function ajax_bulk_example_list_callback($form, $form_state)
{
	$commands = array();
	$chapter_default_value = $form_state['values']['chapter'];
	if ($chapter_default_value > 0)
	{
		$commands[] = ajax_command_html('#ajax_download_chapter', l('Download', 'full_download/chapter/' . $chapter_default_value) . ' ' . t('(Download all the approved and unapproved examples of the entire chapter)'));
		$form['chapter_actions']['#options'] = _bulk_list_chapter_actions();
		$commands[] = ajax_command_replace('#ajax_selected_chapter_action', drupal_render($form['chapter_actions']));
		$form['example']['#options'] = _ajax_bulk_get_examples($chapter_default_value);
		$commands[] = ajax_command_replace('#ajax_selected_example', drupal_render($form['example']));
		$commands[] = ajax_command_html('#ajax_download_selected_example', '');
		$commands[] = ajax_command_html('#ajax_edit_selected_example', '');
		$form['example_actions']['#options'] = _bulk_list_example_actions();
		$commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
		$commands[] = ajax_command_html('#ajax_selected_example_action', '');
		$form['example_files']['#title'] = '';
		$form['example_files']['#markup'] = '';
		$commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
	} //$chapter_default_value > 0
	else
	{
		$commands[] = ajax_command_html('#ajax_download_chapter', '');
		$form['chapter_actions']['#options'] = _bulk_list_chapter_actions();
		$commands[] = ajax_command_replace('#ajax_selected_chapter_action', drupal_render($form['chapter_actions']));
		$commands[] = ajax_command_html('#ajax_selected_chapter_action', '');
		$form['example']['#options'] = _ajax_bulk_get_examples();
		$commands[] = ajax_command_replace('#ajax_selected_example', drupal_render($form['example']));
		$commands[] = ajax_command_html('#ajax_selected_example', '');
		$commands[] = ajax_command_html('#ajax_download_selected_example', '');
		$commands[] = ajax_command_html('#ajax_edit_selected_example', '');
		$form['example_files']['#title'] = '';
		$form['example_files']['#markup'] = '';
		$commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
		$form['example_actions']['#options'] = _bulk_list_example_actions();
		$commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
		$commands[] = ajax_command_html('#ajax_selected_example_action', '');
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
function ajax_bulk_example_files_callback($form, $form_state)
{
	$commands = array();
	$example_list_default_value = $form_state['values']['example'];
	//var_dump($example_list_default_value);
	if ($example_list_default_value > 0)
	{
		/*************************************************************************************/
		/*$example_list_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $form_state['values']['example']);*/
		$query = db_select('textbook_companion_example_files');
		$query->fields('textbook_companion_example_files');
		$query->condition('example_id', $example_list_default_value);
		$example_list_q = $query->execute();
		if ($example_list_q)
		{
			$example_files_rows = array();
			while ($example_list_data = $example_list_q->fetchObject())
			{
				$example_file_type = '';
				switch ($example_list_data->filetype)
				{
					case 'S':
						$example_file_type = 'Source or Main file';
						break;
					case 'R':
						$example_file_type = 'Result file';
						break;
					case 'X':
						$example_file_type = 'xcos file';
						break;
					default:
						$example_file_type = 'Unknown';
						break;
				} //$example_list_data->filetype
				$example_files_rows[] = array(
					l($example_list_data->filename, 'download/file/' . $example_list_data->id),
					$example_file_type
				);
			} //$example_list_data = $example_list_q->fetchObject()
			/* dependency files */
			/*$dependency_list_q = db_query("SELECT dependency.id as dependency_id, dependency.filename as dependency_filename, dependency.caption as dependency_caption
			FROM {textbook_companion_example_dependency} example_dependency LEFT JOIN {textbook_companion_dependency_files} dependency
			ON example_dependency.dependency_id = dependency.id
			WHERE example_dependency.example_id = %d", $form_state['values']['example']);*/
			$query = db_select('textbook_companion_example_dependency', 'example_dependency');
			$query->fields('dependency', array(
				'id',
				'filename',
				'caption'
			));
			$query->addField('dependency', 'id', 'dependency_id');
			$query->addField('dependency', 'filename', 'dependency_filename');
			$query->addField('dependency', 'caption', 'dependency_caption');
			$query->leftJoin('textbook_companion_dependency_files', 'dependency', 'example_dependency.dependency_id = dependency.id');
			$query->condition('example_dependency.example_id', $form_state['values']['example']);
			$dependency_list_q = $query->execute();
			while ($dependency_list_data = $dependency_list_q->fetchObject())
			{
				$example_file_type = 'Dependency file';
				$temp_caption = '';
				if ($dependency_list_data->dependency_caption)
					$temp_caption = ' (' . $dependency_list_data->dependency_caption . ')';
				$example_files_rows[] = array(
					l($dependency_list_data->dependency_filename, 'download/dependency/' . $dependency_list_data->dependency_id) . $temp_caption,
					$example_file_type
				);
			} //$dependency_list_data = $dependency_list_q->fetchObject()
			/* creating list of files table */
			$example_files_header = array(
				'Filename',
				'Type'
			);
			$example_files = theme('table', array(
				'header' => $example_files_header,
				'rows' => $example_files_rows
			));
			$form['example_files']['#title'] = 'List of example files';
			$form['example_files']['#markup'] = $example_files;
			$commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
			$commands[] = ajax_command_html('#ajax_download_selected_example', l('Download Example', 'download/example/' . $example_list_default_value));
			$commands[] = ajax_command_html('#ajax_edit_selected_example', l('Edit Example', 'code_approval/editcode/' . $example_list_default_value));
			$form['example_actions']['#options'] = _bulk_list_example_actions();
			$commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
			//$commands[] = ajax_command_html('#ajax_selected_example_action', '' );
		} //$example_list_q
	} //$example_list_default_value > 0
	else
	{
		$commands[] = ajax_command_html('#ajax_download_selected_example', '');
		$commands[] = ajax_command_html('#ajax_edit_selected_example', '');
		$form['example_files']['#title'] = '';
		$form['example_files']['#markup'] = '';
		$commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
		$form['example_actions']['#options'] = _bulk_list_example_actions();
		$commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
		$commands[] = ajax_command_html('#ajax_selected_example_action', '');
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
function ajax_bulk_chapter_actions_callback()
{
	//if($form_state['values']['chapter_actions'] > 0){
	//	$form['book_actions']['#options'] = _bulk_list_book_actions();
	//$commands[] = ajax_command_replace('#ajax_selected_book_action',drupal_render($form['book_actions']));
	//	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
