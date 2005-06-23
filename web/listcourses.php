<html>

<head>
	<title>Cours offerts</title>
	<link rel="stylesheet" type="text/css" href="listcourses.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
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
Légende:
[SIGLE] [TITRE] [SECTIONS...]

Sections:
Numéro(Théorique ou Lab)
Un astérisque signifie que la section est pleine.

<?php
passthru($cmd." 2>&1");
?>
</pre>
</body>
</html>
