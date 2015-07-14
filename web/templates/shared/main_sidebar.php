<?php

$links = [
  ['accueil', 'index.php', 'Accueil'],
  ['generateur_horaires', 'make_form1.php', 'Générateur d\'horaires'],
  ['horaire_local', 'room_form.php', 'Horaire d\'un local'],
  ['horaire_charge_cours', 'teacher_form.php', 'Horaire d\'un chargé de cours'],
  ['desinscription_courriel', 'email_unsubscribe.php', 'Désinscription<br />(notification automatique)'],
  ['cours_offerts', 'listcourses.php', 'Cours offerts']
];

?>

<div class="col-sm-3 col-md-2 sidebar">
  <ul class="nav nav-sidebar">
    <?php
      foreach($links as $link)
      {
        echo "<li" . active($current_page == $link[0]) . "><a href=\"" . $link[1] . "\">" . $link[2] . "</a></li>\n";
      }
    ?>
  </ul>
</div>
