<html>

<head>
	<title>Horaires</title>
	<link rel="stylesheet" type="text/css" href="make.css">
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
$courses_raw = $_POST['courses'];
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

//print "Command: $cmd";
//print "<pre>\n";
passthru($cmd." 2>&1");
//print "</pre>\n";
?>
</body>
</html>
