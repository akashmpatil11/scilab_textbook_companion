<?php
// $Id$
function list_chapters()
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
	while ($chapter_data = $chapter_q->fetchObject())
	{
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
			$chapter_data->name . ' (' . l('Edit', 'textbook_companion/code/chapter/edit/' . $chapter_data->id) . '|'.l('Delete', 'textbook_companion/code/chapter/delete/'. $chapter_data->id, array(
						'attributes' => array(
							'onClick' => 'return confirm("Are you sure you want to delete the Chapter? Once you delete chapter, all the codes related to chapter will be deleted.")'
						)
					)).')',
			$example_data->example_count,
			l('View', 'textbook_companion/code/list_examples/' . $chapter_data->id)
		);
	} //$chapter_data = $chapter_q->fetchObject()
	/* check if there are any chapters */
	if (!$chapter_rows)
	{
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
	));
	return $return_html;
}
function list_examples()
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
	/* get chapter details */
	$chapter_id = arg(3);
	/*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d LIMIT 1", $chapter_id, $preference_data->id);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('id', $chapter_id);
	$query->condition('preference_id', $preference_data->id);
	$query->range(0, 1);
	$chapter_q = $query->execute();
	if ($chapter_data = $chapter_q->fetchObject())
	{
		$return_html = '<br />';
		$return_html .= '<strong>Title of the Book:</strong><br />' . $preference_data->book . '<br /><br />';
		$return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->full_name . '<br /><br />';
		$return_html .= '<strong>Chapter Number:</strong><br />' . $chapter_data->number . '<br /><br />';
		$return_html .= '<strong>Title of the Chapter:</strong><br />' . $chapter_data->name . '<br />';
	} //$chapter_data = $chapter_q->fetchObject()
	else
	{
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
	while ($example_data = $example_q->fetchObject())
	{
		/* approval status */
		$approval_status = '';
		switch ($example_data->approval_status)
		{
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
		while ($example_files_data = $example_files_q->fetchObject())
		{
			$file_type = '';
			switch ($example_files_data->filetype)
			{
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
		while ($dependency_files_data = $dependency_files_q->fetchObject())
		{
			$dependency_files .= l($dependency_files_data->dependency_filename, 'download/dependency/' . $dependency_files_data->dependency_id) . ' (Dependency)<br />';
		} //$dependency_files_data = $dependency_files_q->fetchObject()
		$example_files .= $dependency_files;
		if ($example_data->approval_status == 0)
		{
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
		else
		{
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
