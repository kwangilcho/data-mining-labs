# Apriori algorithm for finding association rules

Apriori is an algorithm for frequent item set mining and association rule learning over transactional databases.
It proceeds by identifying the frequent individual items in the database and extending them to larger and larger item sets
as long as those item sets appear sufficiently often in the database.
The frequent item sets determined by Apriori can be used to determine association rules which highlight general trends in the database:
this has applications in domains such as market basket analysis.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites & Installation

```
$ git clone http://hconnect.hanyang.ac.kr/2017_ITE4005_10066/2017_ITE4005_2012003716.git
$ cd 2017_ITE4005_2012003716/assignment1/
$ make
```

## Run the test

```
$ ./bin/apriori [minimum support] [input.txt] [output.txt]
$ vi output.txt
```

## Development environment

* Operating System: Ubuntu 14.05 LTS 64-bit
* Compilation: g++ compiler
* Language: C/C++ with C++ standard 11
* Source code editor: Vim
* Source code version control: Git
* This project follows HYU coding convention faithfully

## Authors

* **Kwangil Cho**
    * student ID: 2012003716
    * senior of department of computer science
    * member of [Lab.SCS](http://scslab.hanyang.ac.kr/)