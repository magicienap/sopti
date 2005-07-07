<?php
require_once('config.php');

ob_start();
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
	<title>Cours offerts</title>
	<link rel="stylesheet" type="text/css" href="listcourses.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<h1>Liste des cours offerts</h1>

<pre>

<?php
	$dblink = mysql_connect('', 'poly', 'pol')
		or die('Could not connect: ' . mysql_error());
	mysql_select_db('poly_courses') or die('Could not select database');
	
	// Make the query
	$query = "SELECT courses.symbol AS symbol,courses.title AS title FROM courses ORDER BY courses.symbol";
	
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());
	mysql_close($dblink);
	
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
		echo $row["symbol"] . " " . $row["title"] . "\n";
	}
?>
</pre>
</body>
</html>
