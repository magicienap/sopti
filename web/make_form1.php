<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>Générer des horaires - Étape 1</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<center>

<img src="aep.gif">

<h1>Générer des horaires</h1>

<p class="step_current">Étape 1 - Spécifier les cours désirés
<p class="step_notcurrent">Étape 2 - Choisir les options de génération
<p class="step_notcurrent">Étape 3 - Visualiser les horaires

</center>

<p>&nbsp;

<form method="GET" action="make_form2.php">

<?php
/*
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Dernière mise a jour du fichier de cours: ".$string_modif
*/
?>

<div class="option_block">
	<h2>Cours désirés</h2>
	<p>Écrire les sigles des cours désirés, séparés par un espace. Ex: ING1040 ING1035 ING1010<br>
	<a href="listcourses.php" target="_blank">Voir la liste des cours offerts</a>
	<p><textarea name="courses" cols="50" rows="2"></textarea>
</div>


<div class="option_block">
	<p><input type="submit" value="Continuer...">
</div>

</form>

</body>
</html>
