<html>

<head>
	<title>Horaires</title>
	<link rel="stylesheet" type="text/css" href="make.css">
	<link rel="stylesheet" type="text/css" href="sopti.css">
</head>

<body>

<?php
require_once('config.php');

$cmd_initial=$SOPTI_EXEC_DATA . " --html make";
$allowed_objectives = array( "minholes", "maxmorningsleep" );

function error($msg)
{
	print $msg;
	exit(1);
}

// Parse
$courses_raw = strtoupper($_POST['courses']);
// Replace bizarre chars with spaces
$courses_raw = ereg_replace("[\t\n\v\r,;]", " ", $courses_raw);
// Replace multiple spaces with one space
$courses_raw = ereg_replace(" +", " ", $courses_raw);
// Remove initial and final whitespace
$courses_raw = trim($courses_raw);
// Split it
$courses = explode(" ", $courses_raw);
/*
foreach ($courses as $key => $val) {
	print "Course $key is " . htmlspecialchars($val) . "<br>";
}
*/


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

if($_POST['noevening'] == "on") {
	$cmd .= " -T noevening";
}

if($_POST['noclosed'] == "on") {
        $cmd .= " -T noclosed";
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
			$cmd .= " -t $period -T noperiod";
		}
	}
}

// Counter
$counter = fopen($SOPTI_COUNTERFILE, "a");
fwrite($counter, date("Y/m/d-H:i:s") . " " . $_SERVER['REMOTE_ADDR'] . "\n");
fclose($counter);

?>

<center>

<img src="aep.gif">

<h1>G&eacute;n&eacute;rer des horaires</h1>

<p class="step_notcurrent">&Eacute;tape 1 - Sp&eacute;cifier les cours d&eacute;sir&eacute;s
<p class="step_notcurrent">&Eacute;tape 2 - Choisir les options de g&eacute;n&eacute;ration
<p class="step_current">&Eacute;tape 3 - Visualiser les horaires

</center>

<p>&nbsp;

<h2>Horaires</h2>
<div class="option_block" style="width:600px;">
<p>Voici les horaires correspondant &agrave; aux options s&eacute;lectionn&eacute;es. Pour changer ces options, utiliser le bouton Pr&eacute;c&eacute;dent de votre navigateur.

<p>Pour choisir officiellement un horaire, vous devez visiter votre dossier &eacute;tudiant. Vous trouverez un lien sur la page d'accueil du g&eacute;n&eacute;rateur d'horaires.

<p>Sur chaque horaire, vous verrez appara&icirc;tre un "score"; celui-ci indique &agrave; quel point l'horaire en question correspond &agrave; l'objectif que vous avez choisi (ex: minimisation des trous). Le score le plus <b>faible</b> indique le <b>meilleur</b> horaire.

<p>Merci d'utiliser le g&eacute;n&eacute;rateur d'horaires de l'AEP!
</div>

<?php
//print "Command: $cmd";
//print "<pre>\n";
passthru($cmd." 2>&1");
//print "</pre>\n";
?>
</body>
</html>
