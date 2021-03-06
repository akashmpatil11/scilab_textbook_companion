<?php
function generate_pdf()
{
	$mpath = drupal_get_path('module', 'textbook_companion');
	require($mpath . '/pdf/fpdf/fpdf.php');
	require($mpath . '/pdf/phpqrcode/qrlib.php');
	global $user;
	$x           = $user->uid;
	$proposal_id = arg(2);
	$query2      = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status=1 AND  proposal_id= :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data2       = $query2->fetchObject();
	$query3      = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id= :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data3       = $query3->fetchObject();
	$query4 = db_query("SELECT COUNT( tce.id ) AS example_count FROM textbook_companion_example tce
						LEFT JOIN textbook_companion_chapter tcc ON tce.chapter_id = tcc.id
						LEFT JOIN textbook_companion_preference tcpe ON tcc.preference_id = tcpe.id
						LEFT JOIN textbook_companion_proposal tcpo ON tcpe.proposal_id = tcpo.id
						WHERE tcpo.proposal_status =3 AND tce.approval_status =1 AND tce.approval_status=1 AND tcpo.id = :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data4       = $query4->fetchObject();
	$number_of_example = $data4->example_count;
	$gender      = array(
		'salutation' => 'Mr. /Ms.',
		'gender' => 'He/She'
	);
	if ($data3->gender)
	{
		if ($data3->gender == 'M')
		{
			$gender = array(
				'salutation' => 'Mr.',
				'gender' => 'He'
			);
		} //$data3->gender == 'M'
		else
		{
			$gender = array(
				'salutation' => 'Ms.',
				'gender' => 'She'
			);
		}
	} //$data3->gender
	$pdf = new FPDF('L', 'mm', 'Letter');
	if (!$pdf)
	{
		echo "Error!";
	} //!$pdf
	$pdf->AddPage();
	$image_bg = $mpath . "/pdf/images/bg.png";
	$pdf->Image($image_bg, 0, 0, $pdf->w, $pdf->h);
	$pdf->Rect(5, 5, 267, 207, 'D');
	$pdf->SetMargins(18, 1, 18);
	$pdf->Line(7.0, 7.0, 270.0, 7.0);
	$pdf->Line(7.0, 7.0, 7.0, 210.0);
	$pdf->Line(270.0, 210.0, 270.0, 7.0);
	$pdf->Line(7.0, 210.0, 270.0, 210.0);
	$path   = drupal_get_path('module', 'textbook_companion');
	$image1 = $mpath . "/pdf/images/scilab_logo.png";
	$pdf->Ln(15);
	$pdf->Cell(200, 8, $pdf->Image($image1, 105, 15, 0, 28), 0, 1, 'C');
	$pdf->Ln(20);
	$pdf->SetFont('Arial', 'BI', 25);
	$pdf->SetTextColor(139, 69, 19);
	$pdf->Cell(240, 8, 'Certificate of Participation', '0', 1, 'C');
	$pdf->Ln(4);
	$pdf->SetFont('Arial', 'BI', 12);
	$pdf->SetTextColor(0, 0, 0);
	$pdf->Cell(240, 8, 'This is to certify that', '0', '1', 'C');
	$pdf->Ln(4);
	$pdf->SetFont('Arial', 'BI', 25);
	$pdf->SetTextColor(139, 69, 19);
	$pdf->Cell(240, 8, $data3->full_name, '0', '1', 'C');
	$pdf->Ln(5);
	$pdf->SetFont('Arial', 'I', 12);
	if (strtolower($data3->branch) != "others")
	{
		$pdf->SetTextColor(0, 0, 0);
		$pdf->Cell(240, 8, 'from ' . $data3->university . ' has successfully', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'completed Internship under Scilab Textbook Companion', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'He/she has coded '. $number_of_example .' solved examples using Scilab from the', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'Book: ' . $data2->book . ', Author: ' . $data2->author . '.', '0', '1', 'C');
		$pdf->Ln(0);
	} //strtolower($data3->branch) != "others"
	else
	{
		$pdf->SetTextColor(0, 0, 0);
		$pdf->Cell(240, 8, 'from ' . $data3->university . ' has successfully', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'completed Internship under Scilab Textbook Companion', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'He/she has coded '. $number_of_example .' solved examples using Scilab from the', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'Book: ' . $data2->book . ', Author: ' . $data2->author . '.', '0', '1', 'C');
		$pdf->Ln(0);
	}
	$proposal_get_id = 0;
	$UniqueString    = "";
	$tempDir         = $path . "/pdf/temp_prcode/";
	$query           = db_select('textbook_companion_qr_code');
	$query->fields('textbook_companion_qr_code', array(
		'qr_code'
	));
	$query->condition('proposal_id', $proposal_id);
	$result          = $query->execute();
	$data            = $result->fetchObject();
	$DBString        = $data->qr_code;
	$proposal_get_id = $data->proposal_id;
	if ($DBString == "" || $DBString == "null")
	{
		$UniqueString = generateRandomString();
		$query        = "
				INSERT INTO textbook_companion_qr_code
				(proposal_id,qr_code)
				VALUES
				(:proposal_id,:qr_code)
				";
		$args         = array(
			":proposal_id" => $proposal_id,
			":qr_code" => $UniqueString
		);
		$result       = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$DBString == "" || $DBString == "null"
	else
	{
		$UniqueString = $DBString;
	}
	$codeContents        = "http://scilab.in/textbook_companion/certificates/verify/" . $UniqueString;
	$fileName            = 'generated_qrcode.png';
	$pngAbsoluteFilePath = $tempDir . $fileName;
	$urlRelativeFilePath = $path . "/pdf/temp_prcode/" . $fileName;
	QRcode::png($codeContents, $pngAbsoluteFilePath);
	$pdf->Cell(240, 4, '', '0', '1', 'C');
	$pdf->SetX(95);
	$pdf->write(0, 'The work done is available at ');
	$pdf->SetFont('', 'U');
	$pdf->SetTextColor(139, 69, 19);
	$pdf->write(0, 'http://scilab.in/', 'http://scilab.in/');
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->write(0, '.', '.');
	$pdf->Ln(5);
	$pdf->SetX(198);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetY(-69);
	$pdf->SetX(200);
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetFont('', '');
	//$sign = $path . "/pdf/images/sign.png";
	//$pdf->Image($sign, $pdf->GetX(), $pdf->GetY() - 20, 50, 0);
	$pdf->Cell(0, 8, 'Prof. Madhu Belur', 0, 1, 'L');
	$pdf->SetX(194);
	$pdf->Cell(0, 7, 'Principal Investigator - FOSSEE', 0, 1, 'L');
	$pdf->SetX(185);
	$pdf->Cell(0, 7, ' Dept. of Electrical Engineering, IIT Bombay.', 0, 1, 'L');
	$pdf->SetX(29);
	$pdf->SetFont('Arial', 'B', 10);
	$pdf->Cell(0, 2, $UniqueString, 0, 0, 'C');
	$image4 = $path . "/pdf/images/verify_content.png";
	$pdf->Image($image4, $pdf->GetX(), $pdf->GetY(), 20, 0);
	$pdf->SetY(-40);
	$pdf->SetX(80);
	$image3 = $path . "/pdf/images/iitb.png";
	$image2 = $path . "/pdf/images/fossee.png";
	$pdf->Image($image2, $pdf->GetX() - 15, $pdf->GetY() + 7, 40, 0);
	$pdf->Image($pngAbsoluteFilePath, $pdf->GetX() + 50, $pdf->GetY() - 5, 30, 0);
	$pdf->Image($image3, $pdf->GetX() + 110, $pdf->GetY() + 3, 15, 0);
	$pdf->Image($image4, $pdf->GetX() - 15, $pdf->GetY() + 28, 150, 0);
	$pdf->SetFont('Arial', 'I', 8);
	$pdf->SetTextColor(0, 0, 0);
	$filename = str_replace(' ', '-', $data3->full_name) . '-Scilab-Textbook-Certificate.pdf';
	$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
	$pdf->Output($file, 'F');
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: attachment; filename=" . $filename);
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Description: File Transfer");
	header("Content-Length: " . filesize($file));
	flush();
	$fp = fopen($file, "r");
	while (!feof($fp))
	{
		echo fread($fp, 65536);
		flush();
	} //!feof($fp)
	fclose($fp);
	unlink($file);
	drupal_goto('certificate');
	return;
}
function generateRandomString($length = 5)
{
	$characters       = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
	$charactersLength = strlen($characters);
	$randomString     = '';
	for ($i = 0; $i < $length; $i++)
	{
		$randomString .= $characters[rand(0, $charactersLength - 1)];
	} //$i = 0; $i < $length; $i++
	return $randomString;
}
function generate_copyright_form_pdf()
{
	$mpath = drupal_get_path('module', 'textbook_companion');
	require($mpath . '/pdf/fpdf/fpdf.php');
	global $user;
	$x           = $user->uid;
	$proposal_id = arg(3);
	$query2      = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status=1 AND  proposal_id= :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data2       = $query2->fetchObject();
	$query3      = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id= :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data3       = $query3->fetchObject();
	$gender      = array(
		'salutation' => 'Mr. /Ms.',
		'gender' => 'He/She'
	);
	if ($data3->gender)
	{
		if ($data3->gender == 'M')
		{
			$gender = array(
				'salutation' => 'Mr.',
				'gender' => 'He'
			);
		} //$data3->gender == 'M'
		else
		{
			$gender = array(
				'salutation' => 'Ms.',
				'gender' => 'She'
			);
		}
	} //$data3->gender
	$pdf = new FPDF('P', 'mm', 'Letter');
	if (!$pdf)
	{
		echo "Error!";
	} //!$pdf
	$pdf->AddPage();
	$path = drupal_get_path('module', 'textbook_companion');
	$pdf->SetFont('Arial', 'B', 25);
	$pdf->Ln(30);
	$pdf->Cell(200, 8, 'Copyright Transfer Form', 0, 1, 'C');
	$pdf->Ln(20);
	$pdf->SetFont('Arial', '', 12);
	$pdf->MultiCell(200, 8, 'I hereby transfer the copyrights of the Scilab Textbook Companion for ' . $data2->book . ' ( Author: ' . $data2->author . ', Edition: ' . $data2->edition . ', Publisher: ' . $data2->publisher . ', Year: ' . $data2->year . ') to FOSSEE Project, IIT Bombay.', 0);
	$pdf->Ln(10);
	$pdf->MultiCell(200, 8, 'I  understand  that  the  FOSSEE  project  will  release  the  Textbook  Companion  under  the Creative Commons (CC) license.');
	$pdf->SetX(75);
	$pdf->SetFont('', 'U');
	$pdf->SetTextColor(0, 0, 255);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetY(-10);
	$pdf->SetX(209);
	$cur_date = date('jS F, Y');
	$pdf->SetY(140);
	$pdf->SetFont('', '');
	$pdf->Ln(0);
	$pdf->Cell(200, 0, ' Date: ' . $cur_date . '', 0, 1, 'L');
	$pdf->Cell(200, 20, ' Place: _________________', 0, 1, 'L');
	$pdf->SetY(140);
	$pdf->SetX(120);
	$pdf->Cell(180, 0, 'Signature: _________________', 0, 1, 'L');
	$pdf->SetY(144);
	$pdf->SetX(120);
	$pdf->multicell(140, 14, 'Name: ' . $gender['salutation'] . ' ' . $data3->full_name . '', 0, '');
	$filename = str_replace(' ', '-', $data3->full_name) . '-Scilab-Textbook-Companion-copyright-form.pdf';
	$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
	$pdf->Output($file, 'F');
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: attachment; filename=" . $filename);
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Description: File Transfer");
	header("Content-Length: " . filesize($file));
	flush();
	$fp = fopen($file, "r");
	while (!feof($fp))
	{
		echo fread($fp, 65536);
		flush();
	} //!feof($fp)
	fclose($fp);
	unlink($file);
	drupal_goto('Summer_Internship_Forms/forms');
	return;
}
function generate_undertaking_form_pdf()
{
	$mpath = drupal_get_path('module', 'textbook_companion');
	require($mpath . '/pdf/fpdf/fpdf.php');
	global $user;
	$x           = $user->uid;
	$proposal_id = arg(3);
	$query2      = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status=1 AND  proposal_id= :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data2       = $query2->fetchObject();
	$query3      = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id= :prop_id", array(
		':prop_id' => $proposal_id
	));
	$data3       = $query3->fetchObject();
	$gender      = array(
		'salutation' => 'Mr. /Ms.',
		'gender' => 'He/She'
	);
	if ($data3->gender)
	{
		if ($data3->gender == 'M')
		{
			$gender = array(
				'salutation' => 'Mr.',
				'gender' => 'He'
			);
		} //$data3->gender == 'M'
		else
		{
			$gender = array(
				'salutation' => 'Ms.',
				'gender' => 'She'
			);
		}
	} //$data3->gender
	$pdf = new FPDF('P', 'mm', 'Letter');
	if (!$pdf)
	{
		echo "Error!";
	} //!$pdf
	$pdf->AddPage();
	$path = drupal_get_path('module', 'textbook_companion');
	$pdf->SetFont('Arial', 'B', 25);
	$pdf->Ln(30);
	$pdf->Cell(200, 8, 'Undertaking Form', 0, 1, 'C');
	$pdf->Ln(0);
	$pdf->SetFont('Arial', 'B', 10);
	$pdf->Cell(200, 8, '(To be signed by college teacher)', 0, 1, 'C');
	$pdf->Ln(20);
	$pdf->SetFont('Arial', '', 12);
	$pdf->MultiCell(200, 8, 'I hereby certify that all the codes written by ' . $gender['salutation'] . ' ' . $data3->full_name . ' under the Scilab Textbook Companion Project for the book ' . $data2->book . ' ( Author: ' . $data2->author . ', Edition: ' . $data2->edition . ', Publisher: ' . $data2->publisher . ', Year: ' . $data2->year . ') are correctly reproducing the results given in the aforementioned book.', 0);
	$pdf->Ln(10);
	$pdf->MultiCell(200, 8, 'I understand that the Scilab Textbook Companion created is a part of FOSSEE project, IIT Bombay, and is sponsored by the National Mission on Education through Information and Communication Technology (NMEICT), under MHRD, Govt. of India. The project requires that the textbook companion is made available for public access as an open source document. Hence I undertake that this Scilab Textbook Companion can be made public along with the information that I have certified all the codes as giving the correct answer.');
	$pdf->SetX(75);
	$pdf->SetFont('', 'U');
	$pdf->SetTextColor(0, 0, 255);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetY(-10);
	$pdf->SetX(209);
	$cur_date = date('jS F, Y');
	$pdf->SetY(180);
	$pdf->SetFont('', '');
	$pdf->Ln(0);
	$pdf->Cell(200, 0, ' Date: ' . $cur_date . '', 0, 1, 'L');
	$pdf->Cell(200, 20, ' Place: _________________', 0, 1, 'L');
	$pdf->SetY(180);
	$pdf->SetX(120);
	$pdf->Cell(140, 0, 'Signature: _________________', 0, 1, 'L');
	$pdf->SetX(120);
	$pdf->multicell(140, 14, 'Name: ' . '____________________', 0, '');
	$pdf->SetX(120);
	$pdf->multicell(140, 14, 'Designation: ' . '______________', 0, '');
	$filename = str_replace(' ', '-', $data3->full_name) . '-Scilab-Textbook-Companion-undertaking-form.pdf';
	$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
	$pdf->Output($file, 'F');
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: attachment; filename=" . $filename);
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Description: File Transfer");
	header("Content-Length: " . filesize($file));
	flush();
	$fp = fopen($file, "r");
	while (!feof($fp))
	{
		echo fread($fp, 65536);
		flush();
	} //!feof($fp)
	fclose($fp);
	unlink($file);
	drupal_goto('Summer_Internship_Forms/forms');
	return;
}
