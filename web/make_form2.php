<?php
require_once('config.php');
require_once('lib.php');

ob_start();

function print_open_close_form($courses) {
	global $CONFIG_VARS;

	$dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
		or admin_error('Could not connect to SQL: ' . mysql_error());
	mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select database');
	
	// Make the query
	$query_begin = "SELECT courses.symbol AS sym,courses.title AS title,courses_semester.course_type AS coursetype,groups.name AS groupname,groups.theory_or_lab AS grouptl,groups.teacher AS teacher,groups.closed AS closed, groups.unique AS grpunique FROM courses INNER JOIN courses_semester ON courses_semester.course=courses.unique INNER JOIN groups ON groups.course_semester=courses_semester.unique INNER JOIN semesters ON courses_semester.semester=semesters.unique WHERE semesters.code='".$CONFIG_VARS["default_semester"]."' AND (";
	$query_end   = ") ORDER BY courses.symbol ASC,groups.name ASC,groups.theory_or_lab ASC";
	$query = $query_begin;
	$first = true;
	foreach($courses as $course) {
		if($first == true) {
			$query = $query . " courses.symbol='" . mysql_escape_string($course) . "'";
			$first = false;
		}
		else {
			$query = $query . " OR courses.symbol='" . mysql_escape_string($course) . "'";
		}
	}
	$query = $query . $query_end;
	
	//echo $query;
	
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());
	mysql_close($dblink);
	
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
		$group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["teacher"] = $row['teacher'];
		$group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["closed"] = $row['closed'];
		$group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["unique"] = $row['grpunique'];
		$course_info[$row["sym"]]["type"] = $row["coursetype"];
		$course_info[$row["sym"]]["title"] = $row["title"];
	}
	
	// check that all courses exist
	foreach($courses as $course) {
		if(!array_key_exists($course, $course_info)) {
			error("Ce cours n'existe pas: " . $course);
		}
	}

	$offsetOCChecks= 0;
	foreach($group_info as $sym => $entry1)
	{
		
		// Determine which types will form one block in the course form
		if($course_info[$sym]["type"] == 'T') {
			$types_to_consider = array("C" => 1);
		}
		elseif($course_info[$sym]["type"] == 'L') {
			$types_to_consider = array("L" => 1);
		}
		elseif($course_info[$sym]["type"] == 'TL') {
			$types_to_consider = array("C" => 1);
		}
		elseif($course_info[$sym]["type"] == 'TLS') {
			$types_to_consider = array("C" => 1, "L" => 1);
		}
		else {
			die("invalid course type");
		}
		
		echo '<table class="openclose">';
		echo '<tr><td class="title"><div class="title">' . $sym . "<br>" . $course_info[$sym]["title"] . '<br><a href="http://www.polymtl.ca/etudes/cours/details.php?sigle=' . $sym . '" target="_blank">Détails et horaire du cours</a></div></td></tr>';
		
		foreach(array_intersect_key2($entry1, $types_to_consider) as $type => $entry2)
		{
			// compute the contents of $group_type_string
			if($course_info[$sym]["type"] == 'T' && $type == 'C') {
				$group_type_string = "Théorique";
				$cb_name_prefix = 't';
			}
			elseif($course_info[$sym]["type"] == 'L' && $type == 'L') {
				$group_type_string = "Laboratoire";
				$cb_name_prefix = 'l';
			}
			elseif($course_info[$sym]["type"] == 'TL' && $type == 'C') {
				$group_type_string = "Théorique et laboratoire";
				$cb_name_prefix = 't';
			}
			elseif($course_info[$sym]["type"] == 'TLS' && $type == 'C') {
				$group_type_string = "Théorique";
				$cb_name_prefix = 't';
			}
			elseif($course_info[$sym]["type"] == 'TLS' && $type == 'L') {
				$group_type_string = "Laboratoire";
				$cb_name_prefix = 'l';
			}
			else {
				die("internal error");
			}
			
			$nbOCChecks= count($entry2);
			
			echo '<tr><td class="type">' . $group_type_string . 
			'<br><a href="javascript:changeOCChecks(' . $offsetOCChecks . ',' . $nbOCChecks . ')" class="oclink">(Ouvrir toutes les sections)</a>' . 
			'<br><a href="javascript:changeOCChecks(' . $offsetOCChecks . ',' . $nbOCChecks . ',0)" class="oclink">(Fermer toutes les sections)</a>' . 
			'</td></tr>';
			$offsetOCChecks+= $nbOCChecks;
			
			foreach($entry2 as $groupname => $entry3) {
				$cb_name = $cb_name_prefix . "_" . $sym . "_" . $groupname;
				$cb_name = ereg_replace("[^A-Za-z0-9]", "_", $cb_name);
				$openclosevars = $openclosevars . $cb_name . " ";
				echo '<!-- nested tables are used to avoir a bug in IE 4-6 concerning col widths and colspans -->';
				echo '<tr><td class="nested"><table class="nested"><tr>';
				if($entry3["closed"] == 1) {
					echo '<td class="closed">';
				}
				else {
					echo '<td class="openned">';
				}
				
				// Print group info
				echo $groupname . '<input name="' . $cb_name . '" type="checkbox"' . ($entry3["closed"] ? '' : ' checked') . '></td>';
				echo '<td class="teacher">'; // Begin teacher td
				
				if($course_info[$sym]["type"] == 'TL') {
					// If theory and lab same, we must specify 2 teachers,
					// therefore we indicate which one is 'theory' and 'lab'
					echo "<b>Théorie:</b> ";
				}
				
				echo $entry3["teacher"];
				
				if($course_info[$sym]["type"] == 'TL') {
					echo "<br><b>Lab:</b> " . $entry1["L"][$groupname]["teacher"] . "";
				}
				
				if ($entry3["closed"]) {
					echo '</td><td class="email"><a class="email" target="_blank" href="email_form.php?unique=' . $entry3["unique"] . '" onclick="this.href=\'javascript:openNotification(' . $entry3["unique"] . ')\'; this.target=\'\'"><img src="mail.jpg" alt="Notification par email"></a>';
				}
				
				echo "</td></tr></table></tr>\n";
			}
		}
		echo "</table>";
	}
	
	echo "<input type=\"hidden\" name=\"openclose_vars\" value=\"" . $openclosevars . "\">";
	
	// here's the count for the ocCheckboxOffset: 2 * courses + 1 hidden + 5 optimizations + 1 conflict + 1 minimum courses + 5 * 10 schedule blocks
	echo '
		<script type="text/javascript">
		
		function changeOCChecks(start, num, checked)
		{
			if (arguments.length == 2)
			{
				checked= 1;
			}
			
			// this is the index of the first checkbox for the open close form, the first form element has index 0
			ocCheckboxOffset= ' . (2 * count($courses) + 58) . ';
			
			for (i= 0; i < num; i++)
			{
				document.form2[ocCheckboxOffset + start + i].checked= checked;
			}
		}
		
		function openNotification(group)
		{
			window.open("email_form.php?unique=" + group ,"notification","width=500,height=460,scrollbars=yes,resizable=yes,toolbar=yes,directories=no,status=yes,menubar=no,copyhistory=no,top=20,screenY=20");
		}
	</script>
	';

}

// Sanitize the courses
$courses_raw = strtoupper($_GET['courses']);
// Replace bizarre chars with spaces
$courses_raw = ereg_replace("[^A-Z0-9\.\-]", " ", $courses_raw);
// Replace multiple spaces with one space
$courses_raw = ereg_replace(" +", " ", $courses_raw);
// Remove initial and final whitespace
$courses_raw = trim($courses_raw);

// Put the courses in an array
$courses = explode(" ", $courses_raw);
if(strlen($courses_raw) == 0) {
	error("Aucun cours spécifié");
}

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
	<title>Générer des horaires - Étape 2</title>
	<link rel="stylesheet" type="text/css" href="sopti.css">
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<script type="text/javascript" src="overlib_mini.js"><!-- overLIB (c) Erik Bosrup --></script>
	<script TYPE="text/javascript">
	<!--
	  overlib_pagedefaults(VAUTO);
	//-->
	</script>
</head>

<body>

<!-- For overLib -->
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>

<div id="header">

<img src="genhor_sm.png" alt="Générateur d'horaires">

<h1>Générer des horaires</h1>

<p class="step_notcurrent">Étape 1 - Spécifier les cours désirés
<p class="step_current">Étape 2 - Choisir les options de génération
<p class="step_notcurrent">Étape 3 - Visualiser les horaires

</div>

<form method="POST" action="make.php" name="form2">

<h2>Cours demandés</h2>
<div class="option_block">
	<p><em>Indiquer quels cours le Générateur peut choisir de ne pas mettre dans certains horaires. Cela peut aider à trouver une combinaison de cours qui donne un horaire sans conflits. Si vous voulez simplement un horaire qui comprend tous les cours ci-dessous, laissez-les marqués comme obligatoires.</em></p>
	<p><em>À quoi servent les cours facultatifs? <a href="javascript:void(0);" onmouseover="return overlib('Si vous avez le choix entre plusieurs cours dans votre cheminement et que vous voulez trouver un horaire optimal qui contient un sous-ensemble de ces cours, inscrivez-les tous dans le formulaire de la page précédente, et marquez-les facultatifs ici. Marquez comme obligatoires les cours que vous voulez absolument. Toutes les possibilités d\'horaires avec ou sans les cours marqués facultatifs seront considérées. Ils seront ensuite triés selon l\'optimisation choisie ci-dessous (ex: minimiser les trous).&lt;br/>&lt;br/>Cette option est pratique également lorsque vous êtes incapable de trouver une combinaison de cours qui n\'entrent pas en conflit. Laissez le générateur choisir parmi la liste de tous les cours que vous avez la possibilité de prendre en les marquant comme optionnels.', WIDTH, 400);" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></em></p>

<?php
	echo "<table class=\"coursetab\">\n";
	echo "<tr><th class=\"title\">Cours</th><th class=\"title\">Obligatoire</th><th class=\"title\">Facultatif</th></tr>\n";
	foreach($courses as $course) {
		echo "<tr><td>$course</td><td><input name=\"obl_".string2varname($course)."\" type=\"radio\" value=\"1\" checked></td><td><input name=\"obl_".string2varname($course)."\" type=\"radio\" value=\"0\"></td></tr>\n";
	}
	echo "</table>\n";
	print("<input type=\"hidden\" name=\"courses\" value=\"" . $courses_raw . "\">");
?>
	<p>Nombre minimum de cours dans l'horaire: <input type="text" name="mincoursecount" value="1" size="2"> <a href="javascript:void(0);" onmouseover="return overlib('Indiquer ici le nombre minimum de cours que l\'horaire doit comporter. Cette option est nécessaire seulement lorsque vous avez beaucoup de cours marqués optionnels ci-dessus.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
	
</div>

<h2>Type d'optimisation</h2>
<div class="option_block">
	<p><em>L'une ou l'autre de ces options générera la même liste d'horaires.<br>Seul l'ordre dans lequel ils seront affichés sera affecté.</em></p>
	<p><input name="order" type="radio" value="minholes" checked> Minimiser les trous <a href="javascript:void(0);" onmouseover="return overlib('Affiche en premier les horaires qui minimisent le nombre d\'<b>heures d\'attente</b> entre les cours durant une journée.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
	<p><input name="order" type="radio" value="maxmorningsleep"> Maximiser les heures de sommeil le matin <a href="javascript:void(0);" onmouseover="return overlib('Affiche en premier les horaires qui permettent de <b>commencer le plus tard possible</b> le plus de jours possible.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
	<p><input name="order" type="radio" value="maxfreedays"> Maximiser le nombre de jours de congé <a href="javascript:void(0);" onmouseover="return overlib('Affiche en premier les horaires qui offrent <b>le plus de jours sans cours</b>.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
	<p><input name="order" type="radio" value="maxcourses"> Maximiser le nombre de cours <a href="javascript:void(0);" onmouseover="return overlib('Affiche en premier les horaires qui contiennent <b>le plus de cours</b>.<br /><br />Utile seulement si au moins un cours a été marqué optionnel ci-dessus. Autrement, tous les horaires auront le même nombre de cours.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
	<p><input name="order" type="radio" value="minconflicts"> Minimiser le nombre de périodes de conflits <a href="javascript:void(0);" onmouseover="return overlib('Affiche en premier les horaires qui contiennent le moins de conflits.<br /><br />Utile seulement si le <em>nombre maximum de périodes avec conflit</em> ci-dessous est supérieur à zéro. Autrement, tous les horaires générés sont libres de conflits.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
</div>


<h2>Conflits</h2>

<div class="option_block">
	<p>Nombre maximum de périodes avec conflit: <input type="text" name="maxconflicts" value="0" size="2"> <a href="javascript:void(0);" onmouseover="return overlib('Lorsque ce champ vaut 0, seuls des horaires sans conflits sont générés.<br /><br />Lorsque ce champ est supérieur à zéro, les horaires générés peuvent comporter des conflits. Seuls les horaires dont le nombre de périodes comportant des conflits ne dépasse pas la valeur entrée dans ce champ seront affichés.<br /><br />Utilisez ce champ lorsque vous devez absolument prendre certains cours, mais que le générateur ne trouve pas d\'horaire sans conflits. Combinez-le avec <em>Minimiser le nombre de périodes de conflits</em> ci-dessus afin d\'afficher les horaires comportant le moins de conflits d\'abord.');" onmouseout="return nd();"><img style="border: none;" src="qmark.png" alt="Point d'interrogation pour aide contextuelle"></a></p>
</div>


<h2>Périodes libres</h2>
<div class="option_block">
	<p>
	Cocher les périodes qui doivent absolument être libres.<br>
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
		print '<td><input name="period_' . $period_number . '" type="checkbox"></td>' . "\n";
	}
	print "</tr>\n";
}

// Print Evening checkboxes
print "<tr class=\"" . ((count($week_hours) + 1) % 2?"odd":"even") . "\">\n<th class=\"hour\" onClick=\"switchRow(" . count($week_hours) . ")\">Soir</th>\n";
for($j=0; $j<5; $j++) {
	print "<td><input name=\"noevening_$j\" type=\"checkbox\"></td>\n";
}
print "</tr>\n";

?>
	</table>
	
	<script type="text/javascript">
		// this is the index of the first checkbox for the schedule, the first form element has index 0
		// here's the count: 2 * courses + 1 hidden + 5 optimizations + 1 conflict + 1 minimum courses
		scheduleCheckboxOffset= <?php echo 2 * count($courses) + 8; ?>;
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
	<p>Cocher les sections qui doivent être considérées comme ouvertes. Par défaut, seules les sections où il reste de la place sont cochées.</p>
	<b>Case cochée: </b> Section ouverte<br>
	<b style="background-color:#ffaaaa;">Case rouge:</b> Section pleine
	
	<p><img src="mail-white.jpg" alt="Notification par email" style="float:left; margin-right: 1ex;">
	Le système peut vous avertir par email si une section qui est fermée devient ouverte. 
	Cliquez sur l'icône pour vous inscrire.</p>

<?php
print_open_close_form($courses);
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

<?php
# $html = ob_get_clean();

# // Specify configuration
# $config = array(
	# 'indent' => true, 
	# 'output-xhtml' => true, 
	# 'wrap' => 75, 
	# 'vertical-space' => true);

# // Tidy
# $tidy = new tidy;
# $tidy->parseString($html, $config, 'latin1');
# $tidy->cleanRepair();

# // Output
# echo $tidy;
?>
