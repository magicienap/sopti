<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>G&eacute;n&eacute;rer des horaires - &Eacute;tape 2</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
</head>
<body>

<center>

<img src="aep.gif">

<h1>G&eacute;n&eacute;rer des horaires</h1>

<p class="step_notcurrent">&Eacute;tape 1 - Sp&eacute;cifier les cours d&eacute;sir&eacute;s
<p class="step_current">&Eacute;tape 2 - Choisir les options de g&eacute;n&eacute;ration
<p class="step_notcurrent">&Eacute;tape 3 - Visualiser les horaires

</center>

<form method="POST" action="make.php">

<?php
/*
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Derni&egrave;re mise a jour du fichier de cours: ".$string_modif
*/
?>

<?php
	// Sanitize the courses
	$courses_raw = strtoupper($_GET['courses']);
	// Replace bizarre chars with spaces
	$courses_raw = ereg_replace("[\t\n\v\r,;]", " ", $courses_raw);
	// Replace multiple spaces with one space
	$courses_raw = ereg_replace(" +", " ", $courses_raw);
	// Remove initial and final whitespace
	$courses_raw = trim($courses_raw);
?>

<p>&nbsp;

<form>

<h2>Cours demand&eacute;s</h2>
<div class="option_block">
	<p>

<?php
	print(htmlentities($courses_raw));
	print("<input type=\"hidden\" name=\"courses\" value=\"" . $courses_raw . "\">");
?>
	
</div>

<h2>Type d'optimisation</h2>
<div class="option_block">
	<p><em>L'une ou l'autre de ces options g&eacute;n&eacute;rera la m&ecirc;me liste d'horaires.<br>Seul l'ordre dans lequel ils seront affich&eacute;s sera affect&eacute;.</em>
	<p><input name="order" type="radio" value="minholes" checked> Minimiser les trous
	<p><input name="order" type="radio" value="maxmorningsleep"> Maximiser les heures de sommeil le matin
</div>


<h2>Contraintes particuli&egrave;res</h2>

<div class="option_block">
	<p><input name="noevening" type="checkbox"> Pas de cours le soir
	<p><input name="noclosed" type="checkbox" checked> Groupes avec places disponibles seulement
	<p>Nombre maximum de p&eacute;riodes avec conflit: <input type="text" name="maxconflicts" value="0" size="2">
</div>


<h2>P&eacute;riodes libres</h2>
<div class="option_block">
	<p>Cochez les p&eacute;riodes qui doivent absolument &ecirc;tre libres.
	<table class="free_period_selector">
<?php

$week_hours = array(830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645);
$weekdays = array("Lun", "Mar", "Mer", "Jeu", "Ven");

print "<tr>\n";
// Leave one column empty for the hours
print "<td class=\"empty\"></td>\n";
for($i=0; $i<5; $i++) {
	print "<td class=\"weekday\">${weekdays[$i]}</td>\n";
}
print "</tr>\n";

for($i=0; $i<count($week_hours); $i++) {
	print "<tr>\n";
	$hour_string = sprintf("%d:%.2d", $week_hours[$i]/100, $week_hours[$i]%100);
	print "<td class=\"hour\">${hour_string}</td>";
	
	for($j=0; $j<5; $j++) {
		//print "<td>".($i+1)*10000+$week_hours[$i]."</td>\n";
		$period_number = ($j+1)*10000+$week_hours[$i];
		print "<td><center><input name=\"period_${period_number}\" type=\"checkbox\"></center></td>\n";
	}
	print "</tr>\n";
}

?>
	</table>
</div>

<h2>Ouvrir / fermer des sections</h2>
<div class="option_block">
	<p>Option en d&eacute;veloppement; disponible bient&ocirc;t!
</div>

<h2>Terminer</h2>
<div class="option_block">
	<p style="width:400px;"><b>Avertissement</b><br>Nous nous sommes efforc&eacute;s de faire en sorte que ce programme fonctionne correctement. Cependant, il est en cours de d&eacute;veloppement, et des probl&egrave;mes pourraient survenir. Il est donc fourni sans aucune garantie.
	<p><input type="submit" value="Afficher les horaires">
</div>

</form>

</body>
</html>
