<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">

<html>

<head>
<title>Générateur d'horaires</title>
<!--
<base href="http://www.aep.polymtl.ca/">
-->
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel=stylesheet href="http://www.aep.polymtl.ca/css/global_css.css" type=text/css>

<style type="text/css">
body {
	font-family: georgia;
}

div.sopti_actions_rect {
	width: 300;
	border-style:		double;
	border-color:       	#C7C7C7;
	background-color:   	#BDCEE7;
}

div.sopti_actions_rect p {
	margin-left: 0px;
}

div.sopti_nouvelles_rect {
	margin-left: 30px;
	border-style:		double;
	border-color:       	#C7C7C7;
	background-color:   	#F99C39;
}

dl dt {
	font-weight: bold;
}

dl dd {
	margin-bottom: 10px;
}

a {
	color: #000000;
}

p, dl {
	margin-left: 30px;
}

</style>
</head>

<body>

<!-- Entête AEP -->

<table class=main align=center cellspacing=0>
<tr><td>		<table class=header>
			<tr height=115>
				<td width=48%>
				</td>
				<td align=right valign=bottom>
					<table width=80% cellspacing=4 cellpadding=0>
						<tr align=right>
							<td><img src="http://www.aep.polymtl.ca/images/image1.jpg"></td>
							<td><img src="http://www.aep.polymtl.ca/images/image2.jpg"></td>
							<td><img src="http://www.aep.polymtl.ca/images/image3.jpg"></td>
							<td><img src="http://www.aep.polymtl.ca/images/image4.jpg"></td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td>
				</td>
				<td class=menulink align=right valign=top>
                    <a class=menulinkselected href='http://www.aep.polymtl.ca/accueil.php'>Accueil</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/calendrier.php'>Calendrier</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/exec.php'>Exec</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/ca.php'>C.A.</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/dossier.php'>Dossiers en cours</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/ressources.php'>Ressources</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/contacteznous.php'>Contactez-nous</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/faq.php'>F.A.Q.</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/structure.php'>Infos</a><a class=unmenulink> | <a><a class=menulink href='http://www.aep.polymtl.ca/archive.php'>Archives</a><a class=unmenulink> | <a>				</td>
			</tr>

			<tr>
                <td class=slogan colspan=2 align=right valign=bottom height=100%>
					site web officiel de l'Association des étudiants de Polytechnique
				</td>
			</tr>
		</table>
        </td></tr>
<tr><td>

<!-- Fin en-tête AEP -->



<h1>Générateur d'horaires</h1>
<center><div class="sopti_actions_rect">
<p><font size="-1">Session en cours:</font><br><strong>hiver 2005</strong>
<p><a href="make_form1.php" target="_blank"><font size="+3">Démarrer</font></a><br><font size="-1">(générer des horaires)</font></p>
<p><a href="listcourses.php" target="_blank">Liste des cours offerts</a>
<p><font size="-2">Derni&egrave;re mise &agrave; jour des donn&eacute;es:<br>
<?php
require_once('config.php');
$info = stat($SOPTI_COURSEFILE);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);
print($string_modif);
?></font>
</div></center>

<h2>Nouvelles</h2>
<div class="sopti_nouvelles_rect">
<dl>
<dt>17 d&eacute;cembre 2004
<dd>Quelques am&eacute;liorations
	<ul>
		<li>Support pour l'ouverture et fermeture de sections
		<li>Optimisations de vitesse
		<li>Correction d'un bug avec les conflits (merci &agrave; J&eacute;r&ocirc;me Blais-Morin)
	</ul>
	Je remercie tous ceux et celles qui nous ont &eacute;crit pour donner leurs commentaires! Plusieurs
	personnes nous ont soulign&eacute; que le programme est un peu lent. Cette version devrait &ecirc;tre
	 plus rapide, et nous essaierons d'augmenter encore la vitesse dans l'avenir.<br>- Pierre-Marc

</dl>
</div>

<h2>Qu'est-ce que c'est?</h2>

<p>Le générateur d'horaires de l'AEP est une application web qui vous crée un horaire de cours en fonction de vos besoins.
 
<p>Pour l'utiliser, vous entrez les cours que vous désirez suivre, et le système vous affiche la liste de tous les horaires qui pourraient vous convenir. Vous n'avez qu'à en choisir un, puis à appliquer les modifications à votre dossier étudiant. Cliquez sur Démarrer pour commencer!

<p>Vous avez aussi accès à des fonctions plus avancées. Si vous souhaitez minimiser les "trous" dans votre horaire, maximiser le sommeil le matin, ou avoir certaines périodes de votre choix libres, vous n'avez qu'à sélectionner l'option correspondante. Les horaires seront alors réordonnés et filtrés pour correspondre à votre préférence. Vous trouverez encore plus d'options pour personnaliser votre horaire dans le formulaire.

<p>De plus, le système vous offre:
<ul>
	<li>Des horaires basés sur les données les plus récentes du BAA
	<li>Les cours du bacc, cycles supérieurs, et certificats
	<li>Les cours de jour, de soir et de fin de semaine
	<li>La possibilité de générer des horaires avec conflits
	<li>Une disponibilité du système durant toute la session
</ul>

<h2>Contact</h2>
<p>Ce programme est entretenu par Pierre-Marc Fournier, étudiant 1er cycle, génie informatique. Commentaires, suggestions, rapports de bug bienvenus.
<p>Adresse: pierre-marc.fournier À polymtl.ca (remplacer À par un @)


<h2>Liens</h2>
<p> D'autres sites utiles concernant les horaires dans la communauté de Poly.
<dl>
	<dt><a href="https://www4.polymtl.ca/poly/poly.html">Dossier étudiant</a>
		<dd>Pour enregistrer vos modifications d'horaires
	<dt><a href="http://www.gegi.polymtl.ca/info/lanctot/java/ccref.htm">CCREF</a>
		<dd>Pour une autre opinion, un autre générateur d'horaires
	<dt><a href="http://www2.polymtl.ca/infode/">Bureau des affaires académiques</a>
		<dd>Calendriers, choix de cours, informations diverses
	<dt><a href="http://www.cgmi.polymtl.ca/fabhor.asp">Fabrication d'horaires au CGM</a>
		<dd>Site permettant d'afficher votre horaire dans un beau format
</dl>

<h2>Développeurs</h2>
<p>Ce programme est <a href="http://www.opensource.org/docs/definition.php">open source</a>. Le code est disponible <a href="http://www.info.polymtl.ca/~pifoua/sopti/src/">ici</a> sous la licence <a href="http://www.fsf.org/licenses/gpl.txt">GPL</a>.
<p>Vous aimeriez participer &agrave; son d&eacute;veloppement? Contactez Pierre-Marc Fournier &agrave; l'adresse ci-dessus.

<p>&nbsp;
</body>
</html>
