<?php
# ob_start();
require_once("lib.php");
?>

<!DOCTYPE html>
<html lang="en">
	<head>
		<?php include "_head.php"; ?>
		<title>Étape 1 | Générateur d'horaires de l'AEP | Polytechnique Montréal</title>
	</head>

	<body>

		<?php include "_main_navbar.php"; ?>

    <div class="container-fluid">
      <div class="row">
        <div class="col-sm-3 col-md-2 sidebar">
          <ul class="nav nav-sidebar">
            <li><a href="index.php">Accueil</a></li>
						<li class="active"><a href="make_form1.php">Générateur d'horaires</a></li>
            <li><a href="room_form.php">Horaire d'un local</a></li>
						<li><a href="teacher_form.php">Horaire d'un chargé de cours</a></li>
            <li><a href="email_unsubscribe.php">Désinscription<br />(notification automatique)</a></li>
						<li><a href="listcourses.php">Cours offerts</a></li>
          </ul>
        </div>

				<div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
				  <div class="row en-tete">
				    <div class="col-md-12 large">
				      <h1>Générateur d'horaires</h1>
				    </div>
				  </div>

				  <div class="row">

				    <div class="btn-group btn-group-justified">
				      <a href="#" class="btn btn-primary btn-lg active">1. Cours</a>
				      <a href="#" class="btn btn-primary btn-lg">2. Options</a>
				      <a href="#" class="btn btn-primary btn-lg">3. Horaires</a>

				    </div>
				  </div>

				  <div class="row">
				    <div class="col-md-offset-2 col-md-8 large">
							<form method="get" action="make_form2.php">
								<h2>Cours désirés</h2>

								<div class="form-group">
									<input type="text" name="courses" class="form-control" style="font-family: monospace;" autofocus="autofocus">
									<span class="help-block">
										Écrire les sigles des cours désirés, séparés par un espace. Ex: MTH1101 MTH1007 INF1005C.<br>
										<a href="listcourses.php">Voir la liste des cours offerts</a>
									</span>
								</div>

								<div class="form-group" style="text-align: center;">
									<button type="submit" class="btn btn-lg btn-primary">
										<i class="fa fa-arrow-right"></i>&nbsp;&nbsp;&nbsp;Continuer
									</button>
								</div>

							</form>
				    </div>
				  </div>

				</div>
      </div>
    </div>

	</body>
</html>
