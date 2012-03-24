set terminal png
set parametric
set xrange[0:70]
set yrange[0:70]
set zrange[0:70]
splot [0:pi/2] 64*sin(u), 64*cos(u), 10 w lines lt 3, "rk2-1.dat" w lines lt 1 title "RK2 (h=0.1)", "rk4-1.dat" w lines lt 2 title "RK4 (h=0.1)", "rk2-0001.dat" w lines lt 4 title "RK2 (h=0.0001)", "rk4-0001.dat" w lines lt 5 title "RK4 (h=0.0001)"
