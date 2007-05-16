<?php
require_once('config.php');
require_once('lib.php');
read_config_file($SOPTI_CONFIG_FILE);

ob_start();
header("Content-type: text/javascript");
?>
collection = [
<?php
        $dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
                or admin_error('Could not connect to SQL: ' . mysql_error());
        mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select data
base');
	
	// Make the query
	$query = "SELECT courses.symbol AS sym, courses.title from courses INNER JOIN courses_semester ON courses.unique=courses_semester.course INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS['default_semester']."' ORDER BY courses.symbol";
	
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());
	mysql_close($dblink);

	$first=1;
	while ($row = mysql_fetch_row($result)) {
		if($first == 1) {
			$first = 0;
		}
		else {
			echo ",\n";
		}
			
		echo "'" . $row[0] ." ".ereg_replace("'", "\\'", $row[1])."'";
	}
?>

];

