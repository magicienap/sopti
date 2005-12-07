<?php
require_once('config.php');
require_once('lib.php');

read_config_file($SOPTI_CONFIG_FILE);

$dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"])
	or die('Could not connect: ' . mysql_error());

mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select database');

ob_start();

echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
';

// Step 1
if (isset($_GET["unique"]))
{
	echo <<<BENRULES
	<html>
	
	<head>
		<title>Notification par email</title>
		<link rel="stylesheet" type="text/css" href="sopti.css">
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	</head>
	
	<body>
	
	<div id="header">
	
	<img src="genhor_sm.png" alt="G�n�rateur d'horaires">
	
	<h1>Notification par email</h1>
	
	<p class="step_current">�tape 1 - Sp�cifier le email
	<p class="step_notcurrent">�tape 2 - Confirmation
	
	</div>
	
	<form method="POST" action="email_form.php">
	
	<h2>Groupe demand�</h2>
	<div class="option_block">
		<p>
BENRULES;
	
	$resultat= mysql_query("
		select 
			`groups`.`name` as 'groups.name', 
			`groups`.`theory_or_lab` as 'groups.theory_or_lab', 
			`groups`.`teacher` as 'groups.teacher', 
			`groups`.`closed` as 'groups.closed', 
			`courses_semester`.`course_type` as 'courses_semester.course_type', 
			`courses`.`symbol` as 'courses.symbol', 
			`courses`.`title` as 'courses.title' 
		from `groups` 
		inner join `courses_semester` on `groups`.`course_semester` = `courses_semester`.`unique` 
		inner join `courses` on `courses_semester`.`course` = `courses`.`unique` 
		where `groups`.`unique` = '" . mysql_escape_string($_GET["unique"]) . "'
		limit 1
	") or die('Query failed: ' . mysql_error());
	mysql_close($dblink);
	
	if (!mysql_num_rows($resultat))
	{
		error("Le groupe demand� n'a pas �t� trouv�, veuillez r�essayer en cliquant sur un des liens de la deuxi�me page du syst�me de g�n�ration d'hoaires");
	}
	
	$cours= mysql_fetch_assoc($resultat);
	
	if ($cours["groups.closed"] != 1)
	{
		error("Ce groupe est d�j� ouvert, vous pouvez aller vous inscrire tout de suite!");
	}
	
	if ($cours["courses_semester.course_type"] == "TL")
	{
		$typeCours= "combin�e th�orie et laboratoire";
	}
	else if ($cours["groups.theory_or_lab"] == "C")
	{
		$typeCours= "th�orique";
	}
	else if ($cours["groups.theory_or_lab"] == "L")
	{
		$typeCours= "laboratoire";
	}
	else
	{
		error("Erreur interne, type de cours inconnu");
	}
	
	echo $cours["courses.symbol"] . " - " . $cours["courses.title"] . "<br>
	Section " . $cours["groups.name"] . " pour la partie " . $typeCours . " du cours<br>
	Enseignant(e): " . $cours["groups.teacher"];
	
	echo '<input type="hidden" name="unique" value="' . $_GET["unique"] . '">';
	
	echo <<<BENRULES
	</div>
	
	<h2>Adresse email</h2>
	<div class="option_block">
		<p>Adresse email o� envoyer la notification:<br>
BENRULES;
	
	if (isset($_COOKIE["email_notification"]))
	{
		echo '<input type="text" name="email" size="40" value="' . htmlentities($_COOKIE["email_notification"]) . '">';
	}
	else
	{
		echo '<input type="text" name="email" size="40">';
	}
	
	echo <<<BENRULES
	</div>
	
	<h2>Terminer</h2>
	<div class="option_block">
		<p style="width:400px;"><b>Attention</b><br>Ce syst�me sert seulement � vous avertir lorsqu'une section devient ouverte, il ne peut pas faire le changement pour vous. Il est possible que quelqu'un d'autre s'inscrive entre le moment ou une place devient disponible et le moment ou vous recevez le email. Faites vite!
		<p><input type="submit" value="Soumettre" name="action">
	</div>
	
	</form>
	
	</body>
	</html>
BENRULES;
}
// Step 2
else if (isset($_POST["action"]))
{
	global $CONFIG_VARS;
	
	$resultat= mysql_query("
		select 
			`groups`.`closed` as 'groups.closed' 
		from `groups` 
		where `groups`.`unique` = '" . mysql_escape_string($_POST["unique"]) . "'
		limit 1
	") or die('Query failed: ' . mysql_error());
	
	if (!$resultat)
	{
		error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
	}
	else if (mysql_num_rows($resultat) < 1)
	{
		error("Le groupe demand� n'a pas �t� trouv�");
	}
	
	$cours= mysql_fetch_assoc($resultat);
	
	if ($cours["groups.closed"] != 1)
	{
		error("Ce groupe est d�j� ouvert, vous pouvez aller vous inscrire tout de suite!");
	}
	
	if (!isset($_POST["email"]))
	{
		error("Vous n'avez pas fourni d'adresse email");
	}
	else	if (strlen($_POST["email"]) > 60)
	{
		error("L'adresse email que vous avez fournie est trop longue");
	}
	else if (!preg_match("/^([0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*@(([0-9a-zA-Z])+([-\w]*[0-9a-zA-Z])*\.)+[a-zA-Z]{2,11})$/", $_POST["email"]))
	{
		error("L'adresse email que vous avez fournie est invalide");
	}
	
	// check the group limit (1)
	$resultat= mysql_query("
		select 
			count(*) 
		from `notifications` 
		where `notifications`.`group` = '" . mysql_escape_string($_POST["unique"]) . "' and `notifications`.`email` = '" . mysql_escape_string($_POST["email"]) . "'
	") or die('Query failed: ' . mysql_error());
	
	if (!$resultat || mysql_num_rows($resultat) < 1)
	{
		error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
	}
	else if (mysql_result($resultat, 0) > 0)
	{
		error('Vous avez d�j� une requ�te en attente pour ce groupe, si vous le d�sirez, vous pouvez <a href="email_unsubscribe.php" target="_blank">annuler</a> cette requ�te.');
	}
	
	// check the request limit
	$resultat= mysql_query("
		select 
			count(*) > '" . (int) ($CONFIG_VARS["emailer.maxReq"] - 1) . "'
		from `notifications` 
		where `notifications`.`email` = '" . mysql_escape_string($_POST["email"]) . "'
	") or die('Query failed: ' . mysql_error());
	
	if (!$resultat || mysql_num_rows($resultat) < 1)
	{
		error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
	}
	else if (mysql_result($resultat, 0))
	{
		error('Vous avez atteint votre maximum de demandes en attente, si vous le d�sirez, vous pouvez <a href="email_unsubscribe.php" target="_blank">annuler</a> certaines de vos autres demandes d\'abord puis revenir ajouter celle-ci.');
	}
	
	// check the ip limit
	// exclude ip's from inside the school's address range
	if (!ipcompare($_SERVER["REMOTE_ADDR"], "132.207.0.0", "255.255.0.0"))
	{
		$resultat= mysql_query("
			select 
				count(*) > '" . (int) ($CONFIG_VARS["emailer.maxIP"] - 1) . "'
			from `notifications` 
			where `notifications`.`ip` = '" . mysql_escape_string($_SERVER["REMOTE_ADDR"]) . "'
		") or die('Query failed: ' . mysql_error());
		
		if (!$resultat || mysql_num_rows($resultat) < 1)
		{
			error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
		}
		else if (mysql_result($resultat, 0))
		{
			error('Vous avez atteint votre maximum de demandes en attente pour ce poste, veuillez communiquer avec nous (<a href="mailto:' . $CONFIG_VARS["admin_email"] . '">' . $CONFIG_VARS["admin_email"] . '</a>) si vous d�sirez ajouter d\'autres demandes');
		}
	}
	
	$resultat= mysql_query("
		insert
			into `notifications`
			(`group`, `email`, `ip`)
			values 
			('" . mysql_escape_string($_POST["unique"]) . "', '" . mysql_escape_string($_POST["email"]) . "', '" . mysql_escape_string($_SERVER["REMOTE_ADDR"]) . "')
	") or die('Query failed: ' . mysql_error());
	
	if (!isset($_COOKIE["email_notification"]) or $_COOKIES["email_notification"] != $_POST["email"])
	{
		setcookie("email_notification", $_POST["email"], time()+60*60*24*31*9);
	}

echo <<<BENRULES
	<html>
	
	<head>
		<title>Notification par email - �tape 2</title>
		<link rel="stylesheet" type="text/css" href="sopti.css">
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	</head>
	
	<body>
	
	<div id="header">
	
	<img src="genhor_sm.png" alt="G�n�rateur d'horaires">
	
	<h1>Notification par email</h1>
	
	<p class="step_notcurrent">�tape 1 - Sp�cifier le email
	<p class="step_current">�tape 2 - Confirmation
	
	</div>
	
	<h2>Confirmation</h2>
	<div class="option_block">
		<p>
BENRULES;

	echo 'Une demande de notification a �t� ajout�e � la liste sous le email ' . htmlentities($_POST["email"], ENT_QUOTES) . '.';
	
	echo <<<BENRULES
	<p>Vous pouvez maintenant fermer cette fen�tre et retourner � la 
	g�n�ration d'horaire ou vous inscrire pour �tre notifi� pour un	autre groupe.<br>
	Si vous changez d'id�e, vous pouvez <a href="email_unsubscribe.php" target="_blank">annuler cette requ�te</a>.
	
	<form method="GET" action="javascript:window.close();">
		<input type="submit" value="Fermer cette fen�tre">
	</form>
	</div>
	
	</body>
	</html>
BENRULES;
}
else
{
	error("Le groupe demand� n'a pas �t� sp�cifi�, veuillez r�essayer en cliquant sur un des liens de la deuxi�me page du syst�me de g�n�ration d'hoaires");
}

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
