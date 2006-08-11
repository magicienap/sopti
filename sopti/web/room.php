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
$room = $_GET['room'];

echo "<h1>Horaire de la salle ".htmlspecialchars($room, ENT_QUOTES)."</h1>\n";
echo "<p>".date("Y/m/d H:i:s")."</p>\n";

print_room_schedule($room);

?>

<p style="clear: left;">Avertissement: Cet horaire est construit � partir de l'horaire g�n�ral de l'�cole Polytechnique. Il ne contient que les cours officiellement donn�es � l'�cole Polytechnique. D'autres activit�s peuvent occuper un local. Pour conna�tre l'horaire complet d'un local ou le r�server, communiquer avec le service responsable du local en question.</p>

</body>
</html>
