<?php
# ob_start();
?>

<html>

<head>
	<title>Générer des horaires - Étape 1</title>
	<link rel="stylesheet" type="text/css" href="sopti.css" />
	<link rel="stylesheet" type="text/css" href="./wick.css" />
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body>

<div id="header">

<img src="genhor_sm.png" alt="Générateur d'horaires" />

<h1>Générer des horaires</h1>

<p class="step_current">Étape 1 - Spécifier les cours désirés</p>
<p class="step_notcurrent">Étape 2 - Choisir les options de génération</p>
<p class="step_notcurrent">Étape 3 - Visualiser les horaires</p>

</div>

<form method="get" action="make_form2.php">

<?php
/*
$course_file="../../sopti_run/data/courses.csv";

$info = stat($course_file);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);

print "Dernière mise a jour du fichier de cours: ".$string_modif
*/
?>

<div class="option_block">
	<h2>Cours désirés</h2>
	<p>Écrire les sigles des cours désirés, séparés par un espace et/ou une virgule. Ex: MTH1101 MTH1006 INF1005C<br />
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
