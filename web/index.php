<?php
	require_once('lib.php');
	ob_start();

	$dblink = connect_db();

	$query = "SELECT semesters.pretty_name from semesters where semesters.code='".$CONFIG_VARS["default_semester"]."'";
	
	$result = mysql_query($query) or die('Query failed: ' . mysql_error());
	if(!$result) {
		admin_error(mysql_error());
	}
	if(mysql_num_rows($result) == 0) {
		admin_error("Did not find pretty name for semester code \"" . $CONFIG_VARS["default_semester"] . "\" in database");
	}
	$row = mysql_fetch_row($result);

	$cur_sem_pretty = $row[0];

	mysql_close($dblink);
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>

<head>
  <title>Générateur d'horaires</title>
  <link rel="stylesheet" href="aep.css" type="text/css" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />

  <style type="text/css">
  body {
  	font-size: 14px;
  }
  
  p.generateur_title {
	 font-size: 30px;
	 text-align: center;
	 font-family: sans;
	 font-weight: bold;
	 font-variant: small-caps;
	 margin-bottom: 20px;
	 padding-bottom: 0px;
	 border-bottom: 0px solid;
	 text-decoration: none;
  }
  
  button {
  	font-weight: bold;
  }
  
  div.parags {
  	margin: 50px;
  }
  
  div.parags p, div.parags dl {
	margin-left: 30px;
  }
  
  div.indent1 {
  	margin-left: 60px;
  }
  
  table.main {
  	padding: 0px;
  }
  
  table.main td {
  	margin: 0px;
  }
  
  div.newsdate {
  	border-bottom: 1px solid black;
	width: 472px;
	margin: 3px auto;
	padding: 0px;
	text-align: center;
	background-color: #F9B771;
	font-weight: bold;
  }
  
  div.newscontents {
  	text-align: justify;
	font-size: 12px;
	width:450px;
	margin: 0px auto;
	border: 1px solid black;
	padding: 10px;
  }

  a img {
  	border: 0px;
  }
  </style>
  
</head>

<body>


<div style="width: 125px; padding: 0px; margin: 20px auto;"><img src="genhor_sm.png" alt="Générateur d'horaires" /></div>

<div style="width: 200px; margin: 20px auto; text-align: center; color: white; background-color: #555555; clear: left;"><p style="font-size: 9px; margin-bottom: 0px;">Session en cours</p><p style="font-size: 20px; margin: 0px;"><?php echo $cur_sem_pretty; ?></p></div>

<div style="font-size:13px; font-family: sans;text-align: center; border: 0px outset blue; margin: 20px auto; padding: 5px; font-weight: bold;">
	<div style="font-size: 16px; margin: 1px;"><img src="dentwheel.png" alt="" /> <a href="make_form1.php"> Générer des horaires</a></div>
	<div style="margin: 1px;"><img src="list.png" alt="" /> <a href="listcourses.php"> Liste des cours offerts</a></div>
	<div style="margin: 1px;"><img src="redx.png" alt="" /> <a href="email_unsubscribe.php"> Se désinscrire de la notification automatique</a></div>
	<div style="margin: 1px;"><a href="room_form.php"> Horaire d'un local</a></div>
	<div style="margin: 1px;"><a href="teacher_form.php"> Horaire d'un chargé de cours</a></div>
</div>


<div class="newsdate">6 décembre 2005</div>

<div class="newscontents">
	<b>Données hiver 2006 disponibles</b><br />

	<p>Le Générateur crée maintenant des horaires pour la session d'hiver 2006. Les noms des professeurs et chargés de cours ne sont pas encore disponibles.</p>

</div>


<!--
<p style="border-bottom: 1px solid black; width: 472px; margin: 3px auto; padding: 0px; text-align: center; background-color: #8CABFF; font-weight: bold;">Générer des horaires</p>

<p style="text-align: center; font-size: 12px; width:450px; margin: 0px auto; border: 1px solid black; padding: 10px;">
	<em>Écrire les sigles des cours désirés, séparés par un espace.</em><br />
	<span style="font-size: 10px;">Ex: ING1040 ING1035 ING1010</span><br />
	<textarea cols="30" rows="2"></textarea><br />
	<button>Continuer...</button>
</p>
-->

<div class="parags">

<h2>Qu'est-ce que c'est?</h2>

<p>Le générateur d'horaires de l'AEP est une application web qui vous crée un horaire de cours en fonction de vos besoins.</p>
 
<p>Pour l'utiliser, vous entrez les cours que vous désirez suivre, et le système vous affiche la liste de tous les horaires qui pourraient vous convenir. Vous n'avez qu'à en choisir un, puis à appliquer les modifications à votre dossier étudiant. Cliquez sur Générer des horaires ci-dessus pour commencer!</p>

<p>Vous avez aussi accès à des fonctions plus avancées. Si vous souhaitez minimiser les "trous" dans votre horaire, maximiser le sommeil le matin, ou avoir certaines périodes de votre choix libres, vous n'avez qu'à sélectionner l'option correspondante. Les horaires seront alors réordonnés et filtrés pour correspondre à votre préférence. Vous trouverez encore plus d'options pour personnaliser votre horaire dans le formulaire.</p>

<p>De plus, le système vous offre:</p>
<ul>
	<li>Des horaires basés sur les données les plus récentes du BAA (synchronisation aux 15 minutes)</li>
	<li>Les cours du bacc, cycles supérieurs, et certificats</li>
	<li>Les cours de jour, de soir et de fin de semaine</li>
	<li>Un système qui vous avertit par courriel lorsque des places deviennent disponibles dans les sections qui vous intéressent</li>
	<li>La possibilité de générer des horaires avec conflits</li>
	<li>Une disponibilité du système durant toute la session</li>
</ul>

<h2>Auteurs</h2>
<p><b>Pierre-Marc Fournier</b>, étudiant 1er cycle, génie informatique</p>
<div class="indent1">Directeur du projet</div>
<div class="indent1">Adresse: pierre-marc.fournier À polymtl.ca (remplacer À par un @)</div>
<p><b>Benjamin Poirier</b>, étudiant 1er cycle, génie informatique,</p>
<div class="indent1">Système d'avertissement par courriel de places disponibles, documentation du code et autres</div>

<p><b>Jean-François Lévesque</b>, VP services de l'AEP</p>
<div class="indent1">A fourni des ressources matérielles et administratives essentielles au fonctionnement du système</div>


<h2>Liens</h2>
<p> D'autres sites utiles concernant les horaires dans la communauté de Poly.</p>
<dl>
	<dt><a href="https://www4.polymtl.ca/poly/poly.html">Dossier étudiant</a></dt>
		<dd>Pour enregistrer vos modifications d'horaires</dd>
	<dt><a href="http://www.gegi.polymtl.ca/info/lanctot/java/ccref.htm">CCREF</a></dt>
		<dd>Pour une autre opinion, un autre générateur d'horaires</dd>
	<dt><a href="http://www.polymtl.ca/baa/">Bureau des affaires académiques</a></dt>
		<dd>Calendriers, choix de cours, informations diverses</dd>
	<dt><a href="http://www.cgmi.polymtl.ca/fabhor.asp">Fabrication d'horaires au CGM</a></dt>
		<dd>Site permettant d'afficher votre horaire dans un beau format</dd>
</dl>

<h2>Code source</h2>
<p>Ce programme est <a href="http://www.opensource.org/docs/definition.php">open source</a>. Le code est disponible <a href="http://www.horaires.aep.polymtl.ca/src/">ici</a> sous la licence <a href="http://www.fsf.org/licenses/gpl.txt">GPL</a>.</p>

</div>

<p>
    <a href="http://validator.w3.org/check?uri=referer"><img
       src="http://www.w3.org/Icons/valid-xhtml10"
       alt="Valid XHTML 1.0 Strict" height="31" width="88" /></a>

    <a href="http://jigsaw.w3.org/css-validator/check/referer">
   <img style="border:0;width:88px;height:31px"
       src="http://jigsaw.w3.org/css-validator/images/vcss" 
       alt="Valid CSS!" />
</a>
</p>
</body>
</html>
