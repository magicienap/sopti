<html>

<head>
	<title>Cours offerts</title>
	<link rel="stylesheet" type="text/css" href="listcourses.css">
</head>

<body>

<?php
require_once('config.php');

$cmd_initial=$SOPTI_EXEC_DATA . " listcourses";

function error($msg)
{
	print $msg;
	exit(1);
}

$cmd = $cmd_initial;

?>

<h1>Liste des cours offerts</h1>

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
