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

<form method="POST" action="make.php" name="form2">

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
	<p><input name="order" type="radio" value="maxfreedays"> Maximiser le nombre de jours de cong&eacute;
</div>


<h2>Contraintes particuli&egrave;res</h2>

<div class="option_block">
	<p>Pas de cours le soir - 
	<em>Cette option a &eacute;t&eacute; d&eacute;plac&eacute;e dans le tableau des p&eacute;riodes libres ci-dessous.</em>
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

// Print Evening checkboxes
print "<tr>\n<td class=\"hour\">Soir</td>\n";
for($j=0; $j<5; $j++) {
	print "<td><center><input name=\"noevening_$j\" type=\"checkbox\"></center></td>\n";
}
print "</tr>\n";

?>
	</table>
	
	<script type="text/javascript">
		function no_evening()
		{
			document.form2.noevening_0.checked=true;
			document.form2.noevening_1.checked=true;
			document.form2.noevening_2.checked=true;
			document.form2.noevening_3.checked=true;
			document.form2.noevening_4.checked=true;
		}
	</script>
	<br>
	<input value="Pas de cours du soir" type="button" onClick="no_evening()">

</div>

<h2>Ouvrir / fermer des sections</h2>
<div class="option_block">
	<p>Cocher les sections qui doivent &ecirc;tre consid&eacute;r&eacute;es comme ouvertes. Par d&eacute;faut, seules les sections o&ugrave; il reste de la place sont coch&eacute;es.
	<p><b>Case coch&eacute;e: </b> Section ouverte<br>
	   <b>Case non coch&eacute;e: </b> Section ferm&eacute;e<br>
	   <b style="background-color:#ffaaaa;">Case rouge:</b> Section pleine
	   

<?php
require_once('config.php');

$cmd=$SOPTI_EXEC_DATA . " get_open_close_form";

if(strlen($courses_raw) != 0) {
	$courses = explode(" ", $courses_raw);
	foreach ($courses as $key => $val) {
		$cmd .= " -c " . escapeshellarg($val);
	}

	passthru($cmd." 2>&1");
}
else {
	print("<p>Aucun cours!");
}
?>

</div>

<h2>Terminer</h2>
<div class="option_block">
	<p style="width:400px;"><b>Avertissement</b><br>Nous nous sommes efforc&eacute;s de faire en sorte que ce programme fonctionne correctement. Cependant, il est en cours de d&eacute;veloppement, et des probl&egrave;mes pourraient survenir. Il est donc fourni sans aucune garantie.
	<p><input type="submit" value="Afficher les horaires">
</div>

</form>

</body>
</html>
