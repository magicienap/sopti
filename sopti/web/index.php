<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">

<html>

<head>
<title>G�n�rateur d'horaires</title>
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

<!-- Ent�te AEP -->

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
				<a class=menulink href='http://www.aep.polymtl.ca/'>Aller au site web de l'AEP</a></td>
			</tr>

			<tr>
                <td class=slogan colspan=2 align=right valign=bottom height=100%>
					site web officiel de l'Association des �tudiants de Polytechnique
				</td>
			</tr>
		</table>
        </td></tr>
<tr><td>

<!-- Fin en-t�te AEP -->



<h1>G�n�rateur d'horaires</h1>
<center><div class="sopti_actions_rect">
<p><font size="-1">Session en cours:</font><br><strong>automne 2005</strong>
<p><a href="make_form1.php" target="_blank"><font size="+3">D�marrer</font></a><br><font size="-1">(g�n�rer des horaires)</font></p>
<p><a href="listcourses.php" target="_blank">Liste des cours offerts</a>
<p><a href="email_unsubscribe.php" target="_blank">Se d�sincrire de la notification par email</a>
<p><font size="-2">Derni&egrave;re mise &agrave; jour des donn&eacute;es:<br>
<?php
require_once('config.php');
$info = stat($SOPTI_LASTUPDATE);
$unix_modif = $info[9];
$string_modif = date("r", $unix_modif);
print($string_modif);
?></font>
</div></center>

<h2>Nouvelles</h2>
<div class="sopti_nouvelles_rect">
<dl>
<dt><div style="background-color:#f9b771; margin-right: 15px; border-width: 2px; border-style: outset;">21 ao�t 2005</div>
<dd><b>Am�liorations au g�n�rateur</b><br>
<ul>
	<li>Affichage des noms de charg�s de cours et de lab
	<li>Syst�me de notification par email lorsque des places deviennent disponibles dans une section (par Benjamin Poirier, bac g�nie info)
	<li>Affichage des r�sultats par pages
</ul>

	<p>Gr�ce � une excellente collaboration avec le BAA, le g�n�rateur d'horaires est maintenant synchronis� avec la base de donn�es officielle de l'�cole � toutes les 15 minutes.

	<p>�galement, gr�ce � Jean-Fran�ois L�vesque, VP services de l'AEP, le g�n�rateur est maintenant h�berg� sur un serveur tr�s rapide.

	<p>Durant la session d'hiver 2005, plus de 2000 personnes ont utilis� le g�n�rateur, pour un total de plus de 20 000 requ�tes. Plusieurs nous ont envoy� des suggestions d'am�lioration, et toutes ces suggestions ont �t� ajout�es au programme. Continuez � nous envoyer vos id�es, elle ont beaucoup d'influence sur le d�veloppement du programme.


<dt><div style="background-color:#f9b771; margin-right: 15px; border-width: 2px; border-style: outset;">9 ao�t 2005</div>
<dd>Le g�n�rateur d'horaires g�n�re des horaires pour l'automne 2005 depuis ce matin.<br><br>
	N'oubliez pas que plusieurs cours ont chang� � cause de la r�organisation des programmes.
</dl>
</div>

<h2>Qu'est-ce que c'est?</h2>

<p>Le g�n�rateur d'horaires de l'AEP est une application web qui vous cr�e un horaire de cours en fonction de vos besoins.
 
<p>Pour l'utiliser, vous entrez les cours que vous d�sirez suivre, et le syst�me vous affiche la liste de tous les horaires qui pourraient vous convenir. Vous n'avez qu'� en choisir un, puis � appliquer les modifications � votre dossier �tudiant. Cliquez sur D�marrer pour commencer!

<p>Vous avez aussi acc�s � des fonctions plus avanc�es. Si vous souhaitez minimiser les "trous" dans votre horaire, maximiser le sommeil le matin, ou avoir certaines p�riodes de votre choix libres, vous n'avez qu'� s�lectionner l'option correspondante. Les horaires seront alors r�ordonn�s et filtr�s pour correspondre � votre pr�f�rence. Vous trouverez encore plus d'options pour personnaliser votre horaire dans le formulaire.

<p>De plus, le syst�me vous offre:
<ul>
	<li>Des horaires bas�s sur les donn�es les plus r�centes du BAA
	<li>Les cours du bacc, cycles sup�rieurs, et certificats
	<li>Les cours de jour, de soir et de fin de semaine
	<li>La possibilit� de g�n�rer des horaires avec conflits
	<li>Une disponibilit� du syst�me durant toute la session
</ul>

<h2>Contact</h2>
<p>Ce programme est entretenu par Pierre-Marc Fournier, �tudiant 1er cycle, g�nie informatique. Commentaires, suggestions, rapports de bug bienvenus.
<p>Adresse: pierre-marc.fournier � polymtl.ca (remplacer � par un @)


<h2>Liens</h2>
<p> D'autres sites utiles concernant les horaires dans la communaut� de Poly.
<dl>
	<dt><a href="https://www4.polymtl.ca/poly/poly.html">Dossier �tudiant</a>
		<dd>Pour enregistrer vos modifications d'horaires
	<dt><a href="http://www.gegi.polymtl.ca/info/lanctot/java/ccref.htm">CCREF</a>
		<dd>Pour une autre opinion, un autre g�n�rateur d'horaires
	<dt><a href="http://www2.polymtl.ca/infode/">Bureau des affaires acad�miques</a>
		<dd>Calendriers, choix de cours, informations diverses
	<dt><a href="http://www.cgmi.polymtl.ca/fabhor.asp">Fabrication d'horaires au CGM</a>
		<dd>Site permettant d'afficher votre horaire dans un beau format
</dl>

<h2>D�veloppeurs</h2>
<p>Ce programme est <a href="http://www.opensource.org/docs/definition.php">open source</a>. Le code est disponible <a href="http://www.info.polymtl.ca/~pifoua/sopti/src/">ici</a> sous la licence <a href="http://www.fsf.org/licenses/gpl.txt">GPL</a>.
<p>Vous aimeriez participer &agrave; son d&eacute;veloppement? Contactez Pierre-Marc Fournier &agrave; l'adresse ci-dessus.

<p>&nbsp;
</body>
</html>
