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

<!DOCTYPE html>
<html lang="en">
	<head>
		<?php include "_head.php"; ?>
		<title>Générateur d'horaires de l'AEP | Polytechnique Montréal</title>
	</head>

	<body>

		<?php include "_main_navbar.php"; ?>

    <div class="container-fluid">
      <div class="row">
        <div class="col-sm-3 col-md-2 sidebar">
          <ul class="nav nav-sidebar">
            <li class="active"><a href="index.php">Accueil</a></li>
						<li><a href="make_form1.php">Générateur d'horaires</a></li>
            <li><a href="room_form.php">Horaire d'un local</a></li>
						<li><a href="teacher_form.php">Horaire d'un chargé de cours</a></li>
            <li><a href="email_unsubscribe.php">Désinscription<br />(notification automatique)</a></li>
						<li><a href="listcourses.php">Cours offerts</a></li>
          </ul>
        </div>

        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <div class="row en-tete">
            <div class="col-md-12 large">
              <a href="make_form1.php" class="btn btn-primary btn-lg" style="margin-top: 15px; float: right;">Commencer</a>

							<h1>Générateur d'horaires de l'AEP</h1>
            </div>
          </div>

          <div class="row" style="text-align: justify;">
            <div class="col-md-offset-2 col-md-8 large">

							<h2>Qu'est-ce que c'est?</h2>

							<p>
								Le générateur d'horaires de l'AEP est une application web qui vous crée un horaire de cours en fonction de vos besoins.
							</p>

							<p>
								Pour l'utiliser, vous entrez les cours que vous désirez suivre, et le système vous affiche la liste de tous les horaires qui pourraient vous convenir. Vous n'avez qu'à en choisir un, puis à appliquer les modifications à votre dossier étudiant. Cliquez sur «&nbsp;Générateur d'horaires&nbsp;» ci-contre pour commencer!
							</p>

							<p>
								Vous avez aussi accès à des fonctions plus avancées. Si vous souhaitez minimiser les «&nbsp;trous&nbsp;» dans votre horaire, maximiser le sommeil le matin, ou avoir certaines périodes de votre choix libres, vous n'avez qu'à sélectionner l'option correspondante. Les horaires seront alors réordonnés et filtrés pour correspondre à votre préférence. Vous trouverez encore plus d'options pour personnaliser votre horaire dans le formulaire.
							</p>

							<p>
								De plus, le système vous offre:
							</p>

							<ul>
								<li>Des horaires basés sur les données les plus récentes du BAA (synchronisation aux 15 minutes)</li>
								<li>Les cours du baccalauréat, des cycles supérieurs et des certificats</li>
								<li>Les cours de jour, de soir et de fin de semaine</li>
								<li>Un système qui vous avertit par courriel lorsque des places deviennent disponibles dans les sections qui vous intéressent</li>
								<li>La possibilité de générer des horaires avec conflits</li>
								<li>Une disponibilité du système durant toute la session</li>
							</ul>

							<h2>Auteurs</h2>

							<dl>
								<dt>Pierre-Marc Fournier <span style="font-weight: normal;">étudiant 1er cycle, génie informatique 2003-2007</span></dt>
								<dd>
									Directeur du projet<br />
									Adresse: pierre-marc.fournier À polymtl.ca (remplacer À par un @)
								</dd>

								<dt>Benjamin Poirier <span style="font-weight: normal;">ancien étudiant 1er cycle, génie informatique</span></dt>
								<dd>Système d'avertissement par courriel de places disponibles, documentation du code et autres</dd>

								<dt>Jean-François Lévesque <span style="font-weight: normal;">VP services de l'AEP 2004-2006</span></dt>
								<dd>A fourni des ressources matérielles et administratives essentielles au démarrage du projet</dd>
							</dl>

							<h2>Liens</h2>

							<p>
								D'autres sites utiles concernant les horaires dans la communauté de Poly.
							</p>

							<dl>
								<dt><a href="https://dossieretudiant.polymtl.ca/WebEtudiant7/poly.html">Dossier étudiant</a></dt>
								<dd>Pour enregistrer vos modifications d'horaires</dd>

								<dt><a href="http://www.polymtl.ca/registrariat/">Registrariat</a></dt>
								<dd>Calendriers, choix de cours, informations diverses</dd>
							</dl>

							<h2>Code source</h2>

							<p>
								Ce programme est <a href="http://www.opensource.org/docs/definition.php">open source</a>. Le code est disponible <a href="http://www.horaires.aep.polymtl.ca/src/">ici</a> sous la licence <a href="http://www.fsf.org/licenses/gpl.txt">GPL</a>.
							</p>

			       	<iframe src="http://www.facebook.com/plugins/like.php?locale=fr_CA&href=http://horaires.aep.polymtl.ca"
			        				scrolling="no" frameborder="0"
			        				style="border:none; width:450px; height: 80px;">
							</iframe>

            </div>
          </div>

        </div>
      </div>
    </div>

	</body>
</html>
