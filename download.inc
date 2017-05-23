<?php
// $Id$
function textbook_companion_download_example_file()
{
	$example_file_id = arg(2);
	$root_path = textbook_companion_path();
	/*$example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE id = %d LIMIT 1", $example_file_id);
	$example_file_data = db_fetch_object($example_files_q);*/
	$query = db_select('textbook_companion_example_files');
	$query->fields('textbook_companion_example_files');
	$query->condition('id', $example_file_id);
	$query->range(0, 1);
	$result = $query->execute();
	$example_file_data = $result->fetchObject();
	header('Content-Type: ' . $example_file_data->filemime);
	header('Content-disposition: attachment; filename="' . $example_file_data->filename . '"');
	header('Content-Length: ' . filesize($root_path . $example_file_data->filepath));
	readfile($root_path . $example_file_data->filepath);
}
function textbook_companion_download_dependency_file()
{
	$dependency_file_id = arg(2);
	$root_path = textbook_companion_path();
	/*$dependency_file_q = db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE id = %d LIMIT 1", $dependency_file_id);
	$dependency_file_data = db_fetch_object($dependency_file_q);*/
	$query = db_select('textbook_companion_dependency_files');
	$query->fields('textbook_companion_dependency_files');
	$query->condition('id', $dependency_file_id);
	$query->range(0, 1);
	$result = $query->execute();
	$dependency_file_data = $result->fetchObject();
	header('Content-Type: ' . $dependency_file_data->filemime);
	header('Content-disposition: attachment; filename="' . $dependency_file_data->filename . '"');
	header('Content-Length: ' . filesize($root_path . $dependency_file_data->filepath));
	ob_clean();
	readfile($root_path . $dependency_file_data->filepath);
	exit;
}
function textbook_companion_download_example()
{
	$example_id = arg(2);
	$root_path = textbook_companion_path();
	/* get example data */
	/*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d", $example_id);
	$example_data = db_fetch_object($example_q);*/
	$query = db_select('textbook_companion_example');
	$query->fields('textbook_companion_example');
	$query->condition('id', $example_id);
	$result = $query->execute();
	$example_data = $result->fetchObject();
	/*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
	$chapter_data = db_fetch_object($chapter_q);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('id', $example_data->chapter_id);
	$result = $query->execute();
	$chapter_data = $result->fetchObject();
	/*$example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);*/
	$query = db_select('textbook_companion_example_files');
	$query->fields('textbook_companion_example_files');
	$query->condition('example_id', $example_id);
	$example_files_q = $query->execute();
	/*$example_dependency_files_q = db_query("SELECT * FROM {textbook_companion_example_dependency} WHERE example_id = %d", $example_id);*/
	$query = db_select('textbook_companion_example_dependency');
	$query->fields('textbook_companion_example_dependency');
	$query->condition('example_id', $example_id);
	$example_dependency_files_q = $query->execute();
	$EX_PATH = 'EX' . $example_data->number . '/';
	/* zip filename */
	$zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';
	/* creating zip archive on the server */
	$zip = new ZipArchive();
	$zip->open($zip_filename, ZipArchive::CREATE);
	while ($example_files_row = $example_files_q->fetchObject())
	{
		$zip->addFile($root_path . $example_files_row->filepath, $EX_PATH . $example_files_row->filename);
	} //$example_files_row = $example_files_q->fetchObject()
	/* dependency files */
	while ($example_dependency_files_row = $example_dependency_files_q->fetchObject())
	{
		/*$dependency_file_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE id = %d LIMIT 1", $example_dependency_files_row->dependency_id));*/
		$query = db_select('textbook_companion_dependency_files');
		$query->fields('textbook_companion_dependency_files');
		$query->condition('id', $example_dependency_files_row->dependency_id);
		$query->range(0, 1);
		$result = $query->execute();
		$dependency_file_data = $result->fetchObject();
		if ($dependency_file_data)
			$zip->addFile($root_path . $dependency_file_data->filepath, $EX_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
	} //$example_dependency_files_row = $example_dependency_files_q->fetchObject()
	$zip_file_count = $zip->numFiles;
	$zip->close();
	if ($zip_file_count > 0)
	{
		/* download zip file */
		header('Content-Type: application/zip');
		header('Content-disposition: attachment; filename="EX' . $example_data->number . '.zip"');
		header('Content-Length: ' . filesize($zip_filename));
		ob_end_flush();
		ob_clean();
		flush();
		readfile($zip_filename);
		unlink($zip_filename);
	} //$zip_file_count > 0
	else
	{
		drupal_set_message("There are no files in this examples to download", 'error');
		drupal_goto('textbook_run');
	}
}
function textbook_companion_download_chapter()
{
	$chapter_id = arg(2);
	// var_dump($chapter_id);die;
	$root_path = textbook_companion_path();
	/* get example data */
	/*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $chapter_id);
	$chapter_data = db_fetch_object($chapter_q);*/
	$query = db_select('textbook_companion_chapter');
	$query->fields('textbook_companion_chapter');
	$query->condition('id', $chapter_id);
	$result = $query->execute();
	$chapter_data = $result->fetchObject();
	$CH_PATH = 'CH' . $chapter_data->number . '/';
	/* zip filename */
	$zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';
	/* creating zip archive on the server */
	$zip = new ZipArchive();
	$zip->open($zip_filename, ZipArchive::CREATE);
	/*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND approval_status = 1", $chapter_id);*/
	$query = db_select('textbook_companion_example');
	$query->fields('textbook_companion_example');
	$query->condition('chapter_id', $chapter_id);
	$query->condition('approval_status', 1);
	$example_q = $query->execute();
	while ($example_row = $example_q->fetchObject())
	{
		$EX_PATH = 'EX' . $example_row->number . '/';
		/*$example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_row->id);*/
		$query = db_select('textbook_companion_example_files');
		$query->fields('textbook_companion_example_files');
		$query->condition('example_id', $example_row->id);
		$example_files_q = $query->execute();
		/*$example_dependency_files_q = db_query("SELECT * FROM {textbook_companion_example_dependency} WHERE example_id = %d", $example_row->id);*/
		$query = db_select('textbook_companion_example_dependency');
		$query->fields('textbook_companion_example_dependency');
		$query->condition('example_id', $example_row->id);
		$example_dependency_files_q = $query->execute();
		while ($example_files_row = $example_files_q->fetchObject())
		{
			$zip->addFile($root_path . $example_files_row->filepath, $CH_PATH . $EX_PATH . $example_files_row->filename);
		} //$example_files_row = $example_files_q->fetchObject()
		/* dependency files */
		while ($example_dependency_files_row = $example_dependency_files_q->fetchObject())
		{
			/*$dependency_file_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE id = %d LIMIT 1", $example_dependency_files_row->dependency_id));*/
			$query = db_select('textbook_companion_dependency_files');
			$query->fields('textbook_companion_dependency_files');
			$query->condition('id', $example_dependency_files_row->dependency_id);
			$query->range(0, 1);
			$result = $query->execute();
			$dependency_file_data = $result->fetchObject();
			if ($dependency_file_data)
				$zip->addFile($root_path . $dependency_file_data->filepath, $CH_PATH . $EX_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
		} //$example_dependency_files_row = $example_dependency_files_q->fetchObject()
	} //$example_row = $example_q->fetchObject()
	$zip_file_count = $zip->numFiles;
	$zip->close();
	if ($zip_file_count > 0)
	{
		/* download zip file */
		header('Content-Type: application/zip');
		header('Content-disposition: attachment; filename="CH' . $chapter_data->number . '.zip"');
		header('Content-Length: ' . filesize($zip_filename));
		ob_end_flush();
		ob_clean();
		flush();
		readfile($zip_filename);
		unlink($zip_filename);
	} //$zip_file_count > 0
	else
	{
		drupal_set_message("There are no examples in this chapter to download", 'error');
		drupal_goto('textbook_run');
	}
}
function textbook_companion_download_book()
{
	$book_id = arg(2);
	$root_path = textbook_companion_path();
	/* get example data */
	$book_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = :book_id", array(
		':book_id' => $book_id
	));
	$book_data = $book_q->fetchObject();
	$zipname = str_replace(' ', '_', ($book_data->book));
	$BK_PATH = $zipname . '/';
	/* zip filename */
	$zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';
	/* creating zip archive on the server */
	$zip = new ZipArchive;
	$zip->open($zip_filename, ZipArchive::CREATE);
	$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = :book_id", array(
		':book_id' => $book_id
	));
	while ($chapter_row = $chapter_q->fetchObject())
	{
		$CH_PATH = 'CH' . $chapter_row->number . '/';
		$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = :chapter_id AND approval_status = 1", array(
			':chapter_id' => $chapter_row->id
		));
		while ($example_row = $example_q->fetchObject())
		{
			$EX_PATH = 'EX' . $example_row->number . '/';
			$example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = :id", array(
				':id' => $example_row->id
			));
			$example_dependency_files_q = db_query("SELECT * FROM {textbook_companion_example_dependency} WHERE example_id = :id", array(
				':id' => $example_row->id
			));
			while ($example_files_row = $example_files_q->fetchObject())
			{
				$zip->addFile($root_path . $example_files_row->filepath, $BK_PATH . $CH_PATH . $EX_PATH . $example_files_row->filename);
			} //$example_files_row = $example_files_q->fetchObject()
			/* dependency files */
			while ($example_dependency_files_row = $example_dependency_files_q->fetchObject())
			{
				$query = db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE id = :id LIMIT 1", array(
					':id' => $example_dependency_files_row->dependency_id
				));
				$dependency_file_data = $query->fetchObject();
				if ($dependency_file_data)
					$zip->addFile($root_path . $dependency_file_data->filepath, $BK_PATH . $CH_PATH . $EX_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
			} //$example_dependency_files_row = $example_dependency_files_q->fetchObject()
			$query = "SELECT * FROM {textbook_companion_dependency_files} WHERE preference_id = :preference_id";
			$args = array(
				':preference_id' => $book_id
			);
			$result = db_query($query, $args);
			while ($row = $result->fetchObject())
			{
				$zip->addFile($root_path . $row->filepath, $BK_PATH . 'DEPENDENCIES/' . $row->filename);
			} //$row = $result->fetchObject()
		} //$example_row = $example_q->fetchObject()
	} //$chapter_row = $chapter_q->fetchObject()
	$zip_file_count = $zip->numFiles;
	$zip->close();
	if ($zip_file_count > 0)
	{
		/* download zip file */
		global $user;
		if ($user->uid)
		{
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', ($book_data->book)) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			ob_clean();
			readfile($zip_filename);
			unlink($zip_filename);
		} //$user->uid
		else
		{
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', ($book_data->book)) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			header("Content-Transfer-Encoding: binary");
			header('Expires: 0');
			header('Pragma: no-cache');
			ob_end_flush();
			ob_clean();
			flush();
			readfile($zip_filename);
			unlink($zip_filename);
		}
	} //$zip_file_count > 0
	else
	{
		drupal_set_message("There are no examples in this book to download", 'error');
		drupal_goto('textbook_run');
	}
}
function textbook_companion_download_sample_code()
{
	$proposal_id = arg(2);
	$root_path = textbook_companion_samplecode_path();
	$query = db_select('textbook_companion_proposal');
	$query->fields('textbook_companion_proposal');
	$query->condition('id', $proposal_id);
	$query->range(0, 1);
	$result = $query->execute();
	$example_file_data = $result->fetchObject();
	$samplecodename = substr($example_file_data->samplefilepath, strrpos($example_file_data->samplefilepath, '/') + 1);
	header('Content-Type: application/zip');
	header('Content-disposition: attachment; filename="' . $samplecodename . '"');
	header('Content-Length: ' . filesize($root_path . $example_file_data->samplefilepath));
	ob_clean();
	readfile($root_path . $example_file_data->samplefilepath);
}
   
