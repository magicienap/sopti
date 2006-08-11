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

echo "<h1>Horaire d'un charg� de cours</h1>\n";
echo "<h2>".htmlspecialchars($teacher, ENT_QUOTES)."</h2>\n";
echo "<p>".date("Y/m/d H:i:s")."</p>\n";

print_teacher_schedule($teacher);

?>

<p style="clear: left;">Avertissement: Cet horaire est construit � partir de l'horaire g�n�ral de l'�cole Polytechnique. Il ne contient que les charges de cours donn�es � l'�cole Polytechnique. Il n'a aucun caract�re officiel.</p>

</body>
</html>
