<html>

<head>
	<title>Cours offerts</title>
	<link rel="stylesheet" type="text/css" href="listcourses.css">
</head>

<body>

<?php
$cmd_initial="../../sopti_run/sopti --coursefile ../../sopti_run/data/courses.csv --closedfile ../../sopti_run/data/closed.csv listcourses";

function error($msg)
{
	print $msg;
	exit(1);
}

$cmd = $cmd_initial;

?>

<pre>
L&eacute;gende:
[SIGLE] [TITRE] [SECTIONS...]

Sections:
Num&eacute;ro(Th&eacute;orique ou Lab)
Un ast&eacute;risque signifie que la section est pleine.

<?php
passthru($cmd." 2>&1");
?>
</pre>
</body>
</html>
