<?php
require_once('config.php');
require_once('lib.php');
read_config_file($SOPTI_CONFIG_FILE);

ob_start();
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
	<title>Horaire d'un chargé de cours</title>
	<link rel="stylesheet" type="text/css" href="listcourses.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<h1>Horaire d'un chargé de cours</h1>

<pre>

<?php
        $dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
                or admin_error('Could not connect to SQL: ' . mysql_error());
        mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select data
base');
	
	// Make the query
	$query = "SELECT groups.teacher AS teacher FROM groups INNER JOIN courses_semester ON courses_semester.unique=groups.course_semester INNER JOIN courses ON courses.unique=courses_semester.course INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS["default_semester"]."' GROUP BY teacher ORDER BY teacher";
	
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());
	mysql_close($dblink);
?>
<form action="teacher.php" method="get">
	<select name="teacher">
<?php	
	while ($row = mysql_fetch_row($result)) {
		echo "\t\t<option>".$row[0] . "</option>\n";
	}
?>
	</select>
	<input type="submit" value="Voir >">
</form>
</pre>
</body>
</html>
