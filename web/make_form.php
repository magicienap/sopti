<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>G&eacute;n&eacute;rer des horaires</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
</head>
<body>

<form method="POST" action="make.php">

<h1>G&eacute;n&eacute;rer des horaires</h1>

<?php
/*
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Derni&egrave;re mise a jour du fichier de cours: ".$string_modif
*/
?>

<h2>1. S&eacute;lectionner les cours</h2>
<div class="option_block">
	<p>&Eacute;crire les sigles des cours d&eacute;sir&eacute;s, s&eacute;par&eacute;s par un espace. Les lettres doivent &egrave;tre en majuscules (ex: ING1040)<br>
	(<a href="listcourses.php">Liste des cours offerts</a>)
	<p><textarea name="courses" cols="50" rows="2"></textarea>
</div>


<h2>2. Type d'optimisation</h2>
<form>
<div class="option_block">
	<p><input name="order" type="radio" value="minholes" checked> Minimiser les trous
	<p><input name="order" type="radio" value="maxmorningsleep"> Maximiser les heures de sommeil le matin
</div>


<h2>3. Contraintes particuli&egrave;res</h2>

<div class="option_block">
	<p><input name="noevening" type="checkbox"> Pas de cours le soir
	<p><input name="noclosed" type="checkbox" checked> Groupes avec places disponibles seulement
	<p>Nombre maximum de p&eacute;riodes avec conflit: <input type="text" name="maxconflicts" value="0" size="2">
</div>


<h2>4. P&eacute;riodes libres</h2>
<div class="option_block">
	<p>Cochez les p&eacute;riodes qui doivent absolument &ecirc;tre libres.
	<table class="free_period_selector">
<?php

$week_hours = array(830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645);
//$weekdays = array("Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi");
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

<h2>5. Terminer</h2>
<div class="option_block">
	<p><input type="submit" value="Afficher les horaires">
</div>

</form>

</body>
</html>
