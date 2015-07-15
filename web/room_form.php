<?php
  require_once('config.php');
  require_once("lib.php");
  require_once('lib/templates.php');
  read_config_file($SOPTI_CONFIG_FILE);
  ob_start();
?>

<?php begin_page('Horaire de salle'); ?>
<?php main_navbar(); ?>

<div class="container-fluid">
  <div class="row">
    <?php main_sidebar('horaire_local'); ?>

    <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
      <div class="row en-tete">
        <div class="col-md-12 large">
          <h1>Horaire de salle</h1>
        </div>
      </div>

      <div class="row">
        <div class="col-md-offset-2 col-md-8 large">
          <?php
            $dblink = connect_db();

            // Make the query
            $query = "SELECT periods.room AS room FROM periods INNER JOIN groups ON groups.unique=periods.group INNER JOIN courses_semester ON courses_semester.unique=groups.course_semester INNER JOIN courses ON courses.unique=courses_semester.course INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS['default_semester']."' GROUP BY room ORDER BY room";

            $result = mysql_query($query) or die('Query failed: ' . mysql_error());
            mysql_close($dblink);
          ?>

          <form action="room.php" method="get" style="margin-top: 20px;">
            <div class="form-group">
              <p>
                Sélectionner la salle dans la liste ci-dessous.
              </p>
              <select name="room" class="form-control" autofocus="autofocus">
                <?php
                  while ($row = mysql_fetch_row($result)) {
                    echo "\t\t<option>".$row[0] . "</option>\n";
                  }
                ?>
              </select>
            </div>

            <div class="form-group" style="text-align: center;">
              <button type="submit" class="btn btn-lg btn-primary">
                <i class="fa fa-arrow-right"></i>&nbsp;&nbsp;&nbsp;Voir
              </button>
            </div>

          </form>
        </div>
      </div>

    </div>
  </div>
</div>

<?php end_page(); ?>
