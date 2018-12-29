# this shell script is for automatic test with sample input data
./bin/clustering ./data/input/input1.txt 8 15 22 
./bin/clustering ./data/input/input2.txt 5 2 7
./bin/clustering ./data/input/input3.txt 4 5 5
gnuplot plot.gp
