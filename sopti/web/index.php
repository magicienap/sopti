<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title></title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
</head>
<body>

<form method="POST" action="make.php">

<h1>Optimiseur d'horaire</h1>

<p class="news">
<?php
print file_get_contents("NEWS");
?>
<br>
<br>
<?php
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Derni&egrave;re mise a jour du fichier de cours: ".$string_modif
?>
</p>

<h2>1. S&eacute;lectionner les cours</h2>
<p>&Eacute;crire les sigles des cours d&eacute;sir&eacute;s, s&eacute;par&eacute;s par un espace.
<p><textarea name="courses" cols="50" rows="2"></textarea>

<br>

<h2>2. Type d'optimisation</h2>
<form>
<p><input name="order" type="radio" value="minholes" checked> Minimiser les trous
<p><input name="order" type="radio" value="maxmorning"> Concentrer le matin
<p><input name="order" type="radio" value="maxevening"> Concentrer le soir

<br>

<h2>3. Contraintes particulieres</h2>
<p><input name="noevening" type="checkbox"> Pas de cours le soir

<h2>4. Terminer</h2>

<p><input type="submit">

</form>

</body>
</html>
