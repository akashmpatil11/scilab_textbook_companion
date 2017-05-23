<?php
/* function to display books in progress */
function tbc_books_in_progress_all()
{
	$output = "";
	$query = db_select('list_of_category');
	$query->fields('list_of_category');
	$query->orderBy('id', 'ASC');
	$category_list = $query->execute();
	$query = "
  SELECT po.creation_date, pe.book as book, pe.author as author, pe.publisher as publisher,pe.edition as edition, pe.isbn as isbn, pe.year as year, pe.id as pe_id, loc.category_name as category, loc.category_id as cat_id
FROM {textbook_companion_preference} pe 
LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id
LEFT JOIN list_of_category loc on pe.category = loc.category_id
WHERE po.proposal_status IN (1,4) AND pe.approval_status = 1
ORDER BY po.creation_date DESC
  ";
	$result = db_query($query);
	$proposal_rows = array();
	$i = 1;
	$category_data = _tbc_list_of_category($preference_data->category);
	$output = "<hr>";
	while ($row = $result->fetchObject())
	{
		$proposal_date = date("d-m-Y", $row->creation_date); // remove comment to display year
		if ($row->category != NULL)
		{
			$category = $row->category;
		} //$row->category != NULL
		else
		{
			$category = "Not assigned";
		}
		$preference_rows[] = array(
			$i,
			$proposal_date,
			$row->book . "<br><br>[ Author: " . $row->author . ", Publisher: " . $row->publisher . ", Year: " . $row->year . ", Edition: " . $row->edition . ", ISBN: " . $row->isbn . " ]",
			$category
		);
		$i++;
	} //$row = $result->fetchObject()
	$preference_header = array(
		'No',
		'Proposal Date',
		'Book',
		'Category'
	);
	$output .= theme('table', array(
		'header' => $preference_header,
		'rows' => $preference_rows
	));
	return $output;
}
function _textbook_companion_list_of_new_category($category_id = NULL)
{
	$category .= "";
	if ($category_id != NULL)
	{
		$query = db_select('list_of_category');
		$query->fields('list_of_category');
		$query->condition('id', $category_id);
		$category_list = $query->execute();
		$category .= "<ul>";
	} //$category_id != NULL
	else
	{
		$category_list = db_query('SELECT * FROM {list_of_category} WHERE category_id != 0');
	}
	$i = 1;
	while ($category_list_data = $category_list->fetchObject())
	{
		$category .= "<li><a href=#$i>$category_list_data->maincategory</a></li>";
		$i++;
	} //$category_list_data = $category_list->fetchObject()
	$category .= "</ul>";
	return $category;
}
function _textbook_companion_list_of_new_category_display($category_id = NULL)
{
	$category .= "";
	if ($category_id != NULL)
	{
		$query = db_select('list_of_category');
		$query->fields('list_of_category');
		$query->condition('id', $category_id);
		$category_list = $query->execute();
	} //$category_id != NULL
	else
	{
		$category_list = db_query('SELECT * FROM {list_of_category} WHERE category_id != 0');
	}
	while ($category_list_data = $category_list->fetchObject())
	{
		$category .= "<li>$category_list_data->maincategory</li>";
		$query_sub_cat = db_select('list_of_subcategory');
		$query_sub_cat->fields('list_of_subcategory');
		$query_sub_cat->condition('maincategory_id', $category_id);
		$subcategory_list = $query_sub_cat->execute();
		$category .= "<ol style='list-style-type: lower-roman;'><h5>";
		while ($sub_category_list_data = $subcategory_list->fetchObject())
		{
			$preference_q = db_query("
      SELECT DISTINCT (tcbm.sub_category), los.subcategory, loc.category_id,loc.maincategory,
      pe.book as book, pe.author as author, pe.publisher as publisher, pe.year as year, pe.id as pe_id,
      po.approval_date as approval_date
      FROM textbook_companion_preference pe
      LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id
      LEFT JOIN textbook_companion_book_main_subcategories tcbm ON pe.id = tcbm.pref_id
      LEFT JOIN list_of_category loc ON tcbm.main_category = loc.category_id
      LEFT JOIN list_of_subcategory los ON tcbm.sub_category = los.subcategory_id
      WHERE po.proposal_status = 3 AND pe.approval_status = 1 AND pe.category>0
      AND pe.id = tcbm.pref_id AND tcbm.sub_category= :subcategory", array(
				":subcategory" => $sub_category_list_data->subcategory_id
			));
			$category .= "<li>$sub_category_list_data->subcategory</li>";
			$category .= "<ol style='list-style-type: decimal;'>";
			while ($preference_data = $preference_q->fetchObject())
			{
				if ($sub_category_list_data->subcategory == $preference_data->subcategory && $sub_category_list_data->maincategory_id == $preference_data->category_id)
				{
					$category .= "<li>";
					$category .= l($preference_data->book . " by " . $preference_data->author . ", " . $preference_data->publisher . ", " . $preference_data->year, 'textbook_run/' . $preference_data->pe_id . '/' . $preference_data->sub_category . '/' . $preference_data->category_id);
					$category .= "</li>";
				} //$sub_category_list_data->subcategory == $preference_data->subcategory && $sub_category_list_data->maincategory_id == $preference_data->category_id
			} //$preference_data = $preference_q->fetchObject()
			$category .= "</ol>";
		} //$sub_category_list_data = $subcategory_list->fetchObject()
		$category .= "</h5></ol>";
	} //$category_list_data = $category_list->fetchObject()
	return $category;
}
/* Display Completed books */
function tbc_completed_books_all()
{
	$category_id = NULL;
	$output = "";
	$output = "<h4>Category</h4>";
	$output .= "<hr style='background-color: #abb2b8;' /><div style='width:100%; float:left;'><h4>";
	$output .= _textbook_companion_list_of_new_category($category_id);
	$output .= "</h4></div>";
	$result_count = db_query("SELECT pe.book FROM {textbook_companion_preference} pe LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id WHERE po.proposal_status =3 AND pe.approval_status =1 AND pe.category>0");
	$row_count = $result_count->rowCount();
	$output .= "<p style='clear: both;'>Total number of completed books : &nbsp;" . $row_count . " </p>";
	$output .= "<hr style='background-color: #abb2b8;' />";
	$result_category = db_query("SELECT * FROM {list_of_category} WHERE category_id !=0");
	$row_category_count = $result_category->rowCount();
	$output .= "<ol style='list-style-type: upper-roman;'><h4>";
	for ($i = 1; $i <= $row_category_count; $i++)
	{
		$output .= "<div id=$i>" . _textbook_companion_list_of_new_category_display($i) . "</div><br>";
	} //$i = 1; $i <= $row_category_count; $i++
	$output .= "</h4></ol>";
	return $output;
}
function tbc_completed_books_display_new_category_all()
{
	$category_id = NULL;
	$output = "";
	$tbc_books_display_new_category_form = drupal_get_form("tbc_books_display_new_category_form");
	$output .= drupal_render($tbc_books_display_new_category_form);
	return $output;
}
function tbc_books_display_new_category_form($form, &$form_state)
{
	$category_default_value = 0;
	$form['category'] = array(
		'#type' => 'select',
		'#title' => t('Category'),
		'#options' => _list_of_display_category(),
		'#default_value' => $category_default_value,
		'#ajax' => array(
			'callback' => 'ajax_display_all_book_list_callback'
		),
		'#validated' => TRUE
	);
	$form['subcategory'] = array(
		'#type' => 'select',
		'#title' => t('Sub Category'),
		'#options' => _list_of_subcategory($category_default_value),
		'#default_value' => $category_default_value,
		'#ajax' => array(
			'callback' => 'ajax_display_subcategory_book_list_callback'
		),
		'#prefix' => '<div id="ajax-subcategory-list-replace">',
		'#suffix' => '</div>',
		'#validated' => TRUE,
		'#states' => array(
			'invisible' => array(
				':input[name="category"]' => array(
					'value' => 0
				)
			)
		)
	);
	$form['book'] = array(
		'#type' => 'item',
		'#prefix' => '<div id="ajax-book-list-replace">',
		'#suffix' => '</div>',
		'#markup' => _list_of_all_completed_books()
	);
	return $form;
}
function _list_of_display_category($category_id = NULL)
{
	$category = "";
	$category[0] .= "All";
	if ($category_id == NULL)
	{
		$query = db_select('list_of_category');
		$query->fields('list_of_category');
		$query->orderBy('category_id', 'ASC');
		$category_list = $query->execute();
	} //$category_id == NULL
	else
	{
		$query = db_select('list_of_category');
		$query->fields('list_of_category');
		$query->condition('category_id', $category_id);
		$query->orderBy('id', 'ASC');
		$category_list = $query->execute();
	}
	while ($category_list_data = $category_list->fetchObject())
	{
		$category[$category_list_data->category_id] .= $category_list_data->maincategory;
	} //$category_list_data = $category_list->fetchObject()
	return $category;
}
/********************* Ajax callback ***************************/
function ajax_display_all_book_list_callback($form, $form_state)
{
	$category_default_value = $form_state['values']['category'];
	if ($category_default_value > 0)
	{
		$form['subcategory']['#options'] = _list_of_subcategory($category_default_value);
		$form['book']['#markup'] = _list_of_all_completed_books($category_default_value);
		$commands[] = ajax_command_replace("#ajax-subcategory-list-replace", drupal_render($form['subcategory']));
		$commands[] = ajax_command_replace("#ajax-book-list-replace", drupal_render($form['book']));
	} //$category_default_value > 0
	else
	{
		$form['book']['#markup'] = _list_of_all_completed_books();
		$commands[] = ajax_command_replace("#ajax-book-list-replace", drupal_render($form['book']));
		$commands[] = ajax_command_html("#ajax-subcategory-list-replace", '');
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
function ajax_display_subcategory_book_list_callback($form, $form_state)
{
	$category_default_value = $form_state['values']['category'];
	$subcategory_default_value = $form_state['values']['subcategory'];
	if ($category_default_value > 0 && $subcategory_default_value > 0)
	{
		$form['subcategory']['#options'] = _list_of_subcategory($category_default_value);
		$form['book']['#markup'] = _list_of_all_completed_books($category_default_value, $subcategory_default_value);
		$commands[] = ajax_command_replace("#ajax-subcategory-list-replace", drupal_render($form['subcategory']));
		$commands[] = ajax_command_replace("#ajax-book-list-replace", drupal_render($form['book']));
	} //$category_default_value > 0
	else
	{
		$form['book']['#markup'] = _list_of_all_completed_books();
		$commands[] = ajax_command_replace("#ajax-book-list-replace", drupal_render($form['book']));
		$commands[] = ajax_command_html("#ajax-subcategory-list-replace", '');
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
/*************************************************************************/
function _list_of_all_completed_books($category_default_value = NULL, $subcategory_default_value = NULL)
{
	$output = "";
	//get the book count
	$result = db_query("SELECT COUNT( pe.book ) AS book_count FROM {textbook_companion_preference} pe LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id WHERE po.proposal_status =3 AND pe.approval_status =1 AND pe.category>0");
	$row = $result->fetchObject();
	$book_count = $row->book_count;
	$i = 1;
	/* get preference */
	if ($category_default_value <= 0 && $subcategory_default_value == NULL)
	{
		$preference_q = db_query("SELECT DISTINCT pe.book, count( pe.book) c,tcbm.sub_category,tcbm.main_category as category_id,
      pe.author as author, pe.publisher as publisher, pe.year as year, pe.id as pe_id, pe.edition,
      po.approval_date as approval_date
      FROM {textbook_companion_preference} pe
      LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id
      LEFT JOIN textbook_companion_book_main_subcategories tcbm ON pe.id = tcbm.pref_id
      LEFT JOIN list_of_category loc ON tcbm.main_category = loc.category_id
      WHERE po.proposal_status = 3 AND pe.approval_status = 1 AND pe.category>0
      AND pe.id = tcbm.pref_id GROUP BY pe.book having c >= 1 ORDER BY pe.book");
	} //$category_default_value <= 0 && $subcategory_default_value == NULL
	elseif ($category_default_value > 0 && $subcategory_default_value == NULL)
	{
		$preference_q = db_query("SELECT DISTINCT (loc.category_id),count( pe.book) c, tcbm.sub_category,loc.maincategory,
      pe.book as book, pe.author as author, pe.publisher as publisher, pe.year as year, pe.id as pe_id, pe.edition,
      po.approval_date as approval_date
      FROM {textbook_companion_preference} pe
      LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id
      LEFT JOIN textbook_companion_book_main_subcategories tcbm ON pe.id = tcbm.pref_id
      LEFT JOIN list_of_category loc ON tcbm.main_category = loc.category_id
      WHERE po.proposal_status = 3 AND pe.approval_status = 1 AND pe.category>0
      AND pe.id = tcbm.pref_id AND loc.category_id= :category_id GROUP BY pe.book having c >= 1 ORDER BY pe.book", array(
			"category_id" => $category_default_value
		));
	} //$category_default_value > 0 && $subcategory_default_value == NULL
	else
	{
		$preference_q = db_query("SELECT DISTINCT (loc.category_id),tcbm.sub_category,loc.maincategory,
      pe.book as book, pe.author as author, pe.publisher as publisher, pe.year as year, pe.id as pe_id, pe.edition,
      po.approval_date as approval_date
      FROM textbook_companion_preference pe
      LEFT JOIN textbook_companion_proposal po ON pe.proposal_id = po.id
      LEFT JOIN textbook_companion_book_main_subcategories tcbm ON pe.id = tcbm.pref_id
      LEFT JOIN list_of_category loc ON tcbm.main_category = loc.category_id
      WHERE po.proposal_status = 3 AND pe.approval_status = 1 AND pe.category>0
      AND pe.id = tcbm.pref_id AND loc.category_id= :category_id AND tcbm.sub_category = :sub_category", array(
			"category_id" => $category_default_value,
			":sub_category" => $subcategory_default_value
		));
	}
	while ($preference_data = $preference_q->fetchObject())
	{
		$proposal_rows[] = array(
			$i,
			l($preference_data->book . " by " . $preference_data->author . ", " . $preference_data->publisher . ", " . $preference_data->year, 'textbook_run/' . $preference_data->pe_id . '/' . $preference_data->sub_category . '/' . $preference_data->category_id)
		);
		$i++;
	} //$proposal_data = $proposal_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$proposal_rows)
	{
		$output .= t('There are no books availabe in this sub category.');
	} //!$proposal_rows
	else
	{
		$proposal_header = array(
			'No.',
			'Title of the Book'
		);
		$output .= theme('table', array(
			'header' => $proposal_header,
			'rows' => $proposal_rows
		));
	} //!$proposal_rows
	//	$output .= "Book count with category: " . $book_count;
	return $output;
}
function _list_of_subcategory($category_id = NULL)
{
	$subcategory = "";
	$subcategory[0] .= "Please select";
	if ($category_id == NULL)
	{
		$query = db_select('list_of_subcategory');
		$query->fields('list_of_subcategory');
		$query->condition('maincategory_id', $category_id);
		$query->orderBy('subcategory_id', 'ASC');
		$subcategory_list = $query->execute();
	} //$category_id == NULL
	else
	{
		$query = db_select('list_of_subcategory');
		$query->fields('list_of_subcategory');
		$query->condition('maincategory_id', $category_id);
		$query->orderBy('id', 'ASC');
		$subcategory_list = $query->execute();
	}
	while ($subcategory_list_data = $subcategory_list->fetchObject())
	{
		$subcategory[$subcategory_list_data->subcategory_id] .= $subcategory_list_data->subcategory;
	} //$category_list_data = $category_list->fetchObject()
	return $subcategory;
}
