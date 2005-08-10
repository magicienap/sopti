<?php
require_once('lib.php');

$dblink = mysql_connect($CONFIG_VARS["db.host"], $CONFIG_VARS["db.username"], $CONFIG_VARS["db.password"]) or die('Could not connect: ' . mysql_error());
mysql_select_db($CONFIG_VARS["db.schema"]) or die('Could not select database');

ob_start();

echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
';

// Step 1
if (isset($_GET["email"]) || isset($_GET["hash"]))
{
	if (!isset($_GET["email"]))
	{
		error("Email manquant, v�rifier le lien utilis� ou refaire la demande par email");
	}
	else if (!isset($_GET["hash"]))
	{
		error("Hash manquant, v�rifier le lien utilis� ou refaire la demande par email");
	}
	
	if ($_GET["hash"] != getHash($_GET["email"], $_GET["hash"]))
	{
		error("Hash invalide, v�rifier le lien utilis� ou refaire la demande par email");
		#error("This will help debugging: " . htmlentities(urlencode(getHash($_GET["email"])), ENT_QUOTES));
	}
	
	echo <<<BENRULES
	<html>
	
	<head>
		<title>D�sinscription de la notification par email - �tape 1</title>
		<link rel="stylesheet" type="text/css" href="sopti.css">
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	</head>
	
	<body>
	
	<div id="header">
	
	<img src="aep.gif">
	
	<h1>D�sinscription de la notification par email</h1>
	
	<p class="step_notcurrent">�tape 0 - Code d'acc�s
	<p class="step_current">�tape 1 - Choisir les demandes � annuler
	<p class="step_notcurrent">�tape 2 - Confirmation
	
	</div>
	
	<form method="POST" action="email_unsubscribe.php" name="form">
	
	<p>
	
	<h2>Demandes en attente</h2>
	<div class="option_block">
		<p>
		Veuillez s�lectionner les demandes que vous d�sirez annuler:<br>
BENRULES;
	
	echo '<input type="hidden" name="email" value="' . htmlentities($_GET["email"], ENT_QUOTES) . '">';
	echo '<input type="hidden" name="hash" value="' . htmlentities($_GET["hash"], ENT_QUOTES) . '">';
	
	$resultat= mysql_query("
		select 
			`groups`.`name` as 'groups.name', 
			`groups`.`theory_or_lab` as 'groups.theory_or_lab', 
			`groupsT`.`teacher` as 'groupsT.teacher', 
			`groupsL`.`teacher` as 'groupsL.teacher', 
			`groups`.`closed` as 'groups.closed', 
			`groups`.`unique` as 'groups.unique', 
			`courses_semester`.`course_type` as 'courses_semester.course_type', 
			`courses`.`symbol` as 'courses.symbol', 
			`courses`.`title` as 'courses.title' 
		from `notifications` 
		inner join `groups` on 
			`notifications`.`group` = `groups`.`unique` 
		left join `groups` as `groupsT` on 
			`groups`.`course_semester` = `groupsT`.`course_semester` and 
			`groups`.`name` = `groupsT`.`name` and 
			`groupsT`.`theory_or_lab` = 'C' 
		left join `groups` as `groupsL` on 
			`groups`.`course_semester` = `groupsL`.`course_semester` and 
			`groups`.`name` = `groupsL`.`name` and 
			`groupsL`.`theory_or_lab` = 'L' 
		inner join `courses_semester` on 
			`groups`.`course_semester` = `courses_semester`.`unique` 
		inner join `courses` on 
			`courses_semester`.`course` = `courses`.`unique` 
		where 
			`notifications`.`email` = '" . mysql_escape_string($_GET["email"]) . "'
	") or die('Query failed: ' . mysql_error());
	
	if (!mysql_num_rows($resultat))
	{
		error("Vous n'avez aucune demande de notification en attente");
	}
	
	echo '<a href="javascript:openOCChecks(0,' . mysql_num_rows($resultat) . ')" class="oclink">(S�lectionner toutes les demandes)</a><br/>';
	
	# while($cours= mysql_fetch_assoc($resultat))
	# {	
		# if ($cours["courses_semester.course_type"] == "TLS")
		# {
			# $typeCours= "combin�e th�orie et laboratoire";
		# }
		# else if ($cours["groups.theory_or_lab"] == "C")
		# {
			# $typeCours= "th�orique";
		# }
		# else if ($cours["groups.theory_or_lab"] == "L")
		# {
			# $typeCours= "laboratoire";
		# }
		# else
		# {
			# error("Erreur interne, type de cours inconnu");
		# }
		
		# echo 
		# $cours["courses.symbol"] . " - " . $cours["courses.title"] . "<br>
		# Section " . $cours["groups.name"] . " pour la partie " . $typeCours . " du cours<br>
		# Enseignant(e): " . $cours["groups.teacher"];
	# }
	
	while ($row = mysql_fetch_array($resultat, MYSQL_ASSOC)) {
		$group_info[$row["courses.symbol"]][$row["groups.theory_or_lab"]][$row["groups.name"]]["teacherT"] = $row['groupsT.teacher'];
		$group_info[$row["courses.symbol"]][$row["groups.theory_or_lab"]][$row["groups.name"]]["teacherL"] = $row['groupsL.teacher'];
		$group_info[$row["courses.symbol"]][$row["groups.theory_or_lab"]][$row["groups.name"]]["closed"] = $row['groups.closed'];
		$group_info[$row["courses.symbol"]][$row["groups.theory_or_lab"]][$row["groups.name"]]["unique"] = $row['groups.unique'];
		$course_info[$row["courses.symbol"]]["type"] = $row["courses_semester.course_type"];
		$course_info[$row["courses.symbol"]]["title"] = $row["courses.title"];
	}
	
	$offsetOCChecks= 0;
	foreach($group_info as $sym => $entry1)
	{
		// Determine which types will form one block in the course form
		if($course_info[$sym]["type"] == 'T') {
			$types_to_consider = array("C" => 1);
		}
		elseif($course_info[$sym]["type"] == 'L') {
			$types_to_consider = array("L" => 1);
		}
		elseif($course_info[$sym]["type"] == 'TL') {
			$types_to_consider = array("C" => 1);
		}
		elseif($course_info[$sym]["type"] == 'TLS') {
			$types_to_consider = array("C" => 1, "L" => 1);
		}
		else {
			die("invalid course type");
		}
		
		echo '<table class="openclose">';
		echo '<tr><td class="title"><div class="title">' . $sym . "<br>" . $course_info[$sym]["title"] . '<br><a href="http://www.polymtl.ca/etudes/cours/details.php?sigle=' . $sym . '" target="_blank">D�tails et horaire du cours</a></div></td></tr>';
		
		foreach(array_intersect_key2($entry1, $types_to_consider) as $type => $entry2)
		{
			// compute the contents of $group_type_string
			if($course_info[$sym]["type"] == 'T' && $type == 'C') {
				$group_type_string = "Th�orique";
				$cb_name_prefix = 't';
			}
			elseif($course_info[$sym]["type"] == 'L' && $type == 'L') {
				$group_type_string = "Laboratoire";
				$cb_name_prefix = 'l';
			}
			elseif($course_info[$sym]["type"] == 'TL' && $type == 'C') {
				$group_type_string = "Th�orique et laboratoire";
				$cb_name_prefix = 't';
			}
			elseif($course_info[$sym]["type"] == 'TLS' && $type == 'C') {
				$group_type_string = "Th�orique";
				$cb_name_prefix = 't';
			}
			elseif($course_info[$sym]["type"] == 'TLS' && $type == 'L') {
				$group_type_string = "Laboratoire";
				$cb_name_prefix = 't';
			}
			else {
				die("internal error");
			}
			
			$nbOCChecks= count($entry2);
			
			echo '<tr><td class="type">' . $group_type_string . '<br><a href="javascript:openOCChecks(' . $offsetOCChecks . ',' . $nbOCChecks . ')" class="oclink">(S�lectionner toutes les demandes de ce cours)</a></td></tr>';
			$offsetOCChecks+= $nbOCChecks;
			
			foreach($entry2 as $groupname => $entry3) {
				echo '<!-- nested tables are used to avoir a bug in IE 4-6 concerning col widths and colspans -->';
				echo '<tr><td class="nested"><table class="nested"><tr>';
				if($entry3["closed"] == 1) {
					echo '<td class="closed">';
				}
				else {
					echo '<td class="openned">';
				}
				
				// Print group info
				echo $groupname . '<input name="' . $entry3["unique"] . '" type="checkbox"></td>';
				echo '<td class="teacher">'; // Begin teacher td
				
				if($course_info[$sym]["type"] == 'TL')
				{
					// If theory and lab same, we must specify 2 teachers,
					// therefore we indicate which one is 'theory' and 'lab'
					echo "<b>Th�orie:</b> " . $entry3["teacherT"];
					echo "<br><b>Lab:</b> " . $entry3["teacherL"];
				}
				else if($type == 'C')
				{
					echo $entry3["teacherT"];
				}
				else if($type == 'L')
				{
					echo $entry3["teacherL"];
				}
				
				echo "</td></tr></table></tr>\n";
			}
		}
		echo "</table>";
	}
	
	echo '
		<script type="text/javascript">
			
			function openOCChecks(start, num)
			{
				// this is the index of the first checkbox for the open close form, the first form element has index 0
				ocCheckboxOffset= 2;
				
				for (i= 0; i < num; i++)
				{
					document.form[ocCheckboxOffset + start + i].checked= 1;
				}
			}
		</script>
	';
	
	echo <<<BENRULES
	</div>
	
	<h2>Terminer</h2>
	<div class="option_block">
		<p style="width:400px;"><b>Attention</b><br>Si vous continuez, les demandes que vous avez s�lectionn�es seront retir�es du syst�me.
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
	if (!isset($_POST["email"]))
	{
		error("Email manquant, v�rifier le lien utilis� ou refaire la demande par email");
	}
	else if (!isset($_POST["hash"]))
	{
		error("Hash manquant, v�rifier le lien utilis� ou refaire la demande par email");
	}
	
	if ($_POST["hash"] != getHash($_POST["email"], $_POST["hash"]))
	{
		error("Hash invalide, v�rifier le lien utilis� ou refaire la demande par email");
		#error("This will help debugging: " . htmlentities(urlencode(getHash($_GET["email"])), ENT_QUOTES));
	}
	
	function transform_string($string)
	{
		return "'". mysql_escape_string($string) . "'";
	}
	
	$groupList= array_keys(array_intersect($_POST, array("on")));
	
	if (count($groupList) < 1)
	{
		error("Vous n'avez s�lectionn� aucune demande");
	}
	
	$resultat= mysql_query("
		delete 
		from `notifications` 
		where `group` in (" . implode(", ", array_map("transform_string", $groupList)) . ") and `email`='" . mysql_escape_string($_POST["email"]) . "'
	") or die('Query failed: ' . mysql_error());
	
	if (!$resultat)
	{
		error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
	}
	
	echo <<<BENRULES
	<html>
	
	<head>
		<title>D�sinscription de la notification par email - �tape 2</title>
		<link rel="stylesheet" type="text/css" href="sopti.css">
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	</head>
	
	<body>
	
	<div id="header">
	
	<img src="aep.gif">
	
	<h1>D�sinscription de la notification par email</h1>
	
	<p class="step_notcurrent">�tape 0 - Code d'acc�s
	<p class="step_notcurrent">�tape 1 - Choisir les demandes � annuler
	<p class="step_current">�tape 2 - Confirmation
	
	</div>
	
	<p>
	
	<h2>Confirmation</h2>
	<div class="option_block">
		<p>
BENRULES;
	
	$nbrows= mysql_affected_rows();
	echo 'Vous avez �t� d�sinscrit de ' . htmlentities($nbrows, ENT_QUOTES) . ($nbrows < 2 ? ' demande.' : ' demandes.');
	
	echo <<<BENRULES
	<p>Vous pouvez maintenant fermer cette fen�tre et retourner � la 
	g�n�ration d'horaire ou vous inscrire pour �tre notifi� pour un	autre groupe.
	<form method="GET" action="javascript:window.close();">
		<input type="submit" value="Fermer cette fen�tre">
	</form>
	</div>
	
	</body>
	</html>
BENRULES;
}
// Step 0 b - send an email with link
else if (isset($_POST["email_request"]))
{
	global $CONFIG_VARS;
	
	if (!isset($_POST["email"]))
	{
		error("Vous n'avez pas fourni d'adresse email");
	}
	else if (!preg_match("/^([0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*@(([0-9a-zA-Z])+([-\w]*[0-9a-zA-Z])*\.)+[a-zA-Z]{2,11})$/", $_POST["email"]))
	{
		error("L'adresse email que vous avez fournie est invalide");
	}
	
	// Note: d�commenter ici pour avertir tout de suite la personne si elle n'a aucune requ�te en attente, �a pourrait �tre exploit� par une personne qui voudrait savoir si une autre � des requ�tes en attente
	# $resultat= mysql_query("
		# select 
			# count(*) 
		# from `notifications` 
		# where `notifications`.`email` = '" . mysql_escape_string($_POST["email"]) . "'
	# ") or die('Query failed: ' . mysql_error());
	
	# if (!$resultat || mysql_num_rows($resultat) < 1)
	# {
		# error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
	# }
	# else if (mysql_result($resultat, 0) < 1)
	# {
		# error("Vous n'avez aucune requ�te en attente.");
	# }
	
	// check the request limit
	$resultat= mysql_query("
		select 
			count(*) > " . ($CONFIG_VARS["emailer.maxHashReq"] - 1) . "
		from `demandes` 
		where 
			`email` = '" . mysql_escape_string($_POST["email"]) . (isset($CONFIG_VARS["emailer.maxHashReqTime"]) ? "' and 
			`date` > subtime(now(), '" . $CONFIG_VARS["emailer.maxHashReqTime"] . "')" : "") . " 
	") or die('[ ' . __LINE__ . ' ] Query failed: ' . mysql_error());
	
	if (!$resultat || mysql_num_rows($resultat) < 1)
	{
		error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
	}
	else if (mysql_result($resultat, 0))
	{
		error('Vous avez atteint votre maximum de requ�te de d�sinscription pour cette adresse, veuillez communiquer avec nous (<a href="mailto:' . $CONFIG_VARS["admin_email"] . '">' . $CONFIG_VARS["admin_email"] . '</a>) si la d�sinscription ne fonctionne pas.');
	}
	
	// check the ip limit
	// exclude ip's from inside the school's address range
	if (!ipcompare($_SERVER["REMOTE_ADDR"], "132.207.0.0", "255.255.0.0"))
	{
		$resultat= mysql_query("
			select 
				count(*) > " . ($CONFIG_VARS["emailer.maxHashIP"] - 1) . "
			from `demandes` 
			where 
				`ip` = '" . mysql_escape_string($_SERVER["REMOTE_ADDR"]) . (isset($CONFIG_VARS["emailer.maxHashIPTime"]) ? "' and 
				`date` > subtime(now(), '" . $CONFIG_VARS["emailer.maxHashIPTime"] . "')" : "") . " 
		") or die('[ ' . __LINE__ . ' ] Query failed: ' . mysql_error());
		
		if (!$resultat || mysql_num_rows($resultat) < 1)
		{
			error("Erreur de requ�te sur la base de donn�es: " . mysql_error());
		}
		else if (mysql_result($resultat, 0))
		{
			error('Vous avez atteint votre maximum de requ�te de d�sinscription pour ce poste, veuillez communiquer avec nous (<a href="mailto:' . $CONFIG_VARS["admin_email"] . '">' . $CONFIG_VARS["admin_email"] . '</a>) si la d�sinscription ne fonctionne pas.');
		}
	}
	
	// clear the old entries
	$resultat= mysql_query("
		delete
			from `demandes`
			where 
				1 = 1 " . 
				(isset($CONFIG_VARS["emailer.maxHashReqTime"]) ? " and 
				`date` < subtime(now(), '" . $CONFIG_VARS["emailer.maxHashReqTime"] . "')" : "") . 
				(isset($CONFIG_VARS["emailer.maxHashIPTime"]) ? " and 
				`date` < subtime(now(), '" . $CONFIG_VARS["emailer.maxHashIPTime"] . "')" : "") . " 
	") or die('[ ' . __LINE__ . '] Query failed: ' . mysql_error());
	
	// add a new entry
	$resultat= mysql_query("
		insert
			into `demandes`
			(`email`, `ip`, `date`)
			values 
			('" . mysql_escape_string($_POST["email"]) . "', '" . mysql_escape_string($_SERVER["REMOTE_ADDR"]) . "', now())
	") or die('Query failed: ' . mysql_error());
	
	if (!isset($_COOKIE["email_notification"]) or $_COOKIES["email_notification"] != $_POST["email"])
	{
		setcookie("email_notification", $_POST["email"], time()+60*60*24*31*9);
	}

	$message= "Bonjour,

Vous avez fait une demande pour vous d�sinscire de la notification par email sur le site du g�n�rateur d'horaires.
Pour ce faire vous n'avez qu'� suivre ce lien:
" . $CONFIG_VARS["baseURL"] . "/email_unsubscribe.php?email=" . urlencode($_POST["email"]) . "&hash=" . urlencode(getHash($_POST["email"])) . "
� partir de cette page vous pourrez choisir les notifications desquelles vous souhaitez vous d�sincrire.

Vous pouvez aussi aller v�rifier vos possibilit�s d'horaire � nouveau sur le site du g�n�rateur d'horaires:
" . $CONFIG_VARS["baseURL"] . "

Merci d'avoir utilis� le g�n�rateur d'horaires.
Vous pouvez nous faire part de vos commentaires � l'adresse 
" . $CONFIG_VARS["admin_email"] . "

-L'�quipe du g�n�rateur d'horaires

(Si vous avez re�u ce message sans en avoir fait la demande, pri�re de nous en faire part � l'adresse " . $CONFIG_VARS["admin_email"] . ")
";
	
	$message= wordwrap($message, 70);
	
	$headers= "From: G�n�rateur d'horaires de l'AEP <" . $CONFIG_VARS["admin_email"] . ">";
	
	if (!mail($_POST["email"], "D�sinscription de la notification par email", $message, $headers))
	{
		error("Le email n'a pu �tre envoy� avec succ�s");
	}
	
echo <<<BENRULES
	<html>
	
	<head>
		<title>D�sinscription de la notification par email - �tape 0</title>
		<link rel="stylesheet" type="text/css" href="sopti.css">
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	</head>
	
	<body>
	
	<div id="header">
	
	<img src="aep.gif">
	
	<h1>D�sinscription de la notification par email</h1>
	
	<p class="step_current">�tape 0 - Code d'acc�s
	<p class="step_notcurrent">�tape 1 - Choisir les demandes � annuler
	<p class="step_notcurrent">�tape 2 - Confirmation
	
	</div>
	
	<p>
	
	<h2>Confirmation</h2>
	<div class="option_block">
		<p>
BENRULES;
	
	echo 'Un email � �t� envoy� � votre adresse, ' . htmlentities($_POST["email"], ENT_QUOTES) . ', vous devriez le recevoir d\'ici quelques minutes.<br>
		Si jamais cela ne fonctionne pas, veuillez nous contacter � l\'addresse <a href="mailto:' . $CONFIG_VARS["admin_email"] . '">' . $CONFIG_VARS["admin_email"] . '</a>';
	
	echo <<<BENRULES
	</div>
	
	</body>
	</html>
BENRULES;
}
// Step 0 a - request to send an email
else
{
	echo <<<BENRULES
	<html>
	
	<head>
		<title>D�sinscription de la notification par email - �tape 0</title>
		<link rel="stylesheet" type="text/css" href="sopti.css">
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	</head>
	
	<body>
	
	<div id="header">
	
	<img src="aep.gif">
	
	<h1>D�sinscription de la notification par email</h1>
	
	<p class="step_current">�tape 0 - Code d'acc�s
	<p class="step_notcurrent">�tape 1 - Choisir les demandes � annuler
	<p class="step_notcurrent">�tape 2 - Confirmation
	
	</div>
	
	<p>
	
	<h2>Code d'acc�s</h2>
	<div class="option_block" style="width: 600px;">
	<p>
		Pour annuler les demandes que vous avez d�j� faites il vous faut utiliser un lien comportant un code
		d'acc�s sp�cifique � votre email.
		<el>
		<li>	Si vous avez d�j� re�u une notification, le lien vous � �t� fourni dans le email
		et vous pouvez l'utiliser.
		<li>Si vous n'avez pas encore re�u de notification ou si vous n'avez plus acc�s � un email de notification pr�c�dent
		le syst�me peut vous envoyer un email avec le lien � utilser. Une fois que vous aurez re�u ce email vous n'avez qu'�
		suivre le lien pour vous d�sinscrire des notifications qui ne vous sont plus utiles.<br><br>
		<form action="email_unsubscribe.php" method="POST">
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
			<input type="submit" value="Soumettre" name="email_request">
		</form>
		</el>
	</div>
	
	</body>
	</html>
BENRULES;
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
