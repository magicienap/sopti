<?php
require_once('config.php');
require_once('lib.php');

read_config_file($SOPTI_CONFIG_FILE);

ob_start();

/* array_intersect_key2 has the same functionality as the
   PHP5 array_intersect_key function used with 2 arguments.
   by Benjamin Poirier
*/

function array_intersect_key2($array1, $array2) 
{ 
	$output= array(); 
     
	foreach ($array1 as $key => $value) { 
		if (array_key_exists($key, $array2)) { 
			$output= array_merge($output, array($key => $value)); 
		} 
	} 
     
	return $output; 
} 

function print_open_close_form($courses) {
	global $CONFIG_VARS;

	// Query used
	// SELECT courses.symbol,courses.title,groups.name,groups.theory_or_lab FROM courses INNER JOIN courses_semester ON courses_semester.course=courses.unique INNER JOIN groups ON groups.course_semester=courses_semester.unique WHERE courses.symbol='ING1040' OR courses.symbol='ING1020' OR courses.symbol='INF2600' ORDER BY courses.symbol ASC,groups.name ASC,groups.theory_or_lab ASC
	
	#$dblink = mysql_connect('', 'poly', 'pol')
	$dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
		or die('Could not connect: ' . mysql_error());
	#mysql_select_db('poly_courses') or die('Could not select database');
	mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select database');
	
	// Make the query
	$query_begin = "SELECT courses.symbol AS sym,courses.title AS title,courses_semester.course_type AS coursetype,groups.name AS groupname,groups.theory_or_lab AS grouptl,groups.teacher AS teacher,groups.closed AS closed FROM courses INNER JOIN courses_semester ON courses_semester.course=courses.unique INNER JOIN groups ON groups.course_semester=courses_semester.unique WHERE";
	$query_end   = " ORDER BY courses.symbol ASC,groups.name ASC,groups.theory_or_lab ASC";
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
	
	$current_course = ""; // Course being read
	$current_type_label = ""; // Type being read: T or L
	$current_type_index = 0;
	$group_info = array();
	$first = true; // Are we reading the first entry?
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
		$group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["teacher"] = $row['teacher'];
		$group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["closed"] = $row['closed'];
		$course_info[$row["sym"]]["type"] = $row["coursetype"];
		$course_info[$row["sym"]]["title"] = $row["title"];
	}
	
	// check that all courses exist
	foreach($courses as $course) {
		if(!array_key_exists($course, $course_info)) {
			error("Ce cours n'existe pas: " . $course);
		}
	}

	print("<table class=\"openclose_table\">\n");
	foreach($group_info as $sym => $entry1) {
		/*
		$course_rowspan = 0;
		foreach($group_info as $tmp => $groups) {
			$course_rowspan += count($groups);
		}
		*/

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
		
		echo "<tr><td class=\"course_sym\" width=\"80%\">" . $sym . "<br>" . $course_info[$sym]["title"] . "</td><td style=\"background-color:#FFFFFF;\">&nbsp;</td></tr>\n";
		echo "<tr><td colspan=\"2\" class=\"openclose_whitespace\"></td></tr>\n";
		#echo "<tr><td colspan=2>\n";
		foreach(array_intersect_key2($entry1, $types_to_consider) as $type => $entry2) {
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
				$cb_name_prefix = 't';
			}
			else {
				die("internal error");
			}
		
			echo "<tr><td align=center colspan=2 class=\"ocform_type\">" . $group_type_string . "</td></tr>\n";
			foreach($entry2 as $groupname => $entry3) {
				$cb_name = $cb_name_prefix . "_" . $sym . "_" . $groupname;
				$cb_name = ereg_replace("[^A-Za-z0-9]", "_", $cb_name);
				$openclosevars = $openclosevars . $cb_name . " ";
				echo "<tr><td colspan=\"2\" class=\"openclose_whitespace\"></td></tr>\n";
				echo "<tr><td colspan=2>\n"; // Start the subtable td
				// Start the inner table
				echo "<table class=\"oc_sub_table\">\n<tr><td class=\"grpname\">\n";
				// Print group info
				if($entry3["closed"] == 1) {
					echo "<div class=\"full_cbox\">\n";
				}
				else {
					echo "<div class=\"notfull_cbox\">\n";
				}
				echo $groupname . "<input name=\"" . $cb_name . "\" type=\"checkbox\"";
				if($entry3["closed"] != 1) {
					echo "checked";
				}
				echo "></div></td>";
				echo "<td>"; // Begin teacher td
				
				if($course_info[$sym]["type"] == 'TL') {
					// If theory and lab same, we must specify 2 teachers,
					// therefore we indicate which one is 'theory' and 'lab'
					echo "<b>Théorie:</b> ";
				}
				
				echo $entry3["teacher"];

				if($course_info[$sym]["type"] == 'TL') {
					echo "<br><b>Lab:</b> " . $entry1["L"][$groupname]["teacher"] . "";
				}
				
				// Close the inner table
				echo "</td>";
				echo "</tr></table>\n";

				echo "</td></tr>\n";
			}
		}
		#echo "</td></tr>"; // end of group row
		
		// print whitespace
		printf("<tr><td colspan=\"2\" class=\"openclose_whitespace\">&nbsp;</td></tr>\n");
	}
	print("</table>\n\n");
	
	echo "<input type=\"hidden\" name=\"openclose_vars\" value=\"" . $openclosevars . "\">";
}
?>

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
	
	// Put the courses in an array
	$courses = explode(" ", $courses_raw);
	if(strlen($courses_raw) == 0) {
		error("Aucun cours spécifié");
	}
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


<h2>Conflits</h2>

<div class="option_block">
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
