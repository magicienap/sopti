set terminal png size 1024 768

set title "Nombre de requêtes et d'hôtes distincts par jour sur le générateur d'horaires"
set xlabel "Date"
set ylabel "Nombre"

set xdata time
set timefmt "%Y/%m/%d %H"
set xrange ["2004/12/14":"2005/01/25"]
set format x "%d %b %Y"
set timefmt "%Y/%m/%d"
set style fill solid border -1
set tics out
set logscale y
plot 'plot_reqs_day.txt' using 1:2 with boxes title 'Requetes', \
     'plot_hosts_day.txt' using 1:2 with boxes title 'Hôtes distincts'
     #'plot_reqs_day.txt' using 1:3 title 'Requêtes totales' with linespoints lw 3, 'plot_hosts_day.txt' using 1:3 title 'Hôtes totaux'
pause -1
