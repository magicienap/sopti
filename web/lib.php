<?php

function admin_error($msg)
{
        ob_end_clean();
?>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.or
g/TR/html4/loose.dtd">
<html>

<head>
  <title>Générateur d'horaires - Erreur interne</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
        <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<center>

<img src="aep.gif">
<p><font size="+2">Générateur d'horaires</font>
</center>

<div style="background-color: #ffbbbb;">
<p align="center"><b><font size="+1">ERREUR INTERNE</font></b></p>
<p align="center"><?php echo $msg; ?></p>
</div>

<p align="center">L'administrateur a été informé de cette erreur.
</body>
</html>

<?php

        exit();

}

function error($msg)
{
	ob_end_clean();
?>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.or
g/TR/html4/loose.dtd">
<html>

<head>
  <title>Générateur d'horaires - Erreur</title>
  <link rel="stylesheet" type="text/css" href="sopti.css">
        <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>

<center>

<img src="aep.gif">
<p><font size="+2">Générateur d'horaires</font>
</center>

<div style="background-color: #ffbbbb;">
<p align="center"><b><font size="+1">ERREUR</font></b></p>
<p align="center"><?php echo $msg; ?></p>
</div>

<p align="center">Utiliser le bouton précédent de votre navigateur pour revenir en arrière et corriger l'erreur.
</body>
</html>

<?php

	exit();
}

$CONFIG_VARS=array();

function read_config_file($file)
{
	global $CONFIG_VARS;
	
	$handle = fopen($file, "r");
	if($handle == FALSE) {
		error("Impossible d'ouvrir le fichier de configuration " . $file);
	}
	while (!feof($handle)) {
		$line = fgets($handle, 4096);
		$origline = $line;
		
		// Remove final newline
		$line = rtrim($line, "\n");
		// Remove comments
		$line = ereg_replace("([^#]*)#.*", "\\1", $line);
		// Ignore empty lines
		if (ereg("^[ \t]*$", $line)) {
			continue;
		}
		// Try unquoted argument format
		if(ereg("^[ \t]*([^ \t]+)[ \t]+([^ \t\"]+)[ \t]*$", $line, $args)) {
			$varname = $args[1];
			$varval = $args[2];
			$CONFIG_VARS[$varname] = $varval;
			//echo "$varname $varval\n";
		}
		// Try quoted argument format
		elseif(ereg("^[ \t]*([^ \t]+)[ \t]+\"([^\"]*)\"[ \t]*$", $line, $args)) {
			$varname = $args[1];
			$varval = $args[2];
			$CONFIG_VARS[$varname] = $varval;
			//echo "$varname $varval\n";
		}
		else {
			error("syntax error in config file at line: " . $origline);
		}
	}
	fclose($handle);
}

?>

