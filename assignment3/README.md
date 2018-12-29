# Perform clustering on a given data set by using DBSCAN.

Density-based spatial clustering of applications with noise (DBSCAN) is a data clustering algorithm proposed by Martin Ester, Hans-Peter Kriegel, Jörg Sander and Xiaowei Xu in 1996. It is a density-based clustering algorithm: given a set of points in some space, it groups together points that are closely packed together (points with many nearby neighbors), marking as outlier points that lie alone in low-density regions (whose nearest neighbors are too far away).

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites & Installation

```
$ git clone http://hconnect.hanyang.ac.kr/2017_ITE4005_10066/2017_ITE4005_2012003716.git
$ cd 2017_ITE4005_2012003716/assignment3/
$ make
```

## Run the test

### Automatic test with sample input(Recomended)

```
$ sh ./run.sh

check out the output data with postfix ('_input_data_cluster_#.txt')
$ cd ./data/output/
check out the result figure from output data 
$ cd ./data/result/
```

### Manual test

```
$ ./bin/clustering [input_data.txt] [number of clusters] [Eps] [Minpts]

check out the output data with postfix ('_input_data_cluster_#.txt')
$ cd ./data/output/
```

### Clean up the executable, output files and result figures

```
$ make clean
```

## Development environment

* Operating System: Ubuntu 14.05 LTS 64-bit
* Compilation: g++ compiler
* Language: C/C++ with C++ standard 11
* Source code editor: Vim
* Source code version control: Git
* Plot application: Gnuplot
* This project follows HYU coding convention faithfully

## Authors

* **Kwangil Cho**
    * student ID: 2012003716
    * senior of department of computer science
    * member of [Lab.SCS](http://scslab.hanyang.ac.kr/)
