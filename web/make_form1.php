<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>G&eacute;n&eacute;rer des horaires - &Eacute;tape 1</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
</head>

<body>

<center>

<img src="aep.gif">

<h1>G&eacute;n&eacute;rer des horaires</h1>

<p class="step_current">&Eacute;tape 1 - Sp&eacute;cifier les cours d&eacute;sir&eacute;s
<p class="step_notcurrent">&Eacute;tape 2 - Choisir les options de g&eacute;n&eacute;ration
<p class="step_notcurrent">&Eacute;tape 3 - Visualiser les horaires

</center>

<p>&nbsp;

<form method="GET" action="make_form2.php">

<?php
/*
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Derni&egrave;re mise a jour du fichier de cours: ".$string_modif
*/
?>

<div class="option_block">
	<h2>Cours d&eacute;sir&eacute;s</h2>
	<p>&Eacute;crire les sigles des cours d&eacute;sir&eacute;s, s&eacute;par&eacute;s par un espace. Ex: ING1040.<br>
	<a href="listcourses.php" target="_blank">Voir la liste des cours offerts</a>
	<p><textarea name="courses" cols="50" rows="2"></textarea>
</div>


<div class="option_block">
	<p><input type="submit" value="Continuer...">
</div>

</form>

</body>
</html>
