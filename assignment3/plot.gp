# This gnuplot script is only for test input data 1,2,3 which is provided from TA

set autoscale
set terminal png

set xlabel "x coordinate"
set ylabel "y coordinate"

set nokey

# print core points
set output "./data/result/input1_cores.png"
plot "./data/output/input1_cores.txt" using 1:2 pt 7 ps 0.5
set output "./data/result/input2_cores.png"
plot "./data/output/input2_cores.txt" using 1:2 pt 7 ps 0.5
set output "./data/result/input3_cores.png"
plot "./data/output/input3_cores.txt" using 1:2 pt 7 ps 0.5

# print original point before clustering
set output "./data/result/input1_original.png"
plot "./data/output/input1_original.txt" using 1:2 pt 7 ps 0.5
set output "./data/result/input2_original.png"
plot "./data/output/input2_original.txt" using 1:2 pt 7 ps 0.5
set output "./data/result/input3_original.png"
plot "./data/output/input3_original.txt" using 1:2 pt 7 ps 0.5

# print clusters after clustering 
set output "./data/result/input1.png"
plot "./data/output/input1_xy_0.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_1.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_2.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_3.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_4.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_5.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_6.txt" using 1:2 pt 7 ps 0.5, "./data/output/input1_xy_7.txt" using 1:2 pt 7 ps 0.5
set output "./data/result/input2.png"
plot "./data/output/input2_xy_0.txt" using 1:2 pt 7 ps 0.5, "./data/output/input2_xy_1.txt" using 1:2 pt 7 ps 0.5, "./data/output/input2_xy_2.txt" using 1:2 pt 7 ps 0.5, "./data/output/input2_xy_3.txt" using 1:2 pt 7 ps 0.5, "./data/output/input2_xy_4.txt" using 1:2 pt 7 ps 0.5
set output "./data/result/input3.png"
plot "./data/output/input3_xy_0.txt" using 1:2 pt 7 ps 0.5, "./data/output/input3_xy_1.txt" using 1:2 pt 7 ps 0.5, "./data/output/input3_xy_2.txt" using 1:2 pt 7 ps 0.5, "./data/output/input3_xy_3.txt" using 1:2 pt 7 ps 0.5
