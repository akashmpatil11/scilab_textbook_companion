<?php
/**
 * Implementation of hook_mail().
 */
function textbook_companion_mail($key, &$message, $params)
{
  global $user;
  $language = $message['language'];
  /*$tbc_bcc_emails = array(
  'Bcc' => variable_get('textbook_companion_emails', ''),
  );*/
  switch ($key)
  {
    case 'proposal_received':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /* initializing data */
      /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d LIMIT 1", $params['proposal_received']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);*/
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['proposal_received']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      $samplecodefilename = "";
      if (strlen($proposal_data->samplefilepath) >= 5)
      {
        $samplecodefilename = substr($proposal_data->samplefilepath, strrpos($proposal_data->samplefilepath, '/') + 1);
      } //strlen($proposal_data->samplefilepath) >= 5
      else
      {
        $samplecodefilename = "Not provided";
      }
      /*$preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_received']['proposal_id'], 1);
      $preference1_data = db_fetch_object($preference1_q);*/
      $query = "
        SELECT * FROM list_of_category loc
        JOIN textbook_companion_preference tcp ON loc.category_id = tcp.category
        WHERE proposal_id = :proposal_id AND pref_number = :pref_number LIMIT 1
        ";
      $args = array(
        ':proposal_id' => $params['proposal_received']['proposal_id'],
        ':pref_number' => 1
      );
      $preference1_q = db_query($query, $args);
      $preference1_data = $preference1_q->fetchObject();
      /*$preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_received']['proposal_id'], 2);
      $preference2_data = db_fetch_object($preference2_q);*/
      $query = "
        SELECT * FROM list_of_category loc
        JOIN textbook_companion_preference tcp ON loc.category_id = tcp.category
        WHERE proposal_id = :proposal_id AND pref_number = :pref_number LIMIT 1
        ";
      $args = array(
        ':proposal_id' => $params['proposal_received']['proposal_id'],
        ':pref_number' => 2
      );
      $preference2_q = db_query($query, $args);
      $preference2_data = $preference2_q->fetchObject();
      if($preference2_data->book){
      $book_preference2 = '
Book Preference 2 :-
Title of the book : ' . $preference2_data->book . '
Author name : ' . $preference2_data->author . '
ISBN No. : ' . $preference2_data->isbn . '
Publisher and Place : ' . $preference2_data->publisher . '
Edition : ' . $preference2_data->edition . '
Year of publication : ' . $preference2_data->year . '
Category : ' . $preference2_data->category_name . '
';
      }
      $query = "
        SELECT * FROM list_of_category loc
        JOIN textbook_companion_preference tcp ON loc.category_id = tcp.category
        WHERE proposal_id = :proposal_id AND pref_number = :pref_number LIMIT 1
        ";
      $args = array(
        ':proposal_id' => $params['proposal_received']['proposal_id'],
        ':pref_number' => 3
      );
      $preference3_q = db_query($query, $args);
      $preference3_data = $preference3_q->fetchObject();
      if($preference3_data->book){
      $book_preference3 = '
Book Preference 3 :-
Title of the book : ' . $preference3_data->book . '
Author name : ' . $preference3_data->author . '
ISBN No. : ' . $preference3_data->isbn . '
Publisher and Place : ' . $preference3_data->publisher . '
Edition : ' . $preference3_data->edition . '
Year of publication : ' . $preference3_data->year . '
Category : ' . $preference3_data->category_name . '
';
      }
      /*$query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_received']['proposal_id']);
      $query->condition('pref_number', 3);
      $query->range(0, 1);
      $result = $query->execute();
      $preference3_data =$result->fetchObject();*/
      $user_data = user_load($params['proposal_received']['user_id']);
      $message['headers'] = $params['proposal_received']['headers'];
      $message['subject'] = t('[!site_name] Your book proposal has been received', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have received your following book proposal:

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' . $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' . $proposal_data->reviewer . '
Expected date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '

Your Book Preferences :

Book Preference 1 :-
Title of the book : ' . $preference1_data->book . '
Author name : ' . $preference1_data->author . '
ISBN No. : ' . $preference1_data->isbn . '
Publisher and Place : ' . $preference1_data->publisher . '
Edition : ' . $preference1_data->edition . '
Year of publication : ' . $preference1_data->year . '
Category : ' . $preference1_data->category_name . '
'. ' '.
$book_preference2
.' '.
$book_preference2
.'

Uploaded Sample Code : ' . $samplecodefilename . '

Your proposal is under review and you will soon receive an email from us regarding the same.

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'proposal_disapproved':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /* initializing data */
      /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d LIMIT 1", $params['proposal_disapproved']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);*/
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['proposal_disapproved']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      /*$preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_disapproved']['proposal_id'], 1);
      $preference1_data = db_fetch_object($preference1_q);*/
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_disapproved']['proposal_id']);
      $query->condition('pref_number', 1);
      $query->range(0, 1);
      $result = $query->execute();
      $preference1_data = $result->fetchObject();
      /*$preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_disapproved']['proposal_id'], 2);
      $preference2_data = db_fetch_object($preference2_q);*/
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_disapproved']['proposal_id']);
      $query->condition('pref_number', 2);
      $query->range(0, 1);
      $result = $query->execute();
      $preference2_data = $result->fetchObject();
      if($preference2_data->book){
      $book_preference2 = '
Book Preference 2 :-
Title of the book : ' . $preference2_data->book . '
Author name : ' . $preference2_data->author . '
ISBN No. : ' . $preference2_data->isbn . '
Publisher and Place : ' . $preference2_data->publisher . '
Edition : ' . $preference2_data->edition . '
Year of publication : ' . $preference2_data->year . '
Category : ' . $preference2_data->category_name . '
';
      }
      /*$preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_disapproved']['proposal_id'], 3);
      $preference3_data = db_fetch_object($preference3_q);*/
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_disapproved']['proposal_id']);
      $query->condition('pref_number', 3);
      $query->range(0, 1);
      $result = $query->execute();
      $preference3_data = $result->fetchObject();
      if($preference3_data->book){
      $book_preference3 = '
Book Preference 3 :-
Title of the book : ' . $preference3_data->book . '
Author name : ' . $preference3_data->author . '
ISBN No. : ' . $preference3_data->isbn . '
Publisher and Place : ' . $preference3_data->publisher . '
Edition : ' . $preference3_data->edition . '
Year of publication : ' . $preference3_data->year . '
Category : ' . $preference3_data->category_name . '
';
      }
      $user_data = user_load($params['proposal_disapproved']['user_id']);
      $message['headers'] = $params['proposal_disapproved']['headers'];
      $message['subject'] = t('[!site_name] Your book proposal has been disapproved', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      if ($proposal_data->proposal_type != 1)
      {
        $message['body'] = array(
          'body' => t('
Dear !user_name,

Your following book proposal has been disapproved:

Reason for disapproval: ' . $proposal_data->message . '

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' . $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' . $proposal_data->reviewer . '
Expected date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '

Your Book Preferences :

Book Preference 1 :-
Title of the book : ' . $preference1_data->book . '
Author name : ' . $preference1_data->author . '
ISBN No. : ' . $preference1_data->isbn . '
Publisher and Place : ' . $preference1_data->publisher . '
Edition : ' . $preference1_data->edition . '
Year of publication : ' . $preference1_data->year . '
Category : ' . $preference1_data->category_name . ''.

$book_preference2
.' '.
$book_preference2
.'

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
            '!site_name' => variable_get('site_name', ''),
            '!user_name' => $user_data->name
          ), array(
            'language' => $language->language
          ))
        );
      } //$proposal_data->proposal_type != 1
      else
      {
        //Non AICTE book proposal dissaprove//
        $message['body'] = array(
          'body' => t('
Dear !user_name,

Your following book proposal has been disapproved:

Reason for disapproval: ' . $proposal_data->message . '

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' . $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' . $proposal_data->reviewer . '
Expected date of completion : ' . date('d-m-Y', $proposal_data->completion_date) . '

Your Book Preferences :

Book Preference :-
Title of the book : ' . $preference1_data->book . '
Author name : ' . $preference1_data->author . '
ISBN No. : ' . $preference1_data->isbn . '
Publisher and Place : ' . $preference1_data->publisher . '
Edition : ' . $preference1_data->edition . '
Year of publication : ' . $preference1_data->year . '



Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
            '!site_name' => variable_get('site_name', ''),
            '!user_name' => $user_data->name
          ), array(
            'language' => $language->language
          ))
        );
      }
      break;
    /* Non AICTE Book Proposal */
    case 'nonaicte_proposal_received':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /* initializing data */
      /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d LIMIT 1", $params['proposal_received']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);*/
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['nonaicte_proposal_received']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      $samplecodefilename = "";
      if (strlen($proposal_data->samplefilepath) >= 5)
      {
        $samplecodefilename = substr($proposal_data->samplefilepath, strrpos($proposal_data->samplefilepath, '/') + 1);
      } //strlen($proposal_data->samplefilepath) >= 5
      else
      {
        $samplecodefilename = "Not provided";
      }
      $query = "
                SELECT * FROM list_of_category loc
                JOIN textbook_companion_preference tcp ON loc.category_id = tcp.category
                WHERE proposal_id = :proposal_id AND pref_number = :pref_number LIMIT 1
               ";
      $args = array(
        ':proposal_id' => $params['nonaicte_proposal_received']['proposal_id'],
        ':pref_number' => 1
      );
      $preference1_q = db_query($query, $args);
      $preference1_data = $preference1_q->fetchObject();
      $user_data = user_load($params['nonaicte_proposal_received']['user_id']);
      $message['headers'] = $params['nonaicte_proposal_received']['headers'];
      $message['subject'] = t('[!site_name] Your book proposal has been received', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have received your following book proposal:

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' . $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' . $proposal_data->reviewer . '
Expected date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '

Your Non AICTE Book Preferences:

Book Preference :-
Title of the book : ' . $preference1_data->book . '
Author name : ' . $preference1_data->author . '
ISBN No. : ' . $preference1_data->isbn . '
Publisher and Place : ' . $preference1_data->publisher . '
Edition : ' . $preference1_data->edition . '
Year of publication : ' . $preference1_data->year . '
Category : ' . $preference1_data->category_name . '


Uploaded Sample Code : ' . $samplecodefilename . '

Your proposal is under review and you will soon receive an email from us regarding the same.

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'nonaicte_proposal_to_pi':
      /************ fetch proposal data ****************/
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['nonaicte_proposal_received']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      /************ fetch book preference data  ****************/
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['nonaicte_proposal_received']['proposal_id']);
      $query->condition('pref_number', 1);
      $query->range(0, 1);
      $result = $query->execute();
      $preference1_data = $result->fetchObject();
      $user_data = user_load($params['nonaicte_proposal_received']['user_id']);
      $message['headers'] = $params['nonaicte_proposal_received']['headers'];
      $message['subject'] = t('[!site_name] New Non AICTE book suggestion has been received', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      /******************* Email Template for not aicte book suggestion  **************************/
      $message['body'] = array(
        'body' => t('
Dear all,

We have received following Non AICTE book suggestion:

Full Name : ' . $proposal_data->full_name . '
University/Institute :' . $proposal_data->university . '


Reason(s): ' . $proposal_data->reason . '


Your Non AICTE Book Preference :

Title of the book : ' . $preference1_data->book . '
Author name : ' . $preference1_data->author . '
ISBN No. : ' . $preference1_data->isbn . '
Publisher and Place : ' . $preference1_data->publisher . '
Edition : ' . $preference1_data->edition . '
Year of publication : ' . $preference1_data->year . '

Reference :- ' . $proposal_data->reference
        //Please click here to provide your feedback: http://scilab.in/comments
          . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'proposal_approved':
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['proposal_approved']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_approved']['proposal_id']);
      $query->condition('approval_status', 1);
      $query->range(0, 1);
      $result = $query->execute();
      $approved_preference_data = $result->fetchObject();
      $user_data = user_load($params['proposal_approved']['user_id']);
      $message['headers'] = $params['proposal_approved']['headers'];
      $message['subject'] = t('[!site_name] Your book proposal has been approved', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Your following book proposal has been approved:

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' . $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' . $proposal_data->reviewer . '
Expected date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '

Title of the book : ' . $approved_preference_data->book . '
Author name : ' . $approved_preference_data->author . '
ISBN No. : ' . $approved_preference_data->isbn . '
Publisher and Place : ' . $approved_preference_data->publisher . '
Edition : ' . $approved_preference_data->edition . '
Year of publication : ' . $approved_preference_data->year . '

According the new Textbook Companion procedure, a student doing a textbook companion is not required to have a mentor. For more details check the links given below.
http://scilab.in/tbc_honorarium
http://scilab.in/Textbook_Companion_Internship

Please contact us by sending an e-mail to textbook@scilab.in in case you wish to cancel this book proposal.

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'proposal_edit':
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['proposal_edit']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_edit']['proposal_id']);
      $query->condition('approval_status', 1);
      $query->range(0, 1);
      $result = $query->execute();
      $approved_preference_data = $result->fetchObject();
      $user_data = user_load($params['proposal_edit']['user_id']);
      $message['headers'] = $params['proposal_edit']['headers'];
      $message['subject'] = t('[!site_name] Your book proposal has been edited', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
  Dear !user_name,

  Your following book proposal details has been updated:

  Full Name : ' . $proposal_data->full_name . '
  Email : ' . $user_data->mail . '
  Mobile : ' . $proposal_data->mobile . '
  Course : ' . $proposal_data->course . '
  Department/Branch : ' . $proposal_data->branch . '
  University/Institute : ' . $proposal_data->university . '
  College Teacher / Professor : ' . $proposal_data->faculty . '
  Reviewer : ' . $proposal_data->reviewer . '
  Expected date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '

  Title of the book : ' . $approved_preference_data->book . '
  Author name : ' . $approved_preference_data->author . '
  ISBN No. : ' . $approved_preference_data->isbn . '
  Publisher and Place : ' . $approved_preference_data->publisher . '
  Edition : ' . $approved_preference_data->edition . '
  Year of publication : ' . $approved_preference_data->year . '

  Please contact us by sending an e-mail to textbook@scilab.in in case you wish to cancel this book proposal.

  Best Wishes,

  Scilab TBC Team,
  FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'proposal_completed':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d LIMIT 1", $params['proposal_completed']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);*/
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['proposal_completed']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      /*$approved_preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $params['proposal_completed']['proposal_id']);
      $approved_preference_data = db_fetch_object($approved_preference_q);*/
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('proposal_id', $params['proposal_completed']['proposal_id']);
      $query->condition('approval_status', 1);
      $query->range(0, 1);
      $result = $query->execute();
      $approved_preference_data = $result->fetchObject();
      $user_data = user_load($params['proposal_completed']['user_id']);
      $message['headers'] = $params['proposal_completed']['headers'];
      $message['subject'] = t('[!site_name] Congratulations for completion of the book.', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Following book has been completed sucessfully by you:

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' . $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' . $proposal_data->reviewer . '
Proposed date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '
Actual date of completion : ' . date('d-m-Y', $proposal_data->completion_date) . '

Title of the book : ' . $approved_preference_data->book . '
Author name : ' . $approved_preference_data->author . '
ISBN No. : ' . $approved_preference_data->isbn . '
Publisher and Place : ' . $approved_preference_data->publisher . '
Edition : ' . $approved_preference_data->edition . '
Year of publication : ' . $approved_preference_data->year . '

Your book is now available at following link to download.

http://scilab.in/textbook_run/' . $approved_preference_data->id . '

Now you should be able to propose a new book...

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    /**
      *Adding mail details for editing book details
      */
    case 'book_edit':
      $query = db_select('textbook_companion_proposal');
      $query->fields('textbook_companion_proposal');
      $query->condition('id', $params['book_edit']['proposal_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $proposal_data = $result->fetchObject();
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('id', $params['book_edit']['preference_id']);
      $result = $query->execute();
      $approved_preference_data = $result->fetchObject();
      $user_data = user_load($params['book_edit']['user_id']);
      $message['headers'] = $params['book_edit']['headers'];
      $message['subject'] = t('[!site_name] Your book details has been updated', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
  Dear !user_name,

  Your following book details has been updated:

  Full Name : ' . $proposal_data->full_name . '
  Reviewer : ' . $proposal_data->reviewer . '
  Expected date of completion : ' . date('d-m-Y', $proposal_data->proposed_completion_date) . '

  Title of the book : ' . $approved_preference_data->book . '
  Author name : ' . $approved_preference_data->author . '
  ISBN No. : ' . $approved_preference_data->isbn . '
  Publisher and Place : ' . $approved_preference_data->publisher . '
  Edition : ' . $approved_preference_data->edition . '
  Year of publication : ' . $approved_preference_data->year . '

  Please contact us by sending an e-mail to textbook@scilab.in in case you wish to cancel this book proposal.

  Regards,

  Scilab TBC Team,
  FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
      /**
        *Adding mail details for status update book details
        */
      case 'book_status_updated':
        $query = db_select('textbook_companion_preference');
        $query->fields('textbook_companion_preference');
        $query->condition('id', $params['book_status_updated']['preference_id']);
        $result = $query->execute();
        $preference_data = $result->fetchObject();
        $book_status = '';
        switch ($preference_data->book_status)
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
        $user_data = user_load($params['book_status_updated']['user_id']);
        $message['headers'] = $params['book_status_updated']['headers'];
        $message['subject'] = t('[!site_name] Your book status has been changed.', array(
          '!site_name' => variable_get('site_name', '')
        ), array(
          'language' => $language->language
        ));
        $message['body'] = array(
          'body' => t('
    Dear !user_name,

    Your following book status has been updated:

    Title of the book : ' . $preference_data->book . '
    Author name : ' . $preference_data->author . '
    Status :'. $book_status.'

    Regards,

    Scilab TBC Team,
    FOSSEE, IIT Bombay', array(
            '!site_name' => variable_get('site_name', ''),
            '!user_name' => $user_data->name
          ), array(
            'language' => $language->language
          ))
        );
        break;
    case 'example_uploaded':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $params['example_uploaded']['example_id']);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_example');
      $query->fields('textbook_companion_example');
      $query->condition('id', $params['example_uploaded']['example_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $example_data = $result->fetchObject();
      $query = db_select('textbook_companion_chapter');
      $query->fields('textbook_companion_chapter');
      $query->condition('id', $example_data->chapter_id);
      $query->range(0, 1);
      $result = $query->execute();
      $chapter_data = $result->fetchObject();
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('id', $chapter_data->preference_id);
      $query->range(0, 1);
      $result = $query->execute();
      $preference_data = $result->fetchObject();
      $user_data = user_load($params['example_uploaded']['user_id']);
      $message['headers'] = $params['example_uploaded']['headers'];
      $message['subject'] = t('[!site_name] You have uploaded example for Textbook Companion', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

You have uploaded the following solution :

Title of the book : ' . $preference_data->book . '
Title of the chapter : ' . $chapter_data->name . '
Example number : ' . $example_data->number . '
Caption : ' . $example_data->caption . '

You shall be notified after the solution is reviewed

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'example_updated':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $params['example_updated']['example_id']);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_example');
      $query->fields('textbook_companion_example');
      $query->condition('id', $params['example_updated']['example_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $example_data = $result->fetchObject();
      $query = db_select('textbook_companion_chapter');
      $query->fields('textbook_companion_chapter');
      $query->condition('id', $example_data->chapter_id);
      $query->range(0, 1);
      $result = $query->execute();
      $chapter_data = $result->fetchObject();
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('id', $chapter_data->preference_id);
      $query->range(0, 1);
      $result = $query->execute();
      $preference_data = $result->fetchObject();
      $user_data = user_load($params['example_updated']['user_id']);
      $message['headers'] = $params['example_updated']['headers'];
      $message['subject'] = t('[!site_name] You have updated example for Textbook Companion', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

You have updated the following example:

Title of the book : ' . $preference_data->book . '
Title of the chapter : ' . $chapter_data->name . '
Example number : ' . $example_data->number . '
Caption : ' . $example_data->caption . '

The example is still under review. You will be notified when it has been approved.

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'example_updated_admin':
      // bcc to textbook_companion_emails
      $message['headers'] += $tbc_bcc_emails;
      /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $params['example_updated_admin']['example_id']);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_example');
      $query->fields('textbook_companion_example');
      $query->condition('id', $params['example_updated_admin']['example_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $example_data = $result->fetchObject();
      $user_data = user_load($params['example_updated_admin']['user_id']);
      $message['subject'] = t('[!site_name] Reviewer have updated example', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Reviewer have updated the following example:

Example number : ' . $example_data->number . '
Caption : ' . $example_data->caption . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'example_approved':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $params['example_approved']['example_id']);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_example');
      $query->fields('textbook_companion_example');
      $query->condition('id', $params['example_approved']['example_id']);
      $query->range(0, 1);
      $result = $query->execute();
      $example_data = $result->fetchObject();
      $user_data = user_load($params['example_approved']['user_id']);
      $message['headers'] = $params['example_approved']['headers'];
      $message['subject'] = t('[!site_name] Your uploaded example has been approved', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Your following example has been approved:

Example number : ' . $example_data->number . '
Caption : ' . $example_data->caption . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'example_disapproved':
      // bcc to textbook_companion_emails
      $message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['example_disapproved']['user_id']);
      $message['subject'] = t('[!site_name] Your uploaded example has been disapproved', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Your following example has been disapproved:

Example number : ' . $params['example_disapproved']['example_number'] . '
Caption : ' . $params['example_disapproved']['example_caption'] . '

Reason for dis-approval : ' . $params['example_disapproved']['message'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'example_deleted_user':
      // bcc to textbook_companion_emails
      // $message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['example_deleted_user']['user_id']);
      $message['headers'] = $params['example_deleted_user']['headers'];
      $message['subject'] = t('[!site_name] User has deleted pending example', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Your following pending example has been deleted :

Title of the Book : ' . $params['example_deleted_user']['book_title'] . '
Title of the Chapter : ' . $params['example_deleted_user']['chapter_title'] . '
Example number : ' . $params['example_deleted_user']['example_number'] . '
Caption : ' . $params['example_deleted_user']['example_caption'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'dependency_uploaded':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['dependency_uploaded']['user_id']);
      $message['headers'] = $params['example_deleted_user']['headers'];
      $dependency_files = implode(',', $params['dependency_uploaded']['dependency_names']);
      $message['subject'] = t('[!site_name] You have uploaded dependency file', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

You have uploaded following dependency files :
    ' . $dependency_files . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    /**
      *Mail content for locking book
      */
    case 'all_example_submitted':
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('id', $params['all_example_submitted']['preference_id']);
      $result = $query->execute();
      $preference_data = $result->fetchObject();
      $user_data = user_load($params['all_example_submitted']['user_id']);
      $message['headers'] = $params['all_example_submitted']['headers'];
      $message['subject'] = t('[!site_name] Your have locked the submission for your book.', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
  Dear !user_name,

  Your following book code submission has been locked:

  Title of the book : ' . $preference_data->book . '
  Author name : ' . $preference_data->author . '

  You can not upload examples any more. Book will be sent to checking.

  Regards,

  Scilab TBC Team,
  FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;

    case 'feedback_received':
      // bcc to textbook_companion_emails
      // $message['headers'] += $tbc_bcc_emails;
      $message['headers'] = $params['feedback_received']['headers'];
      $user_data = user_load($params['feedback_received']['user_id']);
      $message['subject'] = t('[!site_name] We have received your feedback', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have received your following feedback

Title of the Book: ' . $params['feedback_received']['book_title'] . '
Title of the Chapter: ' . $params['feedback_received']['chapter_number'] . ' ' . $params['feedback_received']['chapter_title'] . '
Example No.: ' . $params['feedback_received']['example_no'] . '

Your feedback :
' . $params['feedback_received']['feedback'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'chapter_title_updated':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $params['example_updated']['example_id']);
      $example_data = db_fetch_object($example_q);*/
      $query = db_select('textbook_companion_chapter');
      $query->fields('textbook_companion_chapter');
      $query->condition('id', $example_data->chapter_id);
      $query->range(0, 1);
      $result = $query->execute();
      $chapter_data = $result->fetchObject();
      $query = db_select('textbook_companion_preference');
      $query->fields('textbook_companion_preference');
      $query->condition('id', $chapter_data->preference_id);
      $query->range(0, 1);
      $result = $query->execute();
      $preference_data = $result->fetchObject();
      $user_data = user_load($params['chapter_title_updated']['user_id']);
      $message['headers'] = $params['chapter_updated']['headers'];
      $message['subject'] = t('[!site_name] You have changed the chapter name', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

You have updated the following chapter:

Title of the book : ' . $preference_data->book . '
Title of the chapter : ' . $chapter_data->name . '

The example is still under review. You will be notified when it has been approved.

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;

    case 'chapter_deleted_user':
      // bcc to textbook_companion_emails
      // $message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['chapter_deleted_user']['user_id']);
      $message['headers'] = $params['chapter_deleted_user']['headers'];
      $message['subject'] = t('[!site_name] User has deleted pending example', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

Your following chapter has been deleted by you :

Title of the Book : ' . $params['chapter_deleted_user']['book_title'] . '
Chapetr Number:' . $params['chapter_deleted_user']['chapter_number'].'
Title of the Chapter : ' . $params['chapter_deleted_user']['chapter_title'] .'

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'dependency_uploaded':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['dependency_uploaded']['user_id']);
      $message['headers'] = $params['example_deleted_user']['headers'];
      $dependency_files = implode(',', $params['dependency_uploaded']['dependency_names']);
      $message['subject'] = t('[!site_name] You have uploaded dependency file', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

You have uploaded following dependency files :
    ' . $dependency_files . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'internshipform':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $message['headers'] = $params['internshipform']['headers'];
      $user_data = user_load($params['internshipform']['user_id']);
      $message['subject'] = t('[!site_name] We have received your feedback', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have received your Internship Form Application for the book

Title of the Book: ' . $params['internshipform']['book_title'] . '
Title of the Chapter: ' . $params['internshipform']['chapter_number'] . ' ' . $params['internshipform']['chapter_title'] . '
Example No.: ' . $params['internshipform']['example_no'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'copyrighttransferform':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $message['headers'] = $params['copyrighttransferform']['headers'];
      $user_data = user_load($params['copyrighttransferform']['user_id']);
      $message['subject'] = t('[!site_name] We have received your feedback', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have received your Copyright Form Application for the book

Title of the Book: ' . $params['copyrighttransferform']['book_title'] . '
Title of the Chapter: ' . $params['copyrighttransferform']['chapter_number'] . ' ' . $params['copyrighttransferform']['chapter_title'] . '
Example No.: ' . $params['copyrighttransferform']['example_no'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'undertakingform':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $message['headers'] = $params['undertakingform']['headers'];
      $user_data = user_load($params['undertakingform']['user_id']);
      $message['subject'] = t('[!site_name] We have received your feedback', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have received your Undertaking Form Application for the book

Title of the Book: ' . $params['undertakingform']['book_title'] . '
Title of the Chapter: ' . $params['undertakingform']['chapter_number'] . ' ' . $params['undertakingform']['chapter_title'] . '
Example No.: ' . $params['undertakingform']['example_no'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'remark':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['remark']['user_id']);
      $message['headers'] = $params['remark']['headers'];
      $message['subject'] = t('[!site_name] A remark has been given.Please check your contact detail form', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

A Remark has been given.Please check your Contact Detail Form

Title of the Book: ' . $params['internshipform']['book_title'] . '
Title of the Chapter: ' . $params['internshipform']['chapter_number'] . ' ' . $params['internshipform']['chapter_title'] . '
Example No.: ' . $params['internshipform']['example_no'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'cheque_sent':
      // bcc to textbook_companion_emails
      // $message['headers'] += $tbc_bcc_emails;
      $user_data = user_load($params['cheque_sent']['user_id']);
      $message['headers'] = $params['cheque_sent']['headers'];
      $message['subject'] = t('[!site_name] We have received your feedback', array(
        '!site_name' => variable_get('site_name', '')
      ), array(
        'language' => $language->language
      ));
      $message['body'] = array(
        'body' => t('
Dear !user_name,

We have Sent Cheque for the following book proposed

Title of the Book: ' . $params['cheque_sent']['book_title'] . '
Title of the Chapter: ' . $params['cheque_sent']['chapter_number'] . ' ' . $params['cheque_sent']['chapter_title'] . '
Example No.: ' . $params['cheque_sent']['example_no'] . '

Best Wishes,

Scilab TBC Team,
FOSSEE, IIT Bombay', array(
          '!site_name' => variable_get('site_name', ''),
          '!user_name' => $user_data->name
        ), array(
          'language' => $language->language
        ))
      );
      break;
    case 'standard':
      // bcc to textbook_companion_emails
      //$message['headers'] += $tbc_bcc_emails;
      $message['headers'] = $params['standard']['headers'];
      $message['subject'] = $params['standard']['subject'];
      $message['body'] = $params['standard']['body'];
      break;
  } //$key
}
