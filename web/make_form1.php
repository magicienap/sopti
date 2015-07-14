<?php
  require_once("lib.php");
  require_once('lib/templates.php');
?>

<?php begin_page('Étape 1'); ?>
<?php main_navbar(); ?>

<div class="container-fluid">
  <div class="row">
    <?php main_sidebar('generateur_horaires'); ?>

    <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
      <div class="row en-tete">
        <div class="col-md-12 large">
          <h1>Générateur d'horaires</h1>
        </div>
      </div>

      <?php generator_steps(1); ?>

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

<?php end_page(); ?>
