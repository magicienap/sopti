<?php
require_once('config.php');
require_once('lib.php');

ob_start();
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
	<title>Cours offerts</title>
	<link rel="stylesheet" type="text/css" href="listcourses.css">
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>

<body>

<h1>Liste des cours offerts</h1>

<pre>

<?php
        $dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
                or admin_error('Could not connect to SQL: ' . mysql_error());
        mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select data
base');
	
	// Make the query
	$query = "SELECT courses.symbol AS symbol,courses.title AS title FROM courses INNER JOIN courses_semester ON courses_semester.course=courses.unique INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS["default_semester"]."' ORDER BY courses.symbol";
	
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());
	mysql_close($dblink);
	
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
		echo $row["symbol"] . " " . $row["title"] . "\n";
	}
?>
</pre>
</body>
</html>
