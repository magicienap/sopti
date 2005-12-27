# set terminal png size 1024 768

set title "Nombre de requêtes et d'hôtes distincts par heure sur le générateur d'horaires"
set xlabel "Date et heure"
set ylabel "Nombre"

set xdata time
set timefmt "%Y/%m/%d %H"
set xrange ["2004/12/15 12":"2004/12/31 0"]
set format x "%d %b %Hh"
set timefmt "%Y/%m/%d %H"
set style fill solid
set tics out
plot 'plot_reqs_per_hour.txt' using 1:3 with boxes title 'Requetes', 'plot_hosts_per_hour.txt' using 1:3 with boxes title 'Hôtes distincts'
pause -1
