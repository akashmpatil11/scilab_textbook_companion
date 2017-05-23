<?php
/* Aicte books pickup before the proposal form */
function textbook_companion_aicte_proposal_form($form, &$form_state)
{
	/*********** fetch aicte book data ********************/
	$query = db_select('textbook_companion_aicte');
	$query->fields('textbook_companion_aicte');
	$query->condition('status', 0);
	$query->condition('selected', 0);
	$result = $query->execute();
	$form = array();
	$form["wrapper"] = array(
		"#type" => "fieldset",
		"#prefix" => "<div id='aicte-form-wrapper'>",
		"#suffix" => "</div>"
	);
	$num_rows = $result->rowCount();
	if ($num_rows > 0)
	{
		while ($row = $result->fetchObject())
		{
			/* fixing title string */
			$title = "";
			$edition = "";
			$year = "";
			$title = "{$row->book} by {$row->author}";
			if ($row->edition)
			{
				$edition = "<i>ed</i>: {$row->edition}";
			} //$row->edition
			if ($row->year)
			{
				if ($row->edition)
				{
					$year = ", <i>pub</i>: {$row->year}";
				} //$row->edition
				else
				{
					$year = "<i>pub</i>: {$row->year}";
				}
			} //$row->year
			if ($edition or $year)
			{
				$title .= "({$edition} {$year})";
			} //$edition or $year
			$form["wrapper"][$row->id] = array(
				"#type" => "checkbox",
				"#title" => $title,
				"#prefix" => "<div class='title'>",
				"#suffix" => "</div>"
			);
		} //$row = $result->fetchObject()
	} //$num_rows > 0
	$form["submit"] = array(
		"#type" => "submit",
		"#value" => "Submit Book Selections"
	);
	return $form;
}
function textbook_companion_aicte_proposal_form_validate($form, &$form_state)
{
	$query = db_select('textbook_companion_aicte');
	$query->fields('textbook_companion_aicte');
	$query->condition('status', 0);
	$query->condition('selected', 0);
	$result = $query->execute();
	$count = 0;
	$selections = array();
	while ($row = $result->fetchObject())
	{
		if ($form_state["values"][$row->id] == 1)
		{
			$count++;
			array_push($selections, $row->id);
		} //$form_state["values"][$row->id] == 1
	} //$row = $result->fetchObject()
	/* user can choose only 3 books to propose */
	if ($count != variable_get('textbook_companion_book_preferences', ''))
	{
		form_set_error("", "Please select only <strong>1</strong> book. You currently selected <strong>{$count}</strong>");
	} //$count > 3
	elseif ($count == 0)
	{
		
		form_set_error("", "Please select only <strong>1</strong> book. You currently selected <strong>{$count}</strong>");
		
	} //$count == 0
	else
	{
		$form_state["values"]["selections"] = $selections;
	}
}
/* AICTE Book list proposal form submit */
function textbook_companion_aicte_proposal_form_submit($form, &$form_state)
{
	global $user;
	$selections = $form_state["values"]["selections"];
	//var_dump($selections);    
	variable_set("aicte_" . $user->uid, $selections);
	drupal_goto("proposal");
}
/************* AICTE book report form **************/
function textbook_companion_aicte_report_form($from, &$form_state)
{
	$query = db_select('textbook_companion_aicte');
	$query->fields('textbook_companion_aicte');
	$query->condition('status', 0);
	$query->orderBy('book', 'ASC');
	$result = $query->execute();
	$books = array();
	$books[0] = "Please select a book";
	while ($row = $result->fetchObject())
	{
		$books[$row->id] = "{$row->book} ({$row->author})";
	} //$row = $result->fetchObject()
	$form = array();
	$form["name"] = array(
		"#type" => "textfield",
		"#title" => "Name",
		"#description" => t("Please enter your name.")
	);
	$form["email"] = array(
		"#type" => "textfield",
		"#title" => "Email",
		"#description" => t("Please enter your valid email id.")
	);
	$form["number"] = array(
		"#type" => "textfield",
		"#title" => "Number",
		"#description" => t("Please enter your valid phone number.")
	);
	$form["book"] = array(
		"#type" => "select",
		"#title" => "AICTE Book",
		"#description" => t("Please select a book."),
		"#options" => $books
	);
	$form["comment"] = array(
		"#type" => "textarea",
		"#title" => "Any other comment?",
		"#description" => t("Please enter your query (if any)")
	);
	$form["submit"] = array(
		"#type" => "submit",
		"#value" => "Submit"
	);
	return $form;
}
/************* AICTE book report form submit **************/
function textbook_companion_aicte_report_form_submit($form, &$form_state)
{
	$v = $form_state["values"];
	$query = "INSERT INTO {textbook_companion_aicte_report}
        (aicte_id, name, number, email, comment)
        VALUES (:aicte_id, :name, :number, :email, :comment)";
	$args = array(
		":aicte_id" => $v["book"],
		":name" => $v["name"],
		":number" => $v["number"],
		":email" => $v["email"],
		":comment" => $v["comment"]
	);
	$result = db_query($query, $args, array(
		'return' => Database::RETURN_INSERT_ID
	));
	drupal_set_message("Thank you for reporting.", "status");
}
/************* AICTE book list selection **************/
function textbook_companion_aicte_proposal_all()
{
	global $user;
	$page_content = "";
	if (!$user->uid)
	{
		$query = db_select('textbook_companion_aicte');
		$query->fields('textbook_companion_aicte');
		$query->condition('status', 0);
		$result = $query->execute();
		$page_content .= "<ul>";
		$page_content .= "<li>These are the list of books available for <em>Textbook Companion</em> proposal.</li>";
		$page_content .= "<li>Please <a href='/user'><b><u>Login</u></b></a> to create a proposal.</li>";
		//$page_content .= "<li>Unable to propose particular book: <a id='aicte-report' href='#'>Click here</a></li>";
		//$page_content .= "<li>Do not wish to propose any of the below books: <a id='aicte-report' href='http://fossee.in/feedback/scilab-aicte' target = _blank>Click here</a></li>";
		$page_content .= "</ul>";
		$page_content .= "Search :  <input type='text' id='searchtext' onkeyup='searchbookFunction()' style='width:82%'/>";
		$page_content .= "<input type='button' value ='clear' id='search_clear'/>";
		$page_content .= "<div id='aicte-list-wrapper'>";
		$num_rows = $result->rowCount();
		if ($num_rows > 0)
		{
			$i = 1;
			while ($row = $result->fetchObject())
			{
				/* fixing title string */
				$title = "";
				$edition = "";
				$year = "";
				$title = "{$row->book} by {$row->author}";
				if ($row->edition)
				{
					$edition = "<i>ed</i>: {$row->edition}";
				} //$row->edition
				if ($row->year)
				{
					if ($row->edition)
					{
						$year = ", <i>pub</i>: {$row->year}";
					} //$row->edition
					else
					{
						$year = "<i>pub</i>: {$row->year}";
					}
				} //$row->year
				if ($edition or $year)
				{
					$title .= "({$edition} {$year})";
				} //$edition or $year
				$page_content .= "<div class='title'>{$i}) {$title}</div>";
				$i++;
			} //$row = $result->fetchObject()
		} //$num_rows > 0
		$page_content .= "</div>";
		/* adding aicte report form */
		//$page_content .= drupal_get_form("textbook_companion_aicte_report_form");
		return $page_content;
	} //!$user->uid
	/* check if user has already submitted a proposal */
	$query = db_select('textbook_companion_proposal');
	$query->fields('textbook_companion_proposal');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			switch ($proposal_data->proposal_status)
			{
				case 0:
					drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
					drupal_goto('');
					return;
					break;
				case 1:
					drupal_set_message(t('Your proposal has been approved. Please go to ' . l('Code Submission', 'textbook_companion/code') . ' to upload your code'), 'status');
					drupal_goto('');
					return;
					break;
				case 2:
					drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal below.'), 'error');
					break;
				case 3:
					drupal_set_message(t('Congratulations! You have completed your last book proposal. You can create another proposal below.'), 'status');
					break;
				default:
					drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
					drupal_goto('');
					return;
					break;
			} //$proposal_data->proposal_status
		} //$proposal_data = $proposal_q->fetchObject()
	} //$proposal_q
	variable_del("aicte_" . $user->uid);
	$page_content .= "<h5><b>* Below is the list of AICTE recommended textbooks which you may propose. Select any one to continue:</b></h5></br>";
	//$page_content .= "Unable to propose particular book: <a id='aicte-report' href='#'>Click here</a></br></br>";
	//$page_content .= "Do not wish to propose any of the below books: <a id='aicte-report' href='http://fossee.in/feedback/scilab-aicte' target = _blank>Click here</a></br></br>";
	$page_content .= "Search :  <input type='text' id='searchtext' style='width:90%'/>";
	$page_content .= "<br><br><center><input type='button' value ='clear' id='search_clear'/></center>";
	$textbook_companion_aicte_proposal_form = drupal_get_form("textbook_companion_aicte_proposal_form");
	$page_content .= drupal_render($textbook_companion_aicte_proposal_form);
	return $page_content;
}
/*************** non aicte book proposal *******************/
function textbook_companion_nonaicte_proposal_all()
{
	global $user;
	$page_content = "";
	if (!$user->uid)
	{
		$page_content .= "<ul>";
		$page_content .= "<li>Please <a href='/user'><b><u>Login</u></b></a> to create a proposal.</li>";
		$page_content .= "</ul>";
		return $page_content;
	} //!$user->uid
	/* check if user has already submitted a proposal */
	/*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);*/
	$query = db_select('textbook_companion_proposal');
	$query->fields('textbook_companion_proposal');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			switch ($proposal_data->proposal_status)
			{
				case 0:
					drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
					drupal_goto('');
					return;
					break;
				case 1:
					drupal_set_message(t('Your proposal has been approved. Please go to ' . l('Code Submission', 'textbook_companion/code') . ' to upload your code'), 'status');
					drupal_goto('');
					return;
					break;
				case 2:
					drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal below.'), 'error');
					break;
				case 3:
					drupal_set_message(t('Congratulations! You have completed your last book proposal. You can create another proposal below.'), 'status');
					break;
				default:
					drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
					drupal_goto('');
					return;
					break;
			} //$proposal_data->proposal_status
		} //$proposal_data = $proposal_q->fetchObject()
	} //$proposal_q
	//variable_del("aicte_".$user->uid);
	$book_proposal_nonaicte_form = drupal_get_form("book_proposal_nonaicte_form");
	$page_content .= drupal_render($book_proposal_nonaicte_form);
	return $page_content;
}
/* Textbook Companion Proposal */
function textbook_companion_proposal_all()
{
	global $user;
	$page_content = "";
	// if (!$user->uid) {
	//     drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
	//     return;
	// }
	/************* check if user has already submitted a proposal **************/
	$query = db_select('textbook_companion_proposal');
	$query->fields('textbook_companion_proposal');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			switch ($proposal_data->proposal_status)
			{
				case 0:
					drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
					drupal_goto('');
					return;
					break;
				case 1:
					drupal_set_message(t('Your proposal has been approved. Please go to ' . l('Code Submission', 'textbook_companion/code') . ' to upload your code'), 'status');
					drupal_goto('');
					return;
					break;
				case 2:
					drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal below.'), 'error');
					break;
				case 3:
					drupal_set_message(t('Congratulations! You have completed your last book proposal. You can create another proposal below.'), 'status');
					break;
				default:
					drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
					drupal_goto('');
					return;
					break;
			} //$proposal_data->proposal_status
		} //$proposal_data = $proposal_q->fetchObject()
	} //$proposal_q
	$selections = variable_get("aicte_" . $user->uid, "");
	if ($selections)
	{
		$query = db_select('textbook_companion_aicte');
		$query->fields('textbook_companion_aicte');
		$query->condition('id', $selections[0]);
		$result1 = $query->execute();
		$row1 = $result1->fetchObject();
		$query = db_select('textbook_companion_aicte');
		$query->fields('textbook_companion_aicte');
		$query->condition('id', $selections[1]);
		$result2 = $query->execute();
		$row2 = $result2->fetchObject();
		$query = db_select('textbook_companion_aicte');
		$query->fields('textbook_companion_aicte');
		$query->condition('id', $selections[2]);
		$result3 = $query->execute();
		$row3 = $result3->fetchObject();
		$book_proposal_form = drupal_get_form("book_proposal_form", $row1, $row2, $row3);
		$page_content .= drupal_render($book_proposal_form);
	} //$selections
	else
	{
		// $book_proposal_form = drupal_get_form("book_proposal_form");
		//  $page_content .= drupal_render($book_proposal_form);
		drupal_goto("aicte_proposal");
	}
	return $page_content;
}
/* AICTE book proposal form */
function book_proposal_form($form, &$form_state, $row1 = NULL, $row2 = NULL, $row3 = NULL)
{
	global $user;
	$form = array();
	$form['imp_notice'] = array(
		'#type' => 'item',
		'#markup' => '<font color="red"><b>Please fill up this form carefully as the details entered here will be exactly written in the Textbook Companion</b></font>'
	);
	$form['full_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Full Name'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
	);
	$form['email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#size' => 30,
		'#value' => $user->mail,
		'#disabled' => TRUE
	);
	$form['mobile'] = array(
		'#type' => 'textfield',
		'#title' => t('Mobile No.'),
		'#size' => 30,
		'#maxlength' => 15,
		'#required' => TRUE
	);
	$form['gender'] = array(
		'#type' => 'radios',
		'#title' => t('Gender'),
		'#options' => array(
			'M' => 'Male',
			'F' => 'Female'
		),
		'#required' => TRUE
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
	);*/
	$form['course'] = array(
		'#type' => 'textfield',
		'#title' => t('Course'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
	);
	$form['branch'] = array(
		'#type' => 'select',
		'#title' => t('Department/Branch'),
		'#options' => _tbc_list_of_departments(),
		'#required' => TRUE
	);
	$form['university'] = array(
		'#type' => 'textfield',
		'#title' => t('University/Institute'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
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
		'#validated' => TRUE
	);
	$form['other_country'] = array(
		'#type' => 'textfield',
		'#title' => t('Other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your country name')
		),
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
		'#required' => False,
		'#attributes' => array(
			'placeholder' => 'Enter pincode....'
		)
	);
	$form['faculty'] = array(
		'#type' => 'hidden',
		'#value' => 'None',
		'#title' => t('College Teacher/Professor'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
	);
	$form['faculty_email'] = array(
		'#type' => 'hidden',
		'#value' => 'None',
		'#title' => t('Teacher/Professor Email Id'),
		'#default_value' => '@email.com',
		'#size' => 30,
		'#maxlength' => 50
	);
	$form['reviewer'] = array(
		'#type' => 'hidden',
		'#value' => 'None',
		'#title' => t('Reviewer'),
		'#size' => 30,
		'#maxlength' => 50
	);
	$form['completion_date'] = array(
		'#type' => 'textfield',
		'#title' => t('Expected Date of Completion'),
		'#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
		'#size' => 10,
		'#maxlength' => 10
	);
	$form['operating_system'] = array(
		'#type' => 'textfield',
		'#title' => t('Operating System'),
		'#required' => TRUE,
		'#size' => 30,
		'#maxlength' => 50
	);
	$form['version'] = array(
		'#type' => 'textfield',
		'#title' => t('Scilab Version'),
		'#description' => t('Input version format should be like 5.4 or 5.4.1'),
		'#size' => 10,
		'#required' => TRUE,
		'#maxlength' => 20
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
		'#default_value' => $row1->book,
		'#disabled' => ($row1->book ? TRUE : FALSE)
	);
	$form['preference1']['author1'] = array(
		'#type' => 'textfield',
		'#title' => t('Author Name'),
		'#size' => 30,
		'#maxlength' => 100,
		'#required' => TRUE,
		'#default_value' => $row1->author,
		'#disabled' => ($row1->author ? TRUE : FALSE)
	);
	$form['preference1']['isbn1'] = array(
		'#type' => 'textfield',
		'#title' => t('ISBN No'),
		'#size' => 30,
		'#maxlength' => 25,
		'#required' => TRUE,
		'#default_value' => $row1->isbn,
		'#disabled' => ($row1->isbn ? TRUE : FALSE)
	);
	$form['preference1']['publisher1'] = array(
		'#type' => 'textfield',
		'#title' => t('Publisher & Place'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE,
		'#default_value' => $row1->publisher
	);
	$form['preference1']['edition1'] = array(
		'#type' => 'textfield',
		'#title' => t('Edition'),
		'#size' => 2,
		'#maxlength' => 2,
		'#required' => TRUE,
		'#default_value' => $row1->edition
	);
	$form['preference1']['year1'] = array(
		'#type' => 'textfield',
		'#title' => t('Year of pulication'),
		'#size' => 4,
		'#maxlength' => 4,
		'#required' => TRUE,
		'#default_value' => $row1->year
	);
	$form['preference1']['book_category_1'] = array(
		'#type' => 'select',
		'#title' => t('Category'),
		'#options' => _tbc_list_of_category(),
		'#required' => TRUE
	);
	if ($row2 != NULL)
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
			'#default_value' => $row2->book,
			'#disabled' => ($row2->book ? TRUE : FALSE)
		);
		$form['preference2']['author2'] = array(
			'#type' => 'textfield',
			'#title' => t('Author Name'),
			'#size' => 30,
			'#maxlength' => 100,
			'#required' => TRUE,
			'#default_value' => $row2->author,
			'#disabled' => ($row2->author ? TRUE : FALSE)
		);
		$form['preference2']['isbn2'] = array(
			'#type' => 'textfield',
			'#title' => t('ISBN No'),
			'#size' => 30,
			'#maxlength' => 25,
			'#required' => TRUE,
			'#default_value' => $row2->isbn,
			'#disabled' => ($row2->isbn ? TRUE : FALSE)
		);
		$form['preference2']['publisher2'] = array(
			'#type' => 'textfield',
			'#title' => t('Publisher & Place'),
			'#size' => 30,
			'#maxlength' => 50,
			'#required' => TRUE,
			'#default_value' => $row2->publisher
		);
		$form['preference2']['edition2'] = array(
			'#type' => 'textfield',
			'#title' => t('Edition'),
			'#size' => 4,
			'#maxlength' => 2,
			'#required' => TRUE,
			'#default_value' => $row2->edition
		);
		$form['preference2']['year2'] = array(
			'#type' => 'textfield',
			'#title' => t('Year of pulication'),
			'#size' => 4,
			'#maxlength' => 4,
			'#required' => TRUE,
			'#default_value' => $row2->year
		);
		$form['preference2']['book_category_2'] = array(
			'#type' => 'select',
			'#title' => t('Category'),
			'#options' => _tbc_list_of_category(),
			'#required' => TRUE
		);
	} //$row2 != NULL
	if ($row3 != NULL)
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
			'#default_value' => $row3->book,
			'#disabled' => ($row3->book ? TRUE : FALSE)
		);
		$form['preference3']['author3'] = array(
			'#type' => 'textfield',
			'#title' => t('Author Name'),
			'#size' => 30,
			'#maxlength' => 100,
			'#required' => TRUE,
			'#default_value' => $row3->author,
			'#disabled' => ($row3->author ? TRUE : FALSE)
		);
		$form['preference3']['isbn3'] = array(
			'#type' => 'textfield',
			'#title' => t('ISBN No'),
			'#size' => 30,
			'#maxlength' => 25,
			'#required' => TRUE,
			'#default_value' => $row3->isbn,
			'#disabled' => ($row3->isbn ? TRUE : FALSE)
		);
		$form['preference3']['publisher3'] = array(
			'#type' => 'textfield',
			'#title' => t('Publisher & Place'),
			'#size' => 30,
			'#maxlength' => 50,
			'#required' => TRUE,
			'#default_value' => $row3->publisher
		);
		$form['preference3']['edition3'] = array(
			'#type' => 'textfield',
			'#title' => t('Edition'),
			'#size' => 4,
			'#maxlength' => 2,
			'#required' => TRUE,
			'#default_value' => $row3->edition
		);
		$form['preference3']['year3'] = array(
			'#type' => 'textfield',
			'#title' => t('Year of pulication'),
			'#size' => 4,
			'#maxlength' => 4,
			'#required' => TRUE,
			'#default_value' => $row3->year
		);
		$form['preference3']['book_category_3'] = array(
			'#type' => 'select',
			'#title' => t('Category'),
			'#options' => _tbc_list_of_category(),
			'#required' => TRUE
		);
	} //$row3 != NULL
	$form['samplefile'] = array(
		'#type' => 'fieldset',
		'#title' => t('<span class="form-required form-item" title="This field is required.">Sample Source Files *</span>'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE
	);
	$form['samplefile']['samplefile1'] = array(
		'#type' => 'file',
		'#title' => t('Upload sample source file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions : ') . variable_get('textbook_companion_sample_source_file_extensions', '') . '</span>'
	);
	$form['termconditions'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Terms And Conditions'),
		'#options' => array(
			'status' => t('<a href="/term-and-conditions" target="_blank">I agree to the Terms and Conditions</a>')
		),
		'#required' => TRUE
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}
/******************** AICTE book proposal from validate *****************************/
function book_proposal_form_validate($form, &$form_state)
{
	/* mobile */
	if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
		form_set_error('mobile', t('Invalid mobile number'));
	/* date of completion */
	if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
		form_set_error('completion_date', t('Invalid expected date of completion'));
	list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
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
	/* version */
	if (!preg_match('/^\d+(?:\.\d+)+/', $form_state['values']['version']))
		form_set_error('version', t('Please enter correct version in given format'));
	/* country */
	if ($form_state['values']['country'] == 'Others')
	{
		if ($form_state['values']['other_country'] == '')
		{
			form_set_error('other_country', t('Enter country name'));
		} //$form_state['values']['other_country'] == ''
		else
		{
			$form_state['values']['country'] = $form_state['values']['other_country'];
		}
		if ($form_state['values']['other_state'] == '')
		{
			form_set_error('other_state', t('Enter state name'));
		} //$form_state['values']['other_state'] == ''
		else
		{
			$form_state['values']['all_state'] = $form_state['values']['other_state'];
		}
		if ($form_state['values']['other_city'] == '')
		{
			form_set_error('other_city', t('Enter city name'));
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
		} //$form_state['values']['country'] == ''
		if ($form_state['values']['all_state'] == '')
		{
			form_set_error('all_state', t('Select state name'));
		} //$form_state['values']['all_state'] == ''
		if ($form_state['values']['city'] == '')
		{
			form_set_error('city', t('Select city name'));
		} //$form_state['values']['city'] == ''
	}
	if (isset($_FILES['files']))
	{
		/* check if atleast one source or result file is uploaded */
		if (!($_FILES['files']['name']['samplefile1']))
			form_set_error('samplefile1', t('Please upload sample code main or source file.'));
		/* check for valid filename extensions */
		foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
		{
			if ($file_name)
			{
				/* checking file type */
				if (strstr($file_form_name, 'sample'))
					$file_type = 'S';
				else
					$file_type = 'U';
				$allowed_extensions_str = '';
				switch ($file_type)
				{
					case 'S':
						$allowed_extensions_str = variable_get('textbook_companion_sample_source_file_extensions', '');
						break;
				} //$file_type
				$allowed_extensions = explode(',', $allowed_extensions_str);
				$allowd_file = strtolower($_FILES['files']['name'][$file_form_name]);
				$allowd_files = explode('.', $allowd_file);
				$temp_extension = end($allowd_files);
				if (!in_array($temp_extension, $allowed_extensions))
					form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
				if ($_FILES['files']['size'][$file_form_name] <= 0)
					form_set_error($file_form_name, t('File size cannot be zero.'));
				/* check if valid file name */
				if (!textbook_companion_check_valid_filename($_FILES['files']['name'][$file_form_name]))
					form_set_error($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
			} //$file_name
		} //$_FILES['files']['name'] as $file_form_name => $file_name
	} //isset($_FILES['files'])
	return;
}
function book_proposal_form_submit($form, &$form_state)
{
	global $user;
	$selections = variable_get("aicte_" . $user->uid, "");
	$root_path = textbook_companion_samplecode_path();
	if (!$user->uid)
	{
		drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
		return;
	} //!$user->uid
	/* completion date to timestamp */
	list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
	$completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
	$scilab_version = 'scilab ' . $form_state['values']['version'];
	$query = "INSERT INTO {textbook_companion_proposal}
    (uid, approver_uid, full_name, mobile, gender, how_project, course, branch, university, country, city, pincode, state, faculty, reviewer, completion_date, creation_date, approval_date, proposal_status, scilab_version, operating_system, teacher_email, message, samplefilepath,proposed_completion_date) VALUES (:uid, :approver_uid, :full_name, :mobile, :gender, :how_project, :course, :branch, :university, :country, :city, :pincode, :state, :faculty, :reviewer, :completion_date,
 :creation_date, :approval_date, :proposal_status, :scilab_version, :operating_system,
:teacher_email, :message, :samplefilepath,:proposed_completion_date)";
	$args = array(
		":uid" => $user->uid,
		":approver_uid" => 0,
		":full_name" => _tbc_sentence_case($form_state['values']['full_name']),
		":mobile" => $form_state['values']['mobile'],
		":gender" => $form_state['values']['gender'],
		":how_project" => 'Not available',
		":course" => _tbc_sentence_case($form_state['values']['course']),
		":branch" => $form_state['values']['branch'],
		":university" => _tbc_sentence_case($form_state['values']['university']),
		":country" => $form_state['values']['country'],
		":city" => $form_state['values']['city'],
		":pincode" => $form_state['values']['pincode'],
		":state" => $form_state['values']['all_state'],
		":faculty" => _tbc_sentence_case($form_state['values']['faculty']),
		":reviewer" => _tbc_sentence_case($form_state['values']['reviewer']),
		":completion_date" => $completion_date_timestamp,
		":creation_date" => time(),
		":approval_date" => 0,
		":proposal_status" => 0,
		":scilab_version" => $scilab_version,
		":operating_system" => _tbc_sentence_case($form_state['values']['operating_system']),
		":teacher_email" => $form_state['values']['faculty_email'],
		":message" => 'None',
		":samplefilepath" => "",
		":proposed_completion_date" => $completion_date_timestamp
	);
	$result = db_query($query, $args, array(
		'return' => Database::RETURN_INSERT_ID
	));
	$dest_path = $result . '/';
	if (!is_dir($root_path . $dest_path))
		mkdir($root_path . $dest_path);
	/* uploading files */
	foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
	{
		if ($file_name)
		{
			/* checking file type */
			$file_type = 'S';
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
				$query = "UPDATE {textbook_companion_proposal} SET samplefilepath = :samplefilepath WHERE id = :id";
				$args = array(
					":samplefilepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
					":id" => $result
				);
				$updateresult = db_query($query, $args);
				drupal_set_message($file_name . ' uploaded successfully.', 'status');
			} //move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])
			else
			{
				drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
			}
		} //$file_name
	} //$_FILES['files']['name'] as $file_form_name => $file_name
	if (!$result)
	{
		drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
		return;
	} //!$result
	/* proposal id */
	// $proposal_id = db_last_insert_id('textbook_companion_proposal', 'id');
	$proposal_id = $result;
	/* inserting first book preference */
	if ($form_state['values']['book1'])
	{
		$query = "INSERT INTO {textbook_companion_preference}
      (proposal_id, pref_number, book, author, isbn, publisher, edition, year, category, approval_status) VALUES 		   	(:proposal_id, :pref_number, :book, :author, :isbn, :publisher, :edition, :year, :category, :approval_status)
	";
		$args = array(
			":proposal_id" => $proposal_id,
			":pref_number" => 1,
			":book" => _tbc_sentence_case($form_state['values']['book1']),
			":author" => _tbc_sentence_case($form_state['values']['author1']),
			":isbn" => $form_state['values']['isbn1'],
			":publisher" => _tbc_sentence_case($form_state['values']['publisher1']),
			":edition" => $form_state['values']['edition1'],
			":year" => $form_state['values']['year1'],
			":category" => $form_state['values']['book_category_1'],
			":approval_status" => 0
		);
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		if (!$result)
		{
			drupal_set_message(t('Error receiving your first book preference.'), 'error');
		} //!$result
		else
		{
			//$preference_id = db_last_insert_id("textbook_companion_preference", "id");
			$preference_id = $result;
			/*$query = "
			UPDATE textbook_companion_aicte
			SET preference_id = {$preference_id}
			WHERE id = {$selections[0]}
			";
			db_query($query);*/
			$query = db_update('textbook_companion_aicte');
			$query->fields(array(
				'preference_id' => $preference_id
			));
			$query->condition('id', $selections[0]);
			$num_updated = $query->execute();
		}
	} //$form_state['values']['book1']
	/* inserting second book preference */
	if ($form_state['values']['book2'])
	{
		/*$result = db_query("INSERT INTO {textbook_companion_preference}
		(proposal_id, pref_number, book, author, isbn, publisher, edition, year, category, approval_status) VALUES
		(%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', %d, %d)",
		$proposal_id,
		2,
		ucwords(strtolower($form_state['values']['book2'])),
		ucwords(strtolower($form_state['values']['author2'])),
		$form_state['values']['isbn2'],
		ucwords(strtolower($form_state['values']['publisher2'])),
		$form_state['values']['edition2'],
		$form_state['values']['year2'],
		0,
		0
		);
		*/
		$query = "INSERT INTO {textbook_companion_preference}
      (proposal_id, pref_number, book, author, isbn, publisher, edition, year, category, approval_status) VALUES        (:proposal_id, :pref_number, :book, :author, :isbn, :publisher, :edition, :year, :category, :approval_status)
	";
		$args = array(
			":proposal_id" => $proposal_id,
			":pref_number" => 2,
			":book" => _tbc_sentence_case($form_state['values']['book2']),
			":author" => _tbc_sentence_case($form_state['values']['author2']),
			":isbn" => $form_state['values']['isbn2'],
			":publisher" => _tbc_sentence_case($form_state['values']['publisher2']),
			":edition" => $form_state['values']['edition2'],
			":year" => $form_state['values']['year2'],
			":category" => $form_state['values']['book_category_2'],
			":approval_status" => 0
		);
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		if (!$result)
		{
			drupal_set_message(t('Error receiving your second book preference.'), 'error');
		} //!$result
		else
		{
			//$preference_id = db_last_insert_id("textbook_companion_preference", "id");
			$preference_id = $result;
			/*$query = "
			UPDATE textbook_companion_aicte
			SET preference_id = {$preference_id}
			WHERE id = {$selections[1]}
			";
			db_query($query);*/
			$query = db_update('textbook_companion_aicte');
			$query->fields(array(
				'preference_id' => $preference_id
			));
			$query->condition('id', $selections[1]);
			$num_updated = $query->execute();
		}
	} //$form_state['values']['book2']
	/* inserting third book preference */
	if ($form_state['values']['book3'])
	{
		$query = "INSERT INTO {textbook_companion_preference}
      	(proposal_id, pref_number, book, author, isbn, publisher, edition, year, category, approval_status) VALUES 		   	(:proposal_id, :pref_number, :book, :author, :isbn, :publisher, :edition, :year, :category, :approval_status)
	";
		$args = array(
			":proposal_id" => $proposal_id,
			":pref_number" => 3,
			":book" => _tbc_sentence_case($form_state['values']['book3']),
			":author" => _tbc_sentence_case($form_state['values']['author3']),
			":isbn" => $form_state['values']['isbn3'],
			":publisher" => _tbc_sentence_case($form_state['values']['publisher3']),
			":edition" => $form_state['values']['edition3'],
			":year" => $form_state['values']['year3'],
			":category" => $form_state['values']['book_category_3'],
			":approval_status" => 0
		);
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		if (!$result)
		{
			drupal_set_message(t('Error receiving your third book preference.'), 'error');
		} //!$result
		else
		{
			$preference_id = $result;
			/*$query = "
			UPDATE textbook_companion_aicte
			SET preference_id = {$preference_id}
			WHERE id = {$selections[2]}
			";
			db_query($query);*/
			$query = db_update('textbook_companion_aicte');
			$query->fields(array(
				'preference_id' => $preference_id
			));
			$query->condition('id', $selections[2]);
			$num_updated = $query->execute();
		}
		/* locking the books in the textbook_companion_aicte table */
		foreach ($selections as $selection)
		{
			/*$query = "
			UPDATE textbook_companion_aicte
			SET status = 1, uid = {$user->uid}, proposal_id = {$proposal_id}
			WHERE id = {$selection} AND status = 0
			";
			db_query($query);*/
			$query = db_update('textbook_companion_aicte');
			$query->fields(array(
				'status' => 1,
				'uid' => $user->uid,
				'proposal_id' => $proposal_id
			));
			$query->condition('id', $selection);
			$query->condition('status', 0);
			$num_updated = $query->execute();
		} //$selections as $selection
		if (!$result)
		{
			drupal_set_message(t('Error receiving your third book preference.'), 'error');
		} //!$result
	} //$form_state['values']['book3']
	/* sending email */
	$email_to = $user->mail;
	$from = variable_get('textbook_companion_from_email', '');
	$bcc = variable_get('textbook_companion_emails', '');
	$cc = variable_get('textbook_companion_cc_emails', '');
	$params['proposal_received']['proposal_id'] = $proposal_id;
	$params['proposal_received']['user_id'] = $user->uid;
	$params['proposal_received']['headers'] = array(
		'From' => $from,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
	if (!drupal_mail('textbook_companion', 'proposal_received', $email_to, language_default(), $params, $from, TRUE))
		drupal_set_message('Error sending email message.', 'error');
	drupal_set_message(t('We have received you book proposal. We will get back to you soon.'), 'status');
	drupal_goto('');
}
/* AJAX CALLS */
function textbook_companion_ajax()
{
	$query_type = arg(2);
	if ($query_type == 'chapter_title')
	{
		$chapter_number = arg(3);
		$preference_id = arg(4);
		/*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE number = %d AND preference_id = %d LIMIT 1", $chapter_number, $preference_id);*/
		$query = db_select('textbook_companion_chapter');
		$query->fields('textbook_companion_chapter');
		$query->condition('number', $chapter_number);
		$query->condition('preference_id', $preference_id);
		$query->range(0, 1);
		$chapter_q = $query->execute();
		if ($chapter_data = $chapter_q->fetchObject())
		{
			echo $chapter_data->name;
			return;
		} //$chapter_data = $chapter_q->fetchObject()
	} //$query_type == 'chapter_title'
	else if ($query_type == 'example_exists')
	{
		$chapter_number = arg(3);
		$preference_id = arg(4);
		$example_number = arg(5);
		$chapter_id = 0;
		/* $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE number = %d AND preference_id = %d LIMIT 1", $chapter_number, $preference_id);*/
		$query = db_select('textbook_companion_chapter');
		$query->fields('textbook_companion_chapter');
		$query->condition('number', $chapter_number);
		$query->condition('preference_id', $preference_id);
		$query->range(0, 1);
		$chapter_q = $query->execute();
		if (!$chapter_data = $chapter_q->fetchObject())
		{
			echo '';
			return;
		} //!$chapter_data = $chapter_q->fetchObject()
		else
		{
			$chapter_id = $chapter_data->id;
		}
		/*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND number = '%s' LIMIT 1", $chapter_id, $example_number);*/
		$query = db_select('textbook_companion_example');
		$query->fields('textbook_companion_example');
		$query->condition('chapter_id', $chapter_id);
		$query->condition('number', $example_number);
		$query->range(0, 1);
		$example_q = $query->execute();
		if ($example_data = $example_q->fetchObject())
		{
			if ($example_data->approval_status == 1)
				echo 'Warning! Example already approved. You cannot upload the same example again.';
			else
				echo 'Warning! Example already uploaded. Delete the example and reupload it.';
			return;
		} //$example_data = $example_q->fetchObject()
	} //$query_type == 'example_exists'
	echo '';
}
/*  Non aicte book proposal form */
function book_proposal_nonaicte_form($form, &$form_state)
{
	global $user;
	$form = array();
	$form['imp_notice'] = array(
		'#type' => 'item',
		'#markup' => '<font color="red"><b>Please fill up this form carefully as the details entered here will be exactly written in the Textbook Companion and also follow the additional guidelines.</b></font>'
	);
	$form['guidelines'] = array(
		'#type' => 'fieldset',
		'#title' => t('Guidelines'),
		'#attributes' => array(
			'style' => 'font-weight: bold'
		),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE
	);
	$form['guidelines']['book'] = array(
		'#type' => 'item',
		'#required' => TRUE,
		'#markup' => '<ul style="list-style-type:disc;font-weight: normal">
               <li>All the fields are compulsory</li>
               <li>Proof (example: syllabus) to the usage/ popularity of the textbook must be provided in the references box below</li>
               <li>Please make sure that the book proposed by you has <b>at least 80</b> examples which include numerical computations and which can be coded in Scilab</li>
               <li>If the book has less than 80 examples the <a href="tbc_honorarium" target="_blank">honorarium</a> will be provided on a pro-rata basis </li>
               <li>Make sure the book you propose is not already an <a href="aicte_proposal" target="_blank">AICTE recommended book</a>, <a href="Completed_Books" target="_blank">completed book</a> 
                   or a <a href="Books_Progress" target="_blank">book in progress</a> </li>
               <li>You will be intimated about the approval or rejection of your suggestion via e-mail</li>

</ul> '
	);
	$form['full_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Full Name'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
	);
	$form['email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#size' => 30,
		'#value' => $user->mail,
		'#disabled' => TRUE
	);
	$form['mobile'] = array(
		'#type' => 'textfield',
		'#title' => t('Mobile No.'),
		'#size' => 30,
		'#maxlength' => 15,
		'#required' => TRUE
	);
	$form['gender'] = array(
		'#type' => 'radios',
		'#title' => t('Gender'),
		'#options' => array(
			'M' => 'Male',
			'F' => 'Female'
		),
		'#required' => TRUE
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
	);*/
	$form['course'] = array(
		'#type' => 'textfield',
		'#title' => t('Course'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
	);
	$form['branch'] = array(
		'#type' => 'select',
		'#title' => t('Department/Branch'),
		'#options' => _tbc_list_of_departments(),
		'#required' => TRUE
	);
	$form['university'] = array(
		'#type' => 'textfield',
		'#title' => t('University/Institute'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
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
		'#validated' => TRUE
	);
	$form['other_country'] = array(
		'#type' => 'textfield',
		'#title' => t('Other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your country name')
		),
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
		'#required' => False,
		'#attributes' => array(
			'placeholder' => 'Enter pincode....'
		)
	);
	$form['faculty'] = array(
		'#type' => 'hidden',
		'#value' => 'None',
		'#title' => t('College Teacher/Professor'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE
	);
	$form['faculty_email'] = array(
		'#type' => 'hidden',
		'#value' => 'None',
		'#title' => t('Teacher/Professor Email Id'),
		'#default_value' => '@email.com',
		'#size' => 30,
		'#maxlength' => 50
	);
	$form['reviewer'] = array(
		'#type' => 'hidden',
		'#value' => 'None',
		'#title' => t('Reviewer'),
		'#size' => 30,
		'#maxlength' => 50
	);
	$form['version'] = array(
		'#type' => 'textfield',
		'#title' => t('Scilab Version'),
		'#description' => t('Input version format should be like 5.4 or 5.4.1'),
		'#size' => 10,
		'#required' => TRUE,
		'#maxlength' => 20
	);
	$form['completion_date'] = array(
		'#type' => 'textfield',
		'#title' => t('Expected Date of Completion'),
		'#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
		'#size' => 10,
		'#maxlength' => 10,
		'#required' => TRUE
	);
	$form['operating_system'] = array(
		'#type' => 'textfield',
		'#title' => t('Operating System'),
		'#required' => TRUE,
		'#size' => 30,
		'#maxlength' => 50
	);
	$reason = array(
		'Used in more than one University' => t('Used in more than one University'),
		'The book has multiple editions' => t('The book has multiple editions'),
		'Extremely useful' => t('Extremely useful'),
		'Other reason' => t('Any other reason state below')
	);
	$form['reason'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Reasons'),
		'#options' => $reason,
		'#required' => TRUE
	);
	$form['other_reason'] = array(
		'#type' => 'textarea',
		'#size' => 300,
		'#maxlength' => 300,
		'#states' => array(
			'visible' => array(
				':input[name="reason[Other reason]"]' => array(
					'checked' => TRUE
				)
			)
		)
		//'#required' => FALSE,
	);
	$form['proposal_type'] = array(
		'#type' => 'hidden',
		'#default_value' => '1',
		'#required' => FALSE
	);
	$form['reference'] = array(
		'#type' => 'textarea',
		'#title' => t('Reference'),
		'#required' => TRUE,
		'#size' => 1000,
		'#attributes' => array(
			'placeholder' => 'Links of the syllabus must be provided....'
		)
	);
	$form['preference1'] = array(
		'#type' => 'fieldset',
		'#title' => t('Book Preference'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE
	);
	$form['preference1']['book1'] = array(
		'#type' => 'textfield',
		'#title' => t('Title of the book'),
		'#size' => 30,
		'#maxlength' => 100,
		'#required' => TRUE,
		'#default_value' => $row1->book,
		'#disabled' => ($row1->book ? TRUE : FALSE)
	);
	$form['preference1']['author1'] = array(
		'#type' => 'textfield',
		'#title' => t('Author Name'),
		'#size' => 30,
		'#maxlength' => 100,
		'#required' => TRUE,
		'#default_value' => $row1->author,
		'#disabled' => ($row1->author ? TRUE : FALSE)
	);
	$form['preference1']['isbn1'] = array(
		'#type' => 'textfield',
		'#title' => t('ISBN No'),
		'#size' => 30,
		'#maxlength' => 25,
		'#required' => TRUE,
		'#default_value' => $row1->isbn,
		'#disabled' => ($row1->isbn ? TRUE : FALSE)
	);
	$form['preference1']['publisher1'] = array(
		'#type' => 'textfield',
		'#title' => t('Publisher & Place'),
		'#size' => 30,
		'#maxlength' => 50,
		'#required' => TRUE,
		'#default_value' => $row1->publisher
	);
	$form['preference1']['edition1'] = array(
		'#type' => 'textfield',
		'#title' => t('Edition'),
		'#size' => 4,
		'#maxlength' => 2,
		'#required' => TRUE,
		'#default_value' => $row1->edition
	);
	$form['preference1']['year1'] = array(
		'#type' => 'textfield',
		'#title' => t('Year of pulication'),
		'#size' => 4,
		'#maxlength' => 4,
		'#required' => TRUE,
		'#default_value' => $row1->year
	);
	$form['preference1']['book_category_1'] = array(
		'#type' => 'select',
		'#title' => t('Category'),
		'#options' => _tbc_list_of_category(),
		'#required' => TRUE
	);
	$form['samplefile'] = array(
		'#type' => 'fieldset',
		'#title' => t('<span class="form-required form-item" title="This field is required.">Sample Source Files *</span>'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE
	);
	$form['samplefile']['samplefile1'] = array(
		'#type' => 'file',
		'#title' => t('Upload sample source file'),
		'#size' => 48,
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions : ') . variable_get('textbook_companion_sample_source_file_extensions', '') . '</span>'
	);
	$form['termconditions'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Terms And Conditions'),
		'#options' => array(
			'status' => t('<a href="term-and-conditions" target="_blank">I agree to the Terms and Conditions</a>')
		),
		'#required' => TRUE
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}
function book_proposal_nonaicte_form_validate($form, &$form_state)
{
	/* mobile */
	if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
		form_set_error('mobile', t('Invalid mobile number'));
	/* date of completion */
	if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
		form_set_error('completion_date', t('Invalid expected date of completion'));
	list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
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
	/* year of publication */
	if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year1']))
		form_set_error('year1', t('Invalid year of pulication for Book Preference 1'));
	/* year of publication */
	$cur_year = date('Y');
	if ((int) $form_state['values']['year1'] > $cur_year)
		form_set_error('year1', t('Year of pulication should be not in the future for Book Preference 1'));
	/* isbn */
	if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn1']))
		form_set_error('isbn1', t('Invalid ISBN for Book Preference 1'));
	if (!preg_match('/^\d+(?:\.\d+)+/', $form_state['values']['version']))
		form_set_error('version', t('Please enter correct version in speacifed format'));
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
	if (isset($_FILES['files']))
	{
		/* check if atleast one source or result file is uploaded */
		if (!($_FILES['files']['name']['samplefile1']))
			form_set_error('samplefile1', t('Please upload sample code main or source file.'));
		/* check for valid filename extensions */
		foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
		{
			if ($file_name)
			{
				/* checking file type */
				if (strstr($file_form_name, 'sample'))
					$file_type = 'S';
				else
					$file_type = 'U';
				$allowed_extensions_str = '';
				switch ($file_type)
				{
					case 'S':
						$allowed_extensions_str = variable_get('textbook_companion_sample_source_file_extensions', '');
						break;
				} //$file_type
				$allowed_extensions = explode(',', $allowed_extensions_str);
				$allowd_file = strtolower($_FILES['files']['name'][$file_form_name]);
				$temp_extension = end(explode('.', $allowd_file));
				if (!in_array($temp_extension, $allowed_extensions))
					form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
				if ($_FILES['files']['size'][$file_form_name] <= 0)
					form_set_error($file_form_name, t('File size cannot be zero.'));
				/* check if valid file name */
				if (!textbook_companion_check_valid_filename($_FILES['files']['name'][$file_form_name]))
					form_set_error($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
			} //$file_name
		} //$_FILES['files']['name'] as $file_form_name => $file_name
	} //isset($_FILES['files'])
	return;
}
function book_proposal_nonaicte_form_submit($form, &$form_state)
{
	global $user;
	$root_path = textbook_companion_samplecode_path();
	$selections = variable_get("aicte_" . $user->uid, "");
	$tbc_to_emails = variable_get("textbook_companion_emails_all");
	if (!$user->uid)
	{
		drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
		return;
	} //!$user->uid
	/* completion date to timestamp */
	list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
	$completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
	/* Reasons for suggesting a book*/
	if (isset($_POST['reason']))
	{
		if (!($form_state['values']['other_reason']))
		{
			$my_reason = implode(", ", $_POST['reason']);
		} //!($form_state['values']['other_reason'])
		else
		{
			$my_reason = implode(", ", $_POST['reason']);
			$my_reason = $my_reason . "-" . " " . $form_state['values']['other_reason'];
		}
	} //isset($_POST['reason'])
	/************************************/
	$scilab_version = 'scilab ' . $form_state['values']['version'];
	$query = "
INSERT INTO {textbook_companion_proposal}
    (uid, approver_uid, full_name, mobile, gender, how_project, course, branch, university, country, city, pincode, state, faculty, reviewer, reference, completion_date, creation_date, approval_date, proposal_status, message, scilab_version, operating_system, teacher_email, proposal_type, reason, samplefilepath,proposed_completion_date) VALUES (:uid, :approver_uid, :full_name, :mobile, :gender, :how_project, :course, :branch, :university, :country, :city, :pincode, :state,
:faculty, :reviewer, :reference, :completion_date, :creation_date, :approval_date, :proposal_status, :message, :scilab_version,
 :operating_system, :teacher_email, :proposal_type, :reason, :samplefilepath,:proposed_completion_date)
";
	// $result = db_query($query);
	$args = array(
		":uid" => $user->uid,
		":approver_uid" => 0,
		":full_name" => _tbc_sentence_case($form_state['values']['full_name']),
		":mobile" => $form_state['values']['mobile'],
		":gender" => $form_state['values']['gender'],
		":how_project" => 'Not available',
		":course" => $form_state['values']['course'],
		":branch" => $form_state['values']['branch'],
		":university" => $form_state['values']['university'],
		":country" => $form_state['values']['country'],
		":city" => $form_state['values']['city'],
		":pincode" => $form_state['values']['pincode'],
		":state" => $form_state['values']['all_state'],
		":faculty" => _tbc_sentence_case($form_state['values']['faculty']),
		":reviewer" => _tbc_sentence_case($form_state['values']['reviewer']),
		":reference" => strtolower($form_state['values']['reference']),
		":completion_date" => $completion_date_timestamp,
		":creation_date" => time(),
		":approval_date" => time(),
		":proposal_status" => 0,
		":message" => 'None',
		":scilab_version" => $scilab_version,
		":operating_system" => _tbc_sentence_case($form_state['values']['operating_system']),
		":teacher_email" => $form_state['values']['faculty_email'],
		":proposal_type" => $form_state['values']['proposal_type'],
		":reason" => $my_reason,
		":samplefilepath" => "",
		":proposed_completion_date" => $completion_date_timestamp
	);
	$result = db_query($query, $args, array(
		'return' => Database::RETURN_INSERT_ID
	));
	$dest_path = $result . '/';
	if (!is_dir($root_path . $dest_path))
	{
		mkdir($root_path . $dest_path);
	} //!is_dir($root_path . $dest_path)
	/* uploading files */
	foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
	{
		if ($file_name)
		{
			/* checking file type */
			$file_type = 'S';
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
				$query = "UPDATE {textbook_companion_proposal} SET samplefilepath = :samplefilepath WHERE id = :id";
				$args = array(
					":samplefilepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
					":id" => $result
				);
				$updateresult = db_query($query, $args);
				drupal_set_message($file_name . ' uploaded successfully.', 'status');
			} //move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])
			else
			{
				drupal_set_message('Error uploading file : ' . $dest_path . $file_name, 'error');
			}
		} //$file_name
	} //$_FILES['files']['name'] as $file_form_name => $file_name
	if (!$result)
	{
		drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
		return;
	} //!$result
	/* proposal id */
	$proposal_id = $result;
	/* inserting first book preference */
	if ($form_state['values']['book1'])
	{
		$query = "INSERT INTO {textbook_companion_preference}
      (proposal_id, pref_number, book, author, isbn, publisher, edition, year, category, approval_status, nonaicte_book) VALUES
 (:proposal_id, :pref_number, :book, :author, :isbn, :publisher, :edition, :year, :category, :approval_status, :nonaicte_book)";
		$args = array(
			":proposal_id" => $proposal_id,
			":pref_number" => 1,
			":book" => _tbc_sentence_case($form_state['values']['book1']),
			":author" => _tbc_sentence_case($form_state['values']['author1']),
			":isbn" => $form_state['values']['isbn1'],
			":publisher" => _tbc_sentence_case($form_state['values']['publisher1']),
			":edition" => $form_state['values']['edition1'],
			":year" => $form_state['values']['year1'],
			":category" => $form_state['values']['book_category_1'],
			":approval_status" => 0,
			":nonaicte_book" => 1
		);
		$result = db_query($query, $args);
		if (!$result)
		{
			drupal_set_message(t('Error receiving your first book preference.'), 'error');
		} //!$result
	} //$form_state['values']['book1']
	/* sending email */
	$email_to = $user->mail;
	$from = variable_get('textbook_companion_from_email', '');
	$bcc = variable_get('textbook_companion_emails', '');
	$cc = variable_get('textbook_companion_cc_emails', '');
	$params['nonaicte_proposal_received']['proposal_id'] = $proposal_id;
	$params['nonaicte_proposal_received']['user_id'] = $user->uid;
	$params['nonaicte_proposal_received']['headers'] = array(
		'From' => $from,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
	if (!drupal_mail('textbook_companion', 'nonaicte_proposal_received', $email_to, language_default(), $params, $from, TRUE))
		drupal_set_message('Error sending email message.', 'error');
	drupal_set_message(t('We have received your book proposal. We will get back to you soon.'), 'status');
	drupal_goto('');
}
