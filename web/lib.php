<?php
error_reporting(0);

require_once('config.php');

$CONFIG_VARS=array();

read_config_file($SOPTI_CONFIG_FILE);
read_semester_config_file($SOPTI_SEMESTER_CONFIG_FILE);

$group_data = array();
$period_data = array();

function connect_db()
{
	global $CONFIG_VARS;
	global $dblink;

	$dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
                or admin_error('Could not connect to SQL: ' . mysql_error());
	mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select database');

	return $dblink;
}

function admin_error($msg)
{
        ob_end_clean();

?>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>Générateur d'horaires - Erreur interne</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
        <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<center>

<img src="genhor_sm.png" alt="Générateur d'horaires">
<p style="font-size: 15px;">Générateur d'horaires</p>
</center>

<div style="background-color: #ffbbbb;">
<p align="center"><b><font size="+1">ERREUR INTERNE</font></b></p>
<p align="center"><?php echo $msg; ?></p>
</div>

<p align="center">L'administrateur a été informé de cette erreur.
</body>
</html>

<?php

        exit();

}

function error($msg)
{
	ob_end_clean();
?>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
  <title>Générateur d'horaires - Erreur</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
        <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<center>

<img src="genhor_sm.png" alt="Générateur d'horaires">
<p><font size="+2">Générateur d'horaires</font>
</center>

<div style="background-color: #ffbbbb;">
<p align="center"><b><font size="+1">ERREUR</font></b></p>
<p align="center"><?php echo $msg; ?></p>
</div>

<p align="center">Si possible, utiliser le bouton précédent de votre navigateur pour revenir en arrière et corriger l'erreur.
</body>
</html>

<?php

	exit();
}

function read_semester_config_file($file)
{
	global $CONFIG_VARS;

	$semester = file_get_contents($file, "r");
	if($semester === FALSE) {
		error("Impossible de lire le fichier de configuration " . $file);
	}
}

function read_config_file($file)
{
	global $CONFIG_VARS;
	
	$handle = fopen($file, "r");
	if($handle == FALSE) {
		error("Impossible d'ouvrir le fichier de configuration " . $file);
	}
	while (!feof($handle)) {
		$line = fgets($handle, 4096);
		$origline = $line;
		
		// Remove final newline
		$line = rtrim($line, "\n");
		// Remove comments
		$line = ereg_replace("([^#]*)#.*", "\\1", $line);
		// Ignore empty lines
		if (ereg("^[ \t]*$", $line)) {
			continue;
		}
		// Try unquoted argument format
		if(ereg("^[ \t]*([^ \t]+)[ \t]+([^ \t\"]+)[ \t]*$", $line, $args)) {
			$varname = $args[1];
			$varval = $args[2];
			$CONFIG_VARS[$varname] = $varval;
			//echo "$varname $varval\n";
		}
		// Try quoted argument format
		elseif(ereg("^[ \t]*([^ \t]+)[ \t]+\"([^\"]*)\"[ \t]*$", $line, $args)) {
			$varname = $args[1];
			$varval = $args[2];
			$CONFIG_VARS[$varname] = $varval;
			//echo "$varname $varval\n";
		}
		else {
			error("syntax error in config file at line: " . $origline);
		}
	}
	fclose($handle);
}

function mail_admin_error($msg)
{
	global $CONFIG_VARS;

        mail($CONFIG_VARS['private_admin_email'], "Sopti error", "Hello,\n\nSopti at ".php_uname("n")." encountered the following error at ".date("r").":\n\n" . $msg . "\n\nThanks", "From: sopti@".php_uname("n"));
}

function print_schedule($sch, $schedno)
{
	// TODO: ensure at least one group
	// TODO: check valid combination of lab/theory groups
	
	global $CONFIG_VARS;
	global $dblink;
	
	$php_db_time=0;
	$prof_string="";
	$php_function_time=microtime(TRUE);
	
	// Parse the input
	$requested_groups=array();

	foreach($sch['course'] as $course) {
		$entry['symbol'] = (string)$course['symbol'];
		$entry['th_grp'] = (string)$course['theory_grp'];
		$entry['lab_grp'] = (string)$course['lab_group'];
		array_push($requested_groups, $entry);
	}
	
	if(!mysql_ping($dblink)) {
		$dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"], false)
			or admin_error('Could not connect to SQL: ' . mysql_error());
		mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select database');
	}

	global $group_data;
	$groups_to_add = array();
	foreach($requested_groups as $req) {
		if(!array_key_exists($req['symbol'], $group_data)) {
			array_push($groups_to_add, $req);
		}
	}
	if(count($groups_to_add) > 0) {
		// Make the groups query
		$query_begin = "SELECT courses.symbol AS symbol,courses.title AS title,courses_semester.course_type AS course_type,groups.name AS grp,groups.theory_or_lab AS tol,groups.teacher,groups.places_room,groups.places_group,groups.places_taken,groups.closed AS closed FROM groups LEFT OUTER JOIN courses_semester ON courses_semester.unique=groups.course_semester LEFT OUTER JOIN courses ON courses.unique=courses_semester.course LEFT OUTER JOIN semesters ON semesters.unique=courses_semester.semester WHERE";
		$query_end   = ")";
		$query = $query_begin;
		$query .= " semesters.code='" . $CONFIG_VARS['default_semester'] . "' AND (0";
		foreach($groups_to_add as $req) {
			$query .= " OR courses.symbol='".$req['symbol']."'";
		}
		$query .= $query_end;

		//error($query);
	
		$tmp=microtime(TRUE);
		$result = mysql_query($query, $dblink) or admin_error('Query failed: ' . mysql_errno($dblink) . " " . mysql_error($dblink));
		$prof_string .= "group sql query: ".(microtime(TRUE)-$tmp)."<br />\n";
		// Organize the results
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$group_data[$row['symbol']]['course_type'] = $row['course_type'];
			$group_data[$row['symbol']]['title'] = $row['title'];
			if($row['tol'] == 'C') {
				$group_data[$row['symbol']]['theory'][$row['grp']]['teacher'] = $row['teacher'];
			}
			else {
				$group_data[$row['symbol']]['lab'][$row['grp']]['teacher'] = $row['teacher'];
			}
		}
	}
	
	// print the group summary
	$tmp=microtime(TRUE);
?>
	<table class="group_summary" style="width: 675px;">
	<tr><td colspan="7">
		<table width="100%" border="0" style="background-color:#777777; font-size: 14px;">
			<tr>
				<td style="background-color: #777777; text-align: left; color: white;"><span style="font-size: 9px;">Horaire</span> <b><?php echo $schedno; ?></b></td><td style="text-align: right; background-color: #777777; color: white;"><span style="font-size: 9px;">Score:</span> <b><?php printf("%.3f %%", 100.0/($sch['score']+1.0)); ?></b></td>
			</tr>
		</table>
	</td></tr>
	<tr><th rowspan="2">Sigle</th><th rowspan="2">Titre</th><th colspan="2">Théorie</th><th colspan="2">Lab</th></tr>
	<tr><th class="subheader">Section</th><th class="subheader">Chargé</th><th class="subheader">Section</th><th class="subheader">Chargé</th><th class="subheader"></th></tr>
	<tr><td colspan="7" style="background-color: black; height: 1px;"></td></tr>
	
<?php
	foreach($requested_groups as $req) {
		if(count($group_data[(string)$req['symbol']]) == 0) {
			error("Cours introuvable: " . $req['symbol']);
			// abort
		}
		if($group_data[(string)$req['symbol']]['course_type'] == 'T') {
			if(!isset($group_data[$req['symbol']]['theory'][$req['th_grp']])) {
				error("Groupe introuvable: ");
			}
			echo "	<tr class=\"course_row\"><td>".$req['symbol']."</td><td>".$group_data[$req['symbol']]['title']."</td><td>".$req['th_grp']."</td><td>".$group_data[$req['symbol']]['theory'][$req['th_grp']]['teacher']."</td><td>-</td><td>-</td></tr>\n";
		}
		elseif($group_data[$req['symbol']]['course_type'] == 'L') {
			if(!isset($group_data[$req['symbol']]['lab'][$req['lab_grp']])) {
				error("Groupe introuvable: ");
			}
			echo "	<tr class=\"course_row\"><td>".$req['symbol']."</td><td>".$group_data[$req['symbol']]['title']."</td><td>-</td><td>-</td><td>".$req['lab_grp']."</td><td>".$group_data[$req['symbol']]['lab'][$req['lab_grp']]['teacher']."</td></tr>\n";
		}
		elseif($group_data[$req['symbol']]['course_type'] == 'TL') {
		        if($req['th_grp'] != $req['lab_grp']) {
				error("Horaire illégal; th_grp != lab_grp pour un cours TL");
			}

			if(!isset($group_data[$req['symbol']]['theory'][$req['th_grp']])) {
				error("Groupe théorique introuvable: " . $req['symbol'] . "/" . $req['th_grp']);
			}
			if(!isset($group_data[$req['symbol']]['lab'][$req['lab_grp']])) {
				error("Groupe lab introuvable: " . $req['symbol'] . "/" . $req['lab_grp']);
			}
			
			echo "	<tr class=\"course_row\"><td>".$req['symbol']."</td><td>".$group_data[$req['symbol']]['title']."</td><td>".$req['th_grp']."</td><td>".$group_data[$req['symbol']]['theory'][$req['th_grp']]['teacher']."</td><td>".$req['lab_grp']."</td><td>".$group_data[$req['symbol']]['lab'][$req['lab_grp']]['teacher']."</td></tr>\n";
		}
		elseif($group_data[$req['symbol']]['course_type'] == 'TLS') {
			if(!isset($group_data[$req['symbol']]['theory'][$req['th_grp']])) {
				error("Groupe théorique introuvable: " . $req['symbol'] . "/" . $req['th_grp']);
			}
			if(!isset($group_data[$req['symbol']]['lab'][$req['lab_grp']])) {
				error("Groupe lab introuvable: " . $req['symbol'] . "/" . $req['lab_grp']);
			}
			
			echo "	<tr class=\"course_row\"><td>".$req['symbol']."</td><td>".$group_data[$req['symbol']]['title']."</td><td>".$req['th_grp']."</td><td>".$group_data[$req['symbol']]['theory'][$req['th_grp']]['teacher']."</td><td>".$req['lab_grp']."</td><td>".$group_data[$req['symbol']]['lab'][$req['lab_grp']]['teacher']."</td></tr>\n";
		}
		else {
			error_admin("Invalid group type");
		}
	}

	echo '</table>';
 	$prof_string .= "print group summary: ".(microtime(TRUE)-$tmp)."<br />\n";

	global $period_data;
	$periods_to_add = array();
	foreach($requested_groups as $req) {
		if(!array_key_exists($req['symbol'], $period_data)) {
			array_push($periods_to_add, $req);
		}
	}

	if(count($periods_to_add) > 0) {
		// Make the periods query

		$query_begin = "SELECT courses.symbol AS symbol,groups.name AS grp,groups.theory_or_lab AS tol,periods.time AS time,periods.room AS room,periods.week AS week,periods.weekday AS weekday FROM periods LEFT JOIN groups ON groups.unique=periods.group LEFT JOIN courses_semester ON courses_semester.unique=groups.course_semester LEFT JOIN courses ON courses.unique=courses_semester.course LEFT JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS['default_semester']."' AND (0 ";
		$query_end = ")";
		$query = $query_begin;
		foreach($periods_to_add as $req) {
			$query .= "OR courses.symbol='".$req['symbol']."' ";
		}
		$query .= $query_end;
	
		$tmp=microtime(TRUE);
		$result = mysql_query($query) or die('Query failed: ' . mysql_error());
		$prof_string .= "period sql query: ".(microtime(TRUE)-$tmp)."<br />\n";
		//mysql_close($dblink);
		$tmp=microtime(TRUE);

		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			if(!isset($period_data[$row['symbol']][$row['tol']][$row['grp']])) {
				$period_data[$row['symbol']][$row['tol']][$row['grp']]=array();
			}
			array_push($period_data[$row['symbol']][$row['tol']][$row['grp']], $row);
		}
	}

	$schedule_periods=array(); # periods to add in schedule
        foreach($requested_groups as $req) {
		if(strlen($req['th_grp'])) {
			$schedule_periods = array_merge($schedule_periods, $period_data[$req['symbol']]['C'][$req['th_grp']]);
		}
		if(strlen($req['lab_grp'])) {
			$schedule_periods = array_merge($schedule_periods, $period_data[$req['symbol']]['L'][$req['lab_grp']]);
		}
	}

	draw_schedule($schedule_periods);
}

function print_room_schedule($room) {
	global $CONFIG_VARS;
        $dblink = connect_db();
	
	// Make the query

	$query = "SELECT courses.symbol AS symbol,groups.name AS grp,groups.theory_or_lab AS tol, periods.room as room, periods.time AS time, periods.week AS week,periods.weekday AS weekday FROM periods INNER JOIN groups ON groups.unique=periods.group INNER JOIN courses_semester ON courses_semester.unique=groups.course_semester INNER JOIN courses ON courses.unique=courses_semester.course INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS['default_semester']."' AND periods.room='".mysql_real_escape_string($room, $dblink)."'";
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());

	$schedule_periods = array();
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
		array_push($schedule_periods, $row);
	}

	mysql_close($dblink);

	draw_schedule($schedule_periods);
}

function print_teacher_schedule($teacher) {
	global $CONFIG_VARS;
        $dblink = connect_db();
	
	// Make the query

	$query = "SELECT courses.symbol AS symbol,groups.name AS grp,groups.theory_or_lab AS tol, periods.room as room, periods.time AS time, periods.week AS week,periods.weekday AS weekday FROM periods INNER JOIN groups ON groups.unique=periods.group INNER JOIN courses_semester ON courses_semester.unique=groups.course_semester INNER JOIN courses ON courses.unique=courses_semester.course INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS['default_semester']."' AND groups.teacher='".mysql_real_escape_string($teacher, $dblink)."'";
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());

	$schedule_periods = array();
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
		array_push($schedule_periods, $row);
	}

	mysql_close($dblink);

	draw_schedule($schedule_periods);
}

function draw_schedule($schedule_periods) {
	// Define the constants
	$week_hour_codes = array('0830', '0930', '1030', '1130', '1245', '1345', '1445', '1545', '1645');
	$week_hour_labels = array('8:30', '9:30', '10:30', '11:30', '12:45', '13:45', '14:45', '15:45', '16:45');
	$week_nighthours_codes = array('1800', '1900', '2000', '2100');
	$week_nighthours_labels = array('18:00', '19:00', '20:00', '21:00');
	$week_day_codes = array('LUN', 'MAR', 'MER', 'JEU', 'VEN');
	$week_day_labels = array('Lundi', 'Mardi', 'Mercredi', 'Jeudi', 'Vendredi');
	$weekend_hour_codes = array('0800', '0900', '1000', '1100', '1200', '1300', '1400', '1500', '1600', '1700');
	$weekend_hour_labels = array('8:00', '9:00', '10:00', '11:00', '12:00', '13:00', '14:00', '15:00', '16:00', '17:00');
	$weekend_day_codes = array('SAM', 'DIM');
	$weekend_day_labels = array('Samedi', 'Dimanche');

	foreach($schedule_periods as $row) {
		
		// ASSERT : valid days, valid hours
		
		// If is week
		if(array_search($row['weekday'], $week_day_codes) !== FALSE) {
			// If is during day
			if(array_search($row['time'], $week_hour_codes) !== FALSE) {
				if(!isset($week[$row['weekday']][$row['time']])) {
					$week[$row['weekday']][$row['time']] = array();
				}
				array_push($week[$row['weekday']][$row['time']], $row);
				#echo "pushed";
			}
			// If is during evening
			elseif(array_search($row['time'], $week_nighthours_codes) !== FALSE) {
				$week_have_evening=1;
				if(!isset($week[$row['weekday']][$row['time']])) {
					$week[$row['weekday']][$row['time']] = array();
				}
			
				array_push($week[$row['weekday']][$row['time']], $row);
			}
			// If is nonstandard time
			else {
				$week_have_nonstandard=1;
				if(!isset($week[$row['weekday']][$row['time']])) {
					$week_nonstd[$row['weekday']][$row['time']] = array();
				}
		
				array_push($week_nonstd[$row['weekday']][$row['time']], $row);
			}
		}
		// If is weekend
		elseif(array_search($row['weekday'], $weekend_day_codes) !== FALSE) {
			$weekend_have=1;
			// If standard time
			if(array_search($row['time'], $weekend_hour_codes) !== FALSE) {
				if(!isset($weekend[$row['weekday']][$row['time']])) {
					$weekend[$row['weekday']][$row['time']] = array();
				}
				array_push($weekend[$row['weekday']][$row['time']], $row);
			}
			// If nonstandard time
			else {
				$weekend_have_nonstandard=1;
				if(array_search($row['time'], $weekend_hour_codes) !== FALSE) {
					if(!isset($weekend_nonstd[$row['weekday']][$row['time']])) {
						$weekend_nonstd[$row['weekday']][$row['time']] = array();
					}
					array_push($weekend_nonstd[$row['weekday']][$row['time']], $row);
				}
			}
		}
		else {
			admin_error("got invalid weekday (".$row['weekday'].")");
		}
	}
	$prof_string .= "process period query results: ".(microtime(TRUE)-$tmp)."<br />\n";
	
	// Draw the week schedule
	$tmp=microtime(TRUE);


	// We pass 2 times in the drawing code. The first one (0) is to draw the week schedule,
	// the second (1) is for the weekend schedule.

	for($pass=0; $pass<2; $pass++) {

		if($pass == 1 and $weekend_have !== 1) {
			continue;
		}

		if($pass == 0) {
			echo '<table class="schedule">'."\n";
			$hourcodes = $week_hour_codes;
			$hourlabels = $week_hour_labels;
			$daycodes = $week_day_codes;
			$daylabels = $week_day_labels;
	
			if($week_have_evening) {
				$hourcodes = array_merge($hourcodes, $week_nighthours_codes);
				$hourlabels = array_merge($hourlabels, $week_nighthours_labels);
			}
		}
		elseif($pass == 1) {
			echo '<table class="schedule" style="clear:none;">'."\n";
			$hourcodes = $weekend_hour_codes;
			$hourlabels = $weekend_hour_labels;
			$daycodes = $weekend_day_codes;
			$daylabels = $weekend_day_labels;

			$week = $weekend;
		}

		// print the day headers on this subschedule
		echo "<tr>\n";
		echo "\t".'<td class="whitecorner"></td>'."\n";
		foreach($daylabels as $lb) {
			echo "\t".'<td class="weekday">'.$lb.'</td>'."\n";
		}
		echo "</tr>\n";
	
		for($hourindex=0; $hourindex < count($hourcodes); $hourindex++) {
			echo "	<tr>\n";
			echo "		<td class=\"hour\"><b>".$hourlabels[$hourindex]."</b></td>\n";
			for($dayindex=0; $dayindex < count($daycodes); $dayindex++) {
				echo "		<td>";
				if(! $week[$daycodes[$dayindex]][$hourcodes[$hourindex]]) {
					echo "</td>";
					continue;
				}
				// See what kind of conflict we have
				$conflict=0;
				$cumul_mask=0;
				foreach($week[$daycodes[$dayindex]][$hourcodes[$hourindex]] as $period) {
					if($period['week'] == "A") {
						$mask=3;	
					}
					elseif($period['week'] == "B1") {
						$mask=1;
					}
					elseif($period['week'] == "B2") {
						$mask=2;
					}
					else {
						admin_error("Invalid week");
					}
					if($cumul_mask & $mask) {
						$conflict |= ($cumul_mask & $mask);
					}
					$cumul_mask |= $mask;
				}
				// Now print the courses at this hour
				foreach($week[$daycodes[$dayindex]][$hourcodes[$hourindex]] as $period) {
					if($period['tol'] == 'C') {
						$tol = 'TH';
					}
					else {
						$tol = 'LAB';
					}
					
					if($period['week'] == "A") {
						$b1b2="";
						$mask=3;
					}
					elseif($period['week'] == "B1") {
						$b1b2=" <b>&lt;B1&gt;</b>";
						$mask=1;
					}
					elseif($period['week'] == "B2") {
						$b1b2=" <b>&lt;B2&gt;</b>";
						$mask=2;
					}
					else {
						admin_error("Invalid week");
					}
					
					if($mask & $conflict) {
						echo "<div class=\"period_conflict\">";
					}
					else {
						echo "<div class=\"period_noconflict\">";
					}
					echo "<b>".$period['symbol']."</b> (".$period['grp'].")<br />[".$tol."] ".$period['room'].$b1b2;
					echo "</div>";
				}
				echo "		</td>\n";
			}
			echo "	</tr>\n";
		}
		if($week_have_nonstandard and $pass == 0) {
			echo "          <td class=\"hour\"><b>Heures non standard</b></td>\n";
			for($dayindex=0; $dayindex < count($daycodes); $dayindex++) {
				echo "          <td>";
				if(!count($week_nonstd[$daycodes[$dayindex]])) {
					echo "</td>";
					continue;
				}
				ksort($week_nonstd[$daycodes[$dayindex]]);
				foreach($week_nonstd[$daycodes[$dayindex]] as $time => $periods) {
					foreach($periods as $period) {
						if($period['tol'] == 'C') {
							$tol = 'TH';
						}
						else {
							$tol = 'LAB';
						}
	
						if($period['week'] == "A") {
							$b1b2="";
						}
						elseif($period['week'] == "B1") {
							$b1b2=" <b>&lt;B1&gt;</b>";
						}
						elseif($period['week'] == "B2") {
							$b1b2=" <b>&lt;B2&gt;</b>";
						}
						else {
							admin_error("Invalid week");
						}
					
						echo "<div class=\"period_noconflict\">";
						echo "<b><u>".preg_replace("/(.*)(..)/", "$1:$2", $time)."</u><br />".$period['symbol']."</b> (".$period['grp'].")<br />[".$tol."] ".$period['room'].$b1b2;
						echo "</div>\n";
					}
				}
				echo "          </td>\n";
			}
	
		}
		echo "</table>\n";

	} // end of pass

	//$php_function_time=microtime(TRUE)-$php_function_time;
	//$prof_string .= "draw schedule: ".(microtime(TRUE)-$tmp)."<br />\n";
	//echo "php function time: " . $php_function_time . "<br />".$prof_string."<br />\n";
}


/* array_intersect_key2 has the same functionality as the
   PHP5 array_intersect_key function used with 2 arguments.
   by Benjamin Poirier
*/

function array_intersect_key2($array1, $array2) 
{ 
	$output= array(); 
     
	foreach ($array1 as $key => $value) { 
		if (array_key_exists($key, $array2)) { 
			$output[$key]= $value;
		} 
	} 
     
	return $output; 
} 


// note: this is not a real xor, if two characters are the same they will be or'ed to avoid string terminators
function xorString($string1, $string2)
{
	//we make sure string2 is the longest one
	if (strlen($string1) > strlen($string2))
	{
		$temp= $string2;
		$string2= $string1;
		$string1= $temp;
	}
	
	$len1= strlen($string1);
	
	$i= 0;
	while($i < strlen($string2))
	{
		// if the characters are the same this will generate a string terminator '0' which will eventually cause the crypt function to end before the end of the string, this is not desired
		if ($string1[$i % $len1] == $string2[$i])
		{
			$result[$i]= $string2[$i];
		}
		else
		{
			$result[$i]= $string1[$i % $len1] ^ $string2[$i];
		}
		$i++;
	}
	
	return implode("", $result);
}

function getHash($email, $salt=0)
{
	global $CONFIG_VARS;
	
	if ($salt)
	{
		return crypt(xorString($email, $CONFIG_VARS["emailer.pepper"]), $salt);
	}
	else
	{
		return crypt(xorString($email, $CONFIG_VARS["emailer.pepper"]));
	}
}

/* by cam at wecreate dot com - http://php.net/manual/en/function.ip2long.php#54953 */
function ipcompare ($ip1, $ip2, $mask)
{
	$masked1 = ip2long($ip1) & ip2long($mask); // bitwise AND of $ip1 with the mask
	$masked2 = ip2long($ip2) & ip2long($mask); // bitwise AND of $ip2 with the mask
	
	if ($masked1 == $masked2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

function string2varname ($str)
{
	for($c = 0; $c < strlen($str); $c++) {
		if(ord($str{$c}) >= ord("A") and ord($str{$c}) <= ord("Z")) {
			$str{$c} = chr(ord($str{$c}) + ord("a") - ord("A"));
		}
		else if(ord($str{$c}) >= ord("0") and ord($str{$c}) <= ord("9")) {
			if($c == 0) {
				$str{$c} = "_";
			}
		}
		else {
			$str{$c} = "_";
		}
	}

	return $str;
}

?>

