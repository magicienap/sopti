<?php
# ob_start();
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
	<title>G�n�rer des horaires - �tape 1</title>
	<link rel="stylesheet" type="text/css" href="sopti.css">
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<div id="header">

<img src="aep.gif">

<h1>G�n�rer des horaires</h1>

<p class="step_current">�tape 1 - Sp�cifier les cours d�sir�s
<p class="step_notcurrent">�tape 2 - Choisir les options de g�n�ration
<p class="step_notcurrent">�tape 3 - Visualiser les horaires

</div>

<p>

<form method="GET" action="make_form2.php">

<?php
/*
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Derni�re mise a jour du fichier de cours: ".$string_modif
*/
?>

<div class="option_block">
	<h2>Cours d�sir�s</h2>
	<p>�crire les sigles des cours d�sir�s, s�par�s par un espace. Ex: ING1040 ING1035 ING1010<br>
	<a href="listcourses.php" target="_blank">Voir la liste des cours offerts</a>
	<p><textarea name="courses" cols="50" rows="2"></textarea>
</div>


<div class="option_block">
	<p><input type="submit" value="Continuer...">
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
