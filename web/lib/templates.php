<?php

function begin_page($title)
{
  $base_title = "Générateur d'horaires de l'AEP | Polytechnique Montréal";
  if (trim($title)) {
    $title = $title . ' | ' . $base_title;
  }
  else {
    $title = $base_title;
  }

  require_once('templates/shared/head.php');
}

function main_navbar($current_semester)
{
  require_once('templates/shared/main_navbar.php');
}

function main_sidebar($current_page)
{
  require_once('templates/shared/main_sidebar.php');
}

function active($expression, $with_class = True)
{
  if ($expression)
  {
    if ($with_class) {
      return ' class="active"';
    }
    else {
      return ' active';
    }
  }
}

function end_page()
{
  require_once('templates/shared/footer.php');
}


function generator_steps($active_step)
{
  require_once('templates/generator/steps.php');
}

?>
