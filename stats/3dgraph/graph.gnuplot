# set terminal png size 1024 768

set title ""
set xlabel "Année"
set ylabel "Date"
set zlabel "

set ydata time
set timefmt "%Y/%m/%d %H"
set yrange ["12/06":"12/16"]
set format y "%d %b %Hh"
set timefmt "%m/%d"
#set style fill solid

splot 'data.txt'
pause -1
