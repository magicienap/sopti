<?php
  require_once('config.php');
  require_once('lib.php');
  require_once('lib/templates.php');
  ob_start();

function print_open_close_form($courses) {
  global $CONFIG_VARS;

  $dblink = connect_db();

  // Make the query
  $query_begin = "SELECT courses.symbol AS sym,courses.title AS title,courses_semester.course_type AS coursetype,groups.name AS groupname,groups.theory_or_lab AS grouptl,groups.teacher AS teacher,groups.closed AS closed, groups.unique AS grpunique FROM courses INNER JOIN courses_semester ON courses_semester.course=courses.unique INNER JOIN groups ON groups.course_semester=courses_semester.unique INNER JOIN semesters ON courses_semester.semester=semesters.unique WHERE semesters.code='".$CONFIG_VARS["default_semester"]."' AND (";
  $query_end   = ") ORDER BY courses.symbol ASC,groups.name ASC,groups.theory_or_lab ASC";
  $query = $query_begin;
  $first = true;
  foreach($courses as $course) {
    if($first == true) {
      $query = $query . " courses.symbol='" . mysql_escape_string($course) . "'";
      $first = false;
    }
    else {
      $query = $query . " OR courses.symbol='" . mysql_escape_string($course) . "'";
    }
  }
  $query = $query . $query_end;

  $result = mysql_query($query) or die('Query failed: ' . mysql_error());
  mysql_close($dblink);

  while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
    $group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["teacher"] = $row['teacher'];
    $group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["closed"] = $row['closed'];
    $group_info[$row["sym"]][$row["grouptl"]][$row["groupname"]]["unique"] = $row['grpunique'];
    $course_info[$row["sym"]]["type"] = $row["coursetype"];
    $course_info[$row["sym"]]["title"] = $row["title"];
  }

  // check that all courses exist
  foreach($courses as $course) {
    if(!array_key_exists($course, $course_info)) {
      error("Ce cours n'existe pas: " . $course);
    }
  }

  $offsetOCChecks= 0;
  foreach($group_info as $sym => $entry1)
  {

    // Determine which types will form one block in the course form
    if($course_info[$sym]["type"] == 'T') {
      $types_to_consider = array("C" => 1);
    }
    elseif($course_info[$sym]["type"] == 'L') {
      $types_to_consider = array("L" => 1);
    }
    elseif($course_info[$sym]["type"] == 'TL') {
      $types_to_consider = array("C" => 1);
    }
    elseif($course_info[$sym]["type"] == 'TLS') {
      $types_to_consider = array("C" => 1, "L" => 1);
    }
    else {
      die("invalid course type");
    }

    echo '<table class="table table-bordered" style="max-width: 500px;">';
    echo '  <colgroup><col style="width: 100px;"><col><col style="width: 40px;"></colgroup>';
    echo '<tr style="background-color: #efefef;">';
    echo '  <td colspan="3"><strong>' . $sym . "</strong><br><strong>" . $course_info[$sym]["title"] . '</strong><br><a href="http://www.polymtl.ca/etudes/cours/details.php?sigle=' . $sym . '" target="_blank">Détails et horaire du cours</a></td>';
    echo '</tr>';

    foreach(array_intersect_key2($entry1, $types_to_consider) as $type => $entry2)
    {
      // compute the contents of $group_type_string
      if($course_info[$sym]["type"] == 'T' && $type == 'C') {
        $group_type_string = "Théorique";
        $cb_name_prefix = 't';
      }
      elseif($course_info[$sym]["type"] == 'L' && $type == 'L') {
        $group_type_string = "Laboratoire";
        $cb_name_prefix = 'l';
      }
      elseif($course_info[$sym]["type"] == 'TL' && $type == 'C') {
        $group_type_string = "Théorique et laboratoire";
        $cb_name_prefix = 't';
      }
      elseif($course_info[$sym]["type"] == 'TLS' && $type == 'C') {
        $group_type_string = "Théorique";
        $cb_name_prefix = 't';
      }
      elseif($course_info[$sym]["type"] == 'TLS' && $type == 'L') {
        $group_type_string = "Laboratoire";
        $cb_name_prefix = 'l';
      }
      else {
        die("internal error");
      }

      $nbOCChecks= count($entry2);

      echo '<tr><td colspan="3" style="background-color: #f9f9f9;"><div style="text-align: center; font-weight: bold;">' . $group_type_string . '</div>' .
      '<a href="javascript:changeOCChecks(' . $offsetOCChecks . ',' . $nbOCChecks . ')" class="oclink">(Ouvrir toutes les sections)</a>' .
      '<br><a href="javascript:changeOCChecks(' . $offsetOCChecks . ',' . $nbOCChecks . ',0)" class="oclink">(Fermer toutes les sections)</a>' .
      '</td></tr>';
      $offsetOCChecks+= $nbOCChecks;

      foreach($entry2 as $groupname => $entry3) {
        $cb_name = $cb_name_prefix . "_" . $sym . "_" . $groupname;
        $cb_name = ereg_replace("[^A-Za-z0-9]", "_", $cb_name);
        $openclosevars = $openclosevars . $cb_name . " ";
        echo '<tr>';
        if($entry3["closed"] == 1) {
          echo '<td style="background-color:#ffaaaa; text-align: center; vertical-align: middle;">';
        }
        else {
          echo '<td style="text-align: center;">';
        }

        // Print group info
        echo $groupname . ' <input id="' . $cb_name . '" name="' . $cb_name . '" type="checkbox"' . ($entry3["closed"] ? '' : ' checked') . '></td>';
        echo '<td vertical-align: middle;>'; // Begin teacher td

        if($course_info[$sym]["type"] == 'TL') {
          // If theory and lab same, we must specify 2 teachers,
          // therefore we indicate which one is 'theory' and 'lab'
          echo "<b>Théorie:</b> ";
        }

        echo $entry3["teacher"];

        if($course_info[$sym]["type"] == 'TL') {
          echo "<br><b>Lab:</b> " . $entry1["L"][$groupname]["teacher"] . "";
        }

        if ($entry3["closed"]) {
          echo '</td><td style="vertical-align: middle;"><a class="email" target="_blank" href="email_form.php?unique=' . $entry3["unique"] . '" onclick="this.href=\'javascript:openNotification(' . $entry3["unique"] . ')\'; this.target=\'\'"><img src="mail.jpg" alt="Notification par email"></a></td>';
        }
        else {
          echo '</td><td></td>';
        }

        echo "</tr>\n";
      }
    }
    echo "</table>";
  }

  echo "<input type=\"hidden\" name=\"openclose_vars\" value=\"" . $openclosevars . "\">";

  // here's the count for the ocCheckboxOffset: 2 * courses + 1 hidden + 5 optimizations + 1 conflict + 1 minimum courses + 5 * 10 schedule blocks
  echo '
    <script type="text/javascript">

    function changeOCChecks(start, num, checked)
    {
      if (arguments.length == 2)
      {
        checked= 1;
      }

      // this is the index of the first checkbox for the open close form, the first form element has index 0
      ocCheckboxOffset= ' . (2 * count($courses) + 58) . ';

      for (i= 0; i < num; i++)
      {
        document.form2[ocCheckboxOffset + start + i].checked= checked;
      }
    }

    function openNotification(group)
    {
      window.open("email_form.php?unique=" + group ,"notification","width=500,height=460,scrollbars=yes,resizable=yes,toolbar=yes,directories=no,status=yes,menubar=no,copyhistory=no,top=20,screenY=20");
    }
  </script>
  ';

}

// Sanitize the courses
$courses_raw = strtoupper($_GET['courses']);
// Replace bizarre chars with spaces
$courses_raw = ereg_replace("[^A-Z0-9\.\-]", " ", $courses_raw);
// Replace multiple spaces with one space
$courses_raw = ereg_replace(" +", " ", $courses_raw);
// Remove initial and final whitespace
$courses_raw = trim($courses_raw);

// Put the courses in an array
$courses = explode(" ", $courses_raw);
if(strlen($courses_raw) == 0) {
  error("Aucun cours spécifié");
}

?>

<?php begin_page('Étape 2'); ?>
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

      <?php generator_steps(2); ?>

      <div class="row">
        <div class="col-md-offset-2 col-md-8 large">
          <form method="POST" action="make.php" name="form2">

            <div class="panel panel-default" style="margin-top: 20px;">
              <div class="panel-heading">
                <h2 class="panel-title">Cours demandés</h2>
              </div>

              <div class="panel-body">
                <p>
                  Indiquer quels cours le Générateur peut choisir de ne pas mettre dans certains horaires. Cela peut aider à trouver une combinaison de cours qui donne un horaire sans conflits. Si vous voulez simplement un horaire qui comprend tous les cours ci-dessous, laissez-les marqués comme obligatoires.
                </p>
                <p>
                  À quoi servent les cours facultatifs? <a data-toggle="popover" data-content="<p>Si vous avez le choix entre plusieurs cours dans votre cheminement et que vous voulez trouver un horaire optimal qui contient un sous-ensemble de ces cours, inscrivez-les tous dans le formulaire de la page précédente, et marquez-les facultatifs ici. Marquez comme obligatoires les cours que vous voulez absolument. Toutes les possibilités d'horaires avec ou sans les cours marqués facultatifs seront considérées. Ils seront ensuite triés selon l'optimisation choisie ci-dessous (ex: minimiser les trous).</p><p>Cette option est pratique également lorsque vous êtes incapable de trouver une combinaison de cours qui n'entrent pas en conflit. Laissez le générateur choisir parmi la liste de tous les cours que vous avez la possibilité de prendre en les marquant comme optionnels."><i class="fa fa-lg fa-question-circle"></i></a>
                </p>

                <table class="table table-bordered table-striped" style="width: auto;">
                  <thead>
                    <tr>
                      <th>Cours</th>
                      <th style="text-align: center;">Obligatoire</th>
                      <th style="text-align: center;">Facultatif</th>
                    </tr>
                  </thead>
                  <tbody>
                    <?php
                      foreach($courses as $course) {
                        echo "<tr><td>$course</td><td style='text-align: center;'><input name=\"obl_".string2varname($course)."\" type=\"radio\" value=\"1\" checked></td><td style='text-align: center;'><input name=\"obl_".string2varname($course)."\" type=\"radio\" value=\"0\"></td></tr>\n";
                      }
                    ?>
                  </tbody>
                </table>

                <input type="hidden" name="courses" value="<?php echo $courses_raw; ?>">

                <div class="form-inline">
                  <div class="form-group">
                    <label for="mincoursecount">Nombre minimum de cours dans l'horaire</label>
                    <input type="text" id="mincoursecount" name="mincoursecount" value="1" size="2" class="form-control input-sm">
                    <a href="#" data-toggle="popover" data-content="Indiquer ici le nombre minimum de cours que l'horaire doit comporter. Cette option est nécessaire seulement lorsque vous avez beaucoup de cours marqués optionnels ci-dessus."> <i class="fa fa-lg fa-question-circle"></i></a>
                  </div>
                </div>
              </div>
            </div>

            <div class="panel panel-default">
              <div class="panel-heading">
                <h2 class="panel-title">Type d'optimisation</h2>
              </div>

              <div class="panel-body">
                <p>
                  L'une ou l'autre de ces options générera la même liste d'horaires.<br>
                  Seul l'ordre dans lequel ils seront affichés sera affecté.
                </p>

                <div class="radio">
                  <label>
                    <input name="order" type="radio" value="minholes" checked>
                    Minimiser les trous
                    <a href="" data-toggle="popover" data-content="Affiche en premier les horaires qui minimisent le nombre d'<strong>heures d'attente</strong> entre les cours durant une journée."><i class="fa fa-lg fa-question-circle"></i></a>
                  </label>
                </div>

                <div class="radio">
                  <label>
                    <input name="order" type="radio" value="maxmorningsleep">
                    Maximiser les heures de sommeil le matin
                    <a href="" data-toggle="popover" data-content="Affiche en premier les horaires qui permettent de <strong>commencer le plus tard possible</strong> le plus de jours possible."><i class="fa fa-lg fa-question-circle"></i></a>
                  </label>
                </div>

                <div class="radio">
                  <label>
                    <input name="order" type="radio" value="maxfreedays">
                    Maximiser le nombre de jours de congé
                    <a href="" data-toggle="popover" data-content="Affiche en premier les horaires qui offrent <strong>le plus de jours sans cours</strong>."><i class="fa fa-lg fa-question-circle"></i></a>
                  </label>
                </div>

                <div class="radio">
                  <label>
                    <input name="order" type="radio" value="maxcourses">
                    Maximiser le nombre de cours
                    <a href="" data-toggle="popover" data-content="<p>Affiche en premier les horaires qui contiennent <strong>le plus de cours</strong>.</p><p>Utile seulement si au moins un cours a été marqué optionnel ci-dessus. Autrement, tous les horaires auront le même nombre de cours.</p>"><i class="fa fa-lg fa-question-circle"></i></a>
                  </label>
                </div>

                <div class="radio">
                  <label>
                    <input name="order" type="radio" value="minconflicts">
                    Minimiser le nombre de périodes de conflits
                    <a href="" data-toggle="popover" data-content="<p>Affiche en premier les horaires qui contiennent le moins de conflits.</p><p>Utile seulement si le <em>nombre maximum de périodes avec conflit</em> ci-dessous est supérieur à zéro. Autrement, tous les horaires générés sont libres de conflits.</p>"><i class="fa fa-lg fa-question-circle"></i></a>
                  </label>
                </div>
              </div>
            </div>

            <div class="panel panel-default">
              <div class="panel-heading">
                <h2 class="panel-title">Conflits</h2>
              </div>

              <div class="panel-body">
                <div class="form-inline">
                  <div class="form-group">
                    <label for="mincoursecount">Nombre maximum de périodes avec conflit</label>
                    <input type="text" id="maxconflicts" name="maxconflicts" value="0" size="2" class="form-control input-sm">
                    <a href="#" data-toggle="popover" data-content="<p>Lorsque ce champ vaut 0, seuls des horaires sans conflits sont générés.</p><p>Lorsque ce champ est supérieur à zéro, les horaires générés peuvent comporter des conflits. Seuls les horaires dont le nombre de périodes comportant des conflits ne dépasse pas la valeur entrée dans ce champ seront affichés.</p><p>Utilisez ce champ lorsque vous devez absolument prendre certains cours, mais que le générateur ne trouve pas d'horaire sans conflits. Combinez-le avec <em>Minimiser le nombre de périodes de conflits</em> ci-dessus afin d'afficher les horaires comportant le moins de conflits d'abord.</p>"> <i class="fa fa-lg fa-question-circle"></i></a>
                  </div>
                </div>
              </div>
            </div>

            <div class="panel panel-default">
              <div class="panel-heading">
                <h2 class="panel-title">Périodes libres</h2>
              </div>

              <div class="panel-body">
                <p>
                  Cocher les périodes qui doivent absolument être libres.<br>
                  Cliquer sur un nom de colonne ou de rangée pour activer ou désactiver tout ce groupe de cases.
                </p>

                <table class="table table-bordered table-striped" style="width: auto;">
                  <colgroup>
                    <col style="width: 15%;"></col>
                    <col style="width: 17%;"></col>
                    <col style="width: 17%;"></col>
                    <col style="width: 17%;"></col>
                    <col style="width: 17%;"></col>
                    <col style="width: 17%;"></col>
                  </colgroup>

                  <thead>
                    <tr>
                      <th></th>
                      <?php
                        $weekdays = array("Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi");
                        // Print the day names
                        for($i=0; $i < count($weekdays); $i++) {
                          print "<th style=\"text-align: center;\" onClick=\"switchCol(" . $i . ")\">${weekdays[$i]}</th>\n";
                        }
                      ?>
                    </tr>
                  </thead>

                  <tbody>
                    <?php
                      $week_hours = array(830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645);

                      for($i=0; $i<count($week_hours); $i++) {
                        print "<tr>\n";
                        $hour_string = sprintf("%d:%.2d", $week_hours[$i]/100, $week_hours[$i]%100);
                        print "<th style=\"background-color: #efefef; text-align: right; padding-right: 15px;\" onClick=\"switchRow(" . $i . ")\">${hour_string}</th>";

                        for($j=0; $j<5; $j++) {
                          $period_number = ($j+1)*10000+$week_hours[$i];
                          print '<td style="text-align: center;"><input name="period_' . $period_number . '" type="checkbox"></td>' . "\n";
                        }
                        print "</tr>\n";
                      }

                      // Print Evening checkboxes
                      print "<tr>\n<th style=\"background-color: #efefef; text-align: right; padding-right: 15px;\" onClick=\"switchRow(" . count($week_hours) . ")\">Soir</th>\n";
                      for($j=0; $j<5; $j++) {
                        print "<td style=\"text-align: center;\"><input name=\"noevening_$j\" type=\"checkbox\"></td>\n";
                      }
                      print "</tr>\n";
                    ?>
                  </body>
                </table>
              </div>

              <script type="text/javascript">
                // this is the index of the first checkbox for the schedule, the first form element has index 0
                // here's the count: 2 * courses + 1 hidden + 5 optimizations + 1 conflict + 1 minimum courses
                scheduleCheckboxOffset= <?php echo 2 * count($courses) + 8; ?>;
                colStatus= new Array(<?php echo count($weekdays) ?>);
                rowStatus= new Array(<?php echo count($week_hours) + 1 ?>);

                function switchRow(numRow)
                {
                  rowStatus[numRow]= !rowStatus[numRow];

                  for (i= 0; i < <?php echo count($weekdays) ?>; i++)
                  {
                    document.form2[scheduleCheckboxOffset + numRow * <?php echo count($weekdays) ?> + i].checked= rowStatus[numRow];
                  }
                }

                function switchCol(numCol)
                {
                  colStatus[numCol]= !colStatus[numCol];

                  for (j= 0; j < <?php echo count($week_hours) + 1 ?>; j++)
                  {
                    document.form2[scheduleCheckboxOffset + numCol + j * <?php echo count($weekdays) ?>].checked= colStatus[numCol];
                  }
                }
              </script>
            </div>

            <div class="panel panel-default">
              <div class="panel-heading">
                <h2 class="panel-title">Ouvrir/fermer des sections</h2>
              </div>

              <div class="panel-body">
                <p>
                  Cocher les sections qui doivent être considérées comme ouvertes. Par défaut, seules les sections où il reste de la place sont cochées.
                </p>

                <p>
                  <b>Case cochée: </b> Section ouverte<br>
                  <b style="background-color:#ffaaaa;">Case rouge:</b> Section pleine
                </p>

                <p>
                  <img src="mail-white.jpg" alt="Notification par email" style="float:left; margin-right: 1ex;">
                  Le système peut vous avertir par email si une section qui est fermée devient ouverte. Cliquez sur l'icône pour vous inscrire.
                </p>

                <?php print_open_close_form($courses); ?>
              </div>
            </div>

            <div class="panel panel-default">
              <div class="panel-heading">
                <h2 class="panel-title">Avertissement</h2>
              </div>

              <div class="panel-body">
                <p>
                  Nous nous sommes efforcés de faire en sorte que ce programme fonctionne correctement. Cependant, il est en cours de développement et des problèmes pourraient survenir. Il est donc fourni sans aucune garantie.
                </p>
              </div>
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

<script>
  $(function () {
    $('[data-toggle="popover"]').popover({ html: true, trigger: 'hover' });
  });
</script>

<?php end_page(); ?>
