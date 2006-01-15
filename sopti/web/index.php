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
  <title>G�n�rateur d'horaires</title>
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


<div style="width: 125px; padding: 0px; margin: 20px auto;"><img src="genhor_sm.png" alt="G�n�rateur d'horaires" /></div>

<div style="width: 200px; margin: 20px auto; text-align: center; color: white; background-color: #555555; clear: left;"><p style="font-size: 9px; margin-bottom: 0px;">Session en cours</p><p style="font-size: 20px; margin: 0px;"><?php echo $cur_sem_pretty; ?></p></div>

<div style="font-size:13px; font-family: sans;text-align: center; border: 0px outset blue; margin: 20px auto; padding: 5px; font-weight: bold;">
	<div style="font-size: 16px; margin: 1px;"><img src="dentwheel.png" alt="" /> <a href="make_form1.php"> G�n�rer des horaires</a></div>
	<div style="margin: 1px;"><img src="list.png" alt="" /> <a href="listcourses.php"> Liste des cours offerts</a></div>
	<div style="margin: 1px;"><img src="redx.png" alt="" /> <a href="email_unsubscribe.php"> Se d�sinscrire de la notification automatique</a></div>
</div>


<div class="newsdate">6 d�cembre 2005</div>

<div class="newscontents">
	<b>Donn�es hiver 2006 disponibles</b><br />

	<p>Le G�n�rateur cr�e maintenant des horaires pour la session d'hiver 2006. Les noms des professeurs et charg�s de cours ne sont pas encore disponibles.</p>

</div>


<!--
<p style="border-bottom: 1px solid black; width: 472px; margin: 3px auto; padding: 0px; text-align: center; background-color: #8CABFF; font-weight: bold;">G�n�rer des horaires</p>

<p style="text-align: center; font-size: 12px; width:450px; margin: 0px auto; border: 1px solid black; padding: 10px;">
	<em>�crire les sigles des cours d�sir�s, s�par�s par un espace.</em><br />
	<span style="font-size: 10px;">Ex: ING1040 ING1035 ING1010</span><br />
	<textarea cols="30" rows="2"></textarea><br />
	<button>Continuer...</button>
</p>
-->

<div class="parags">

<h2>Qu'est-ce que c'est?</h2>

<p>Le g�n�rateur d'horaires de l'AEP est une application web qui vous cr�e un horaire de cours en fonction de vos besoins.</p>
 
<p>Pour l'utiliser, vous entrez les cours que vous d�sirez suivre, et le syst�me vous affiche la liste de tous les horaires qui pourraient vous convenir. Vous n'avez qu'� en choisir un, puis � appliquer les modifications � votre dossier �tudiant. Cliquez sur G�n�rer des horaires ci-dessus pour commencer!</p>

<p>Vous avez aussi acc�s � des fonctions plus avanc�es. Si vous souhaitez minimiser les "trous" dans votre horaire, maximiser le sommeil le matin, ou avoir certaines p�riodes de votre choix libres, vous n'avez qu'� s�lectionner l'option correspondante. Les horaires seront alors r�ordonn�s et filtr�s pour correspondre � votre pr�f�rence. Vous trouverez encore plus d'options pour personnaliser votre horaire dans le formulaire.</p>

<p>De plus, le syst�me vous offre:</p>
<ul>
	<li>Des horaires bas�s sur les donn�es les plus r�centes du BAA (synchronisation aux 15 minutes)</li>
	<li>Les cours du bacc, cycles sup�rieurs, et certificats</li>
	<li>Les cours de jour, de soir et de fin de semaine</li>
	<li>Un syst�me qui vous avertit par courriel lorsque des places deviennent disponibles dans les sections qui vous int�ressent</li>
	<li>La possibilit� de g�n�rer des horaires avec conflits</li>
	<li>Une disponibilit� du syst�me durant toute la session</li>
</ul>

<h2>Auteurs</h2>
<p><b>Pierre-Marc Fournier</b>, �tudiant 1er cycle, g�nie informatique</p>
<div class="indent1">Directeur du projet</div>
<div class="indent1">Adresse: pierre-marc.fournier � polymtl.ca (remplacer � par un @)</div>
<p><b>Benjamin Poirier</b>, �tudiant 1er cycle, g�nie informatique,</p>
<div class="indent1">Syst�me d'avertissement par courriel de places disponibles, documentation du code et autres</div>

<p><b>Jean-Fran�ois L�vesque</b>, VP services de l'AEP</p>
<div class="indent1">A fourni des ressources mat�rielles et administratives essentielles au fonctionnement du syst�me</div>


<h2>Liens</h2>
<p> D'autres sites utiles concernant les horaires dans la communaut� de Poly.</p>
<dl>
	<dt><a href="https://www4.polymtl.ca/poly/poly.html">Dossier �tudiant</a></dt>
		<dd>Pour enregistrer vos modifications d'horaires</dd>
	<dt><a href="http://www.gegi.polymtl.ca/info/lanctot/java/ccref.htm">CCREF</a></dt>
		<dd>Pour une autre opinion, un autre g�n�rateur d'horaires</dd>
	<dt><a href="http://www.polymtl.ca/baa/">Bureau des affaires acad�miques</a></dt>
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
