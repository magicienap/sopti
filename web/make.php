<html>

<head>
	<title>Horaires</title>
	<link rel="stylesheet" type="text/css" href="make.css">
</head>

<body>

<?php
$cmd_initial="../../sopti_run/sopti --html --coursefile ../../sopti_run/data/courses.csv --closedfile ../../sopti_run/data/closed.csv make";
$allowed_objectives = array( "minholes" );

function error($msg)
{
	print $msg;
	exit(1);
}

// Parse
$courses_raw = $_POST['courses'];
$courses_raw = trim($courses_raw);
// Replace bizarre chars with spaces
$courses_raw = ereg_replace("[\t\n\v\r,;]", " ", $courses_raw);
// Replace multiple spaces with one space
$courses_raw = ereg_replace(" +", " ", $courses_raw);
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

//print "Command: $cmd";
//print "<pre>\n";
passthru($cmd." 2>&1");
//print "</pre>\n";
?>
</body>
</html>
