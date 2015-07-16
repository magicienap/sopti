<?php
	require_once("lib.php");
	require_once('lib/templates.php');
  ob_start();

	$room = $_GET['room'];
?>

<?php begin_page('Horaire de salle'); ?>
<?php main_navbar(); ?>

<div class="container-fluid">
  <div class="row">
    <?php main_sidebar('horaire_local'); ?>

    <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
      <div class="row en-tete">
        <div class="col-md-12 large">
          <h1>Horaire de la salle <?php echo htmlspecialchars($room, ENT_QUOTES); ?></h1>
					<p><?php echo date("Y/m/d H:i:s"); ?></p>
        </div>
      </div>

      <div class="row">
        <div class="col-md-offset-2 col-md-8 large">
					<div style="padding-top: 20px; padding-bottom: 20px;">
						<?php print_room_schedule($room); ?>
					</div>

					<div class="panel panel-default">
						<div class="panel-heading" style="background-color: #C00; border-color: #C00;">
							<h2 class="panel-title"><i class="fa fa-exclamation-triangle"></i>&nbsp;&nbsp;&nbsp;Avertissement</h2>
						</div>

						<div class="panel-body">
							<p>
								Cet horaire est construit à partir de l'horaire général de l'École Polytechnique. Il ne contient que les cours officiellement données à l'École Polytechnique. D'autres activités peuvent occuper un local. Pour connaître l'horaire complet d'un local ou le réserver, communiquer avec le service responsable du local en question.
							</p>
						</div>
					</div>

				</div>
      </div>

    </div>
  </div>
</div>

<?php end_page(); ?>
