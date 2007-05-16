<?php
# ob_start();
?>

<html>

<head>
	<title>G�n�rer des horaires - �tape 1</title>
	<link rel="stylesheet" type="text/css" href="sopti.css" />
	<link rel="stylesheet" type="text/css" href="./wick.css" />
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body>

<div id="header">

<img src="genhor_sm.png" alt="G�n�rateur d'horaires" />

<h1>G�n�rer des horaires</h1>

<p class="step_current">�tape 1 - Sp�cifier les cours d�sir�s</p>
<p class="step_notcurrent">�tape 2 - Choisir les options de g�n�ration</p>
<p class="step_notcurrent">�tape 3 - Visualiser les horaires</p>

</div>

<form method="get" action="make_form2.php">

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
	<p>�crire les sigles des cours d�sir�s, s�par�s par un espace et/ou une virgule. Ex: MTH1101 MTH1006 INF1005C<br />
	<a href="listcourses.php">Voir la liste des cours offerts</a></p>
	<p><input name=courses class="wickEnabled" type="text" size="80" />&nbsp;&nbsp;<input type="submit" value="Continuer..." /></p>
</div>

</form>

	<!--<script type="text/javascript" language="JavaScript" src="./courses_wick_data.php"></script>-->
	<script type="text/javascript" language="JavaScript" src="sample_data1.js"></script>
	<script type="text/javascript" language="JavaScript" src="./wick.js"></script>
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
