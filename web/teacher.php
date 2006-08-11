<?php
require_once('lib.php');

ob_start();
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
	<title>Horaire de salle</title>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	<link rel="stylesheet" type="text/css" href="make.css" />
	<link rel="stylesheet" type="text/css" href="sopti.css" />
</head>

<body>

<?php
$teacher = $_GET['teacher'];

echo "<h1>Horaire d'un chargé de cours</h1>\n";
echo "<h2>".htmlspecialchars($teacher, ENT_QUOTES)."</h2>\n";
echo "<p>".date("Y/m/d H:i:s")."</p>\n";

print_teacher_schedule($teacher);

?>

<p style="clear: left;">Avertissement: Cet horaire est construit à partir de l'horaire général de l'École Polytechnique. Il ne contient que les charges de cours données à l'École Polytechnique. Il n'a aucun caractère officiel.</p>

</body>
</html>
