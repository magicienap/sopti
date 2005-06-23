<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>Générer des horaires - Étape 2</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<center>

<img src="aep.gif">

<h1>Générer des horaires</h1>

<p class="step_notcurrent">Étape 1 - Spécifier les cours désirés
<p class="step_current">Étape 2 - Choisir les options de génération
<p class="step_notcurrent">Étape 3 - Visualiser les horaires

</center>

<form method="POST" action="make.php" name="form2">

<?php
	// Sanitize the courses
	$courses_raw = strtoupper($_GET['courses']);
	// Replace bizarre chars with spaces
	$courses_raw = ereg_replace("[^A-Z0-9\.\-]", " ", $courses_raw);
	// Replace multiple spaces with one space
	$courses_raw = ereg_replace(" +", " ", $courses_raw);
	// Remove initial and final whitespace
	$courses_raw = trim($courses_raw);
?>

<p>&nbsp;

<h2>Cours demandés</h2>
<div class="option_block">
	<p>

<?php
	print(htmlentities($courses_raw));
	print("<input type=\"hidden\" name=\"courses\" value=\"" . $courses_raw . "\">");
?>
	
</div>

<h2>Type d'optimisation</h2>
<div class="option_block">
	<p><em>L'une ou l'autre de ces options générera la même liste d'horaires.<br>Seul l'ordre dans lequel ils seront affichés sera affecté.</em>
	<p><input name="order" type="radio" value="minholes" checked> Minimiser les trous
	<p><input name="order" type="radio" value="maxmorningsleep"> Maximiser les heures de sommeil le matin
	<p><input name="order" type="radio" value="maxfreedays"> Maximiser le nombre de jours de congé
</div>


<h2>Contraintes particulières</h2>

<div class="option_block">
	<p>Pas de cours le soir - 
	<em>Cette option a été déplacée dans le tableau des périodes libres ci-dessous.</em>
	<p>Nombre maximum de périodes avec conflit: <input type="text" name="maxconflicts" value="0" size="2">
</div>


<h2>Périodes libres</h2>
<div class="option_block">
	<p>
	Cochez les périodes qui doivent absolument être libres.<br>
	Cliquer sur un nom de colonne ou de rangée pour activer ou désactiver tout ce groupe de cases.
	<table class="free_period_selector">
<?php

$week_hours = array(830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645);
$weekdays = array("Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi");

print "<tr>\n";

// Leave one column empty for the hours
print "<th class=\"empty\"></th>\n";

// Print the day names
for($i=0; $i < count($weekdays); $i++) {
	print "<th class=\"weekday\" onClick=\"switchCol(" . $i . ")\">${weekdays[$i]}</th>\n";
}

print "</tr>\n";

for($i=0; $i<count($week_hours); $i++) {
	print "<tr class=\"" . (($i + 1) % 2?"odd":"even") . "\">\n";
	$hour_string = sprintf("%d:%.2d", $week_hours[$i]/100, $week_hours[$i]%100);
	print "<th class=\"hour\" onClick=\"switchRow(" . $i . ")\">${hour_string}</th>";
	
	for($j=0; $j<5; $j++) {
		$period_number = ($j+1)*10000+$week_hours[$i];
		print '<td><center><input name="period_' . $period_number . '" type="checkbox"></center></td>' . "\n";
	}
	print "</tr>\n";
}

// Print Evening checkboxes
print "<tr class=\"" . ((count($week_hours) + 1) % 2?"odd":"even") . "\">\n<th class=\"hour\" onClick=\"switchRow(" . count($week_hours) . ")\">Soir</th>\n";
for($j=0; $j<5; $j++) {
	print "<td><center><input name=\"noevening_$j\" type=\"checkbox\"></center></td>\n";
}
print "</tr>\n";

?>
	</table>
	
	<script type="text/javascript">
		// this is the index of the first checkbox for the schedule, the first form element has index 0
		scheduleCheckboxOffset= 5;
		colStatus= new Array(<?php echo count($weekdays) ?>);
		rowStatus= new Array(<?php echo count($week_hours) + 1 ?>);
		
		function switchRow(numRow)
		{
			rowStatus[numRow]= !rowStatus[numRow];
			
			for (i= 0; i < <?php echo count($weekdays) ?>; i++)
			{
				document.form2[scheduleCheckboxOffset + numRow * <?php echo count($weekdays) ?> + i].checked= rowStatus[numRow];
			}
		}
		
		function switchCol(numCol)
		{
			colStatus[numCol]= !colStatus[numCol];
			
			for (j= 0; j < <?php echo count($week_hours) + 1 ?>; j++)
			{
				document.form2[scheduleCheckboxOffset + numCol + j * <?php echo count($weekdays) ?>].checked= colStatus[numCol];
			}
		}
	</script>
</div>

<h2>Ouvrir / fermer des sections</h2>
<div class="option_block">
	<p>Cocher les sections qui doivent être considérées comme ouvertes. Par défaut, seules les sections où il reste de la place sont cochées.
	<p><b>Case cochée: </b> Section ouverte<br>
	   <b>Case non cochée: </b> Section fermée<br>
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
	<p style="width:400px;"><b>Avertissement</b><br>Nous nous sommes efforcés de faire en sorte que ce programme fonctionne correctement. Cependant, il est en cours de développement, et des problèmes pourraient survenir. Il est donc fourni sans aucune garantie.
	<p><input type="submit" value="Afficher les horaires">
</div>

</form>

</body>
</html>
