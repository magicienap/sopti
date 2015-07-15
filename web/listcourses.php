<?php
  require_once('config.php');
  require_once('lib.php');
  require_once('lib/templates.php');
  ob_start();
?>

<?php begin_page('Cours offerts'); ?>
<?php main_navbar(); ?>

<div class="container-fluid">
  <div class="row">
    <?php main_sidebar('cours_offerts'); ?>

    <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
      <div class="row en-tete">
        <div class="col-md-12 large">
          <h1>Liste des cours offerts</h1>
        </div>
      </div>

      <div class="row">
        <div class="col-md-12 large">
          <pre style="margin-top: 20px;">
<?php
  $dblink = connect_db();

  // Make the query
  $query = "SELECT courses.symbol AS symbol,courses.title AS title FROM courses INNER JOIN courses_semester ON courses_semester.course=courses.unique INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='".$CONFIG_VARS["default_semester"]."' ORDER BY courses.symbol";

  $result = mysql_query($query) or die('Query failed: ' . mysql_error());
  mysql_close($dblink);

  while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
    echo $row["symbol"] . " " . $row["title"] . "\n";
  }
?>
          </pre>
        </div>
      </div>

    </div>
  </div>
</div>

<?php end_page(); ?>
