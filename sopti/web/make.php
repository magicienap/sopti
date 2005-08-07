<?php
session_start();
if(!isset($_GET['page'])) {
	$_SESSION = array();
}

require_once('config.php');
require_once('lib.php');

ob_start();

$user_time=microtime(TRUE);
?>

<html>

<head>
	<title>Horaires</title>
	<link rel="stylesheet" type="text/css" href="make.css">
	<link rel="stylesheet" type="text/css" href="sopti.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<?php
require_once('config.php');
?>

<center>

<img src="aep.gif">

<h1>G�n�rer des horaires</h1>

<p class="step_notcurrent">�tape 1 - Sp�cifier les cours d�sir�s
<p class="step_notcurrent">�tape 2 - Choisir les options de g�n�ration
<p class="step_current">�tape 3 - Visualiser les horaires

</center>

<p>&nbsp;

<h2>Horaires</h2>
<div class="option_block" style="width:600px;">
<p>Voici les horaires correspondant aux options s�lectionn�es. Ils sont affich�s en ordre d�croissant de pr�f�rence, selon l'objectif que vous avez choisi (ex: minimisation des trous). Le "score" le plus <b>faible</b> indique le <b>meilleur</b> horaire.

<p>Pour changer les options, utiliser le bouton Pr�c�dent de votre navigateur.

<p>Pour choisir officiellement un horaire, vous devez visiter votre dossier �tudiant. Vous trouverez un lien sur la page d'accueil du g�n�rateur d'horaires.

<p>Merci d'utiliser le g�n�rateur d'horaires de l'AEP!
</div>

<?php
	// Counter
	
	if(!is_writable($SOPTI_COUNTERFILE)) {
		mail_admin_error("Unable to open counter file ($SOPTI_COUNTERFILE)");
	}
	else {
		$counter = fopen($SOPTI_COUNTERFILE, "a");
		fwrite($counter, date("Y/m/d-H:i:s") . " " . $_SERVER['REMOTE_ADDR'] . "\n");
		fclose($counter);
	}

	if(isset($_POST['courses'])) {
	
		// Prepare explicitopen arg
		$openclose_vars_str = $_POST['openclose_vars'];
		trim($openclose_vars_str);
		$openclose_vars_str = trim($openclose_vars_str);
		$oc_vars = explode(" ", $openclose_vars_str);
	
		$explicitopen_arg="";
		foreach($oc_vars as $var) {
			if($_POST[$var] != "") {
				$explicitopen_arg .= $var . " ";
			}
		}
	
		trim($explicitopen_arg);
		//print($explicitopen_arg . "<br>\n");


		$cmd_initial=$SOPTI_EXEC . " --html make";
		$allowed_objectives = array( "minholes", "maxmorningsleep", "maxfreedays" );

		// Parse
		$courses_raw = strtoupper($_POST['courses']);
		// Replace bizarre chars with spaces
		$courses_raw = ereg_replace("[^A-Z0-9\.\-]", " ", $courses_raw);
		// Replace multiple spaces with one space
		$courses_raw = ereg_replace(" +", " ", $courses_raw);
		// Remove initial and final whitespace
		$courses_raw = trim($courses_raw);
	
		if($courses_raw != "") {
			// Split it
			$courses = explode(" ", $courses_raw);
			
		
			// Build the command string
			$cmd=$cmd_initial;
			foreach ($courses as $key => $val) {
				$cmd .= " -c ".escapeshellarg($val);
			}
		
			$result = array_search($_POST['order'], $allowed_objectives);
			if($result===FALSE) {
				error("Invalid objective");
			}
			$cmd .= " -J ${allowed_objectives[$result]}";
		
			if(strlen($explicitopen_arg)) {
				$cmd .= " -g " . escapeshellarg($explicitopen_arg) . " -G explicitopen";
			}
			else {
				$cmd .= " -g '' -G explicitopen";
			}
		
			if($_POST['maxconflicts'] != "") {
				$cmd .= " --max-conflicts ".escapeshellarg($_POST['maxconflicts']);
			}
		
			// Parse
			$week_hours = array(830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645); 
			for($i=0; $i<5; $i++) {
				for($j=0; $j<count($week_hours); $j++) {
					$period = ($i+1)*10000+$week_hours[$j];
					if($_POST["period_$period"] == "on") {
						$cmd .= " -g $period -G noperiod";
					}
				}
			
				if($_POST["noevening_$i"] == "on") {
					$cmd .= " -g \"" . ($i+1) . "1745 " . ($i+1) . "2359\" -G notbetween";
				}
			}
		
			//print $cmd;
			$handle = popen($cmd.' 2>&1', 'r');
			$xml_groups = '';
			while (!feof($handle)) {
				$xml_groups .= fread($handle, 8192);
			
			}
			pclose($handle);
			$_SESSION['xml_groups']=$xml_groups;
		}
	}
	else {
		//echo "We are in GET (first=".$_GET['first'].") (".session_name().")<br>";
	}
		$show = 10;
		$xml = simplexml_load_string($_SESSION['xml_groups']);
		echo "<h5>Engine: Version ".$xml['engine_version']." - Compute:".$xml['compute_time']." - DB Time: ".$xml['db_time']."</h5>";
		
		$schedule_array=array();
		foreach($xml->schedule as $sch) {
			array_push($schedule_array, $sch);
		}
		echo "We have ".count($schedule_array)." schedules<br>\n";
		$full_result_count = count($schedule_array);
		$current_page = $_GET['page'];
		//$first = $_GET['first'];
		//$first = $first-(($first-1)%$show);
		$first = $current_page*$show+1;
		$page_count = (int)($full_result_count/$show);
		if($full_result_count % $show) {
			$page_count++;
		}

		$schedno=0;
		for($i=$first-1; $i<$first-1+$show; $i++) {
			if($i >= $full_result_count) {
				break;
			}
			print_schedule($schedule_array[$i], $i+1);
		}

		if($page_count > 1) {
			echo "<div class=\"page_browser\">";
			if($current_page-1 >= 0) {
				echo "<a href=\"make.php?page=".($current_page-1)."\">&lt;&lt;</a> ";
			}
			for($i=0; $i < $page_count; $i++) {
				if($i == $current_page) {
					echo ($i+1)." ";
				}
				else {
					echo "<a href=\"make.php?page=$i\">".($i+1)."</a> ";
				}
			}
			if($current_page+1 < $page_count) {
				echo "<a href=\"make.php?page=".($current_page+1)."\">&gt;&gt;</a>";
			}
			echo "</div>";
		}

		$user_time=microtime(TRUE)-$user_time;
		echo "<h5 style=\"clear: left;\">Temps: ".$user_time."</h5>\n";
	//else {
		//error("Aucun cours sp�cifi�. Utiliser le bouton Pr�c�dent de votre navigateur pour changer les options.");
	//}
?>

</body>
</html>
