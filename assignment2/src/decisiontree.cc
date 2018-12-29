/**
 * Implement a decision tree for classifying the test set
 *
 * @author      Kwangil Cho
 * @since       2017-03-31
 * @file        decisiontree.cc
 */

#include "decisiontree.h"
#include <iostream>
#include <fstream>

fstream  g_fs_training; // training file stream
fstream  g_fs_test; // test file stream
ofstream g_fs_result; // result file stream
tuple_t g_attributes; // all attributes from training file
vector<vector<class_t>> g_class_of_attr; // all classes for each attribute
vector<tuple_t> g_training_data; // training data
tuple_t g_test_attr; // all attributes from test file
tuple_t g_test_data; // all testing tuple from test file
DecisionTree *g_tree;

/**
 * @param[in]   command line arguments as below:
 *              argv[0] executable file
 *              argv[1] training data file
 *              argv[2] test data file
 *              argv[3] output file
 * @return      0
 */
int main(int argc, const char *argv[]) {
    if (argc == 4) {
        InitCLA(argv);
        InitTrainingData();
        BuildDecisionTree();
        TestDecisionTree();
    } else {
        cout << "Invalid usage. Please check the arguments." << endl
           << "$ [executable file] [training file] [test file] [result file]" << endl;
    }

    return 0;
}

/**
 * Open in/out file with Command Line Arguments
 *
 * @param[in]   argv   input command line arguments 
 */
void InitCLA(const char *argv[]) {
    // open training file
    g_fs_training.open(argv[1], ifstream::in);
    if (g_fs_training.is_open() == false) {
        cout << "Can't open training file. Program terminated." << endl;
        exit(0);
    }

    // open test file
    g_fs_test.open(argv[2], ifstream::in);
    if (g_fs_test.is_open() == false) {
        cout << "Can't open test file. Program terminated." << endl;
        exit(0);
    }

    // open result file
    g_fs_result.open(argv[3], ofstream::out);
    if (g_fs_result.is_open() == false) {
        cout << "Can't open output file. Program terminated." << endl;
        exit(0);
    }
}

/**
 * Initialize training data(tuples) by reading and parsing the input training file
 */
void InitTrainingData() {
    tuple_t tuple;
    string input_line;
    string attribute;
    string delimiter = "\t";
    size_t pos = 0;

    // read first line from training file to init attributes
    getline(g_fs_training, input_line);
    // parse each attributes
    while ((pos = input_line.find(delimiter)) != string::npos) {
        attribute = input_line.substr(0, pos);
        input_line.erase(0, pos + delimiter.length());
        g_attributes.push_back(attribute);
    }
    // erase last new line character ('\n')
    input_line.erase(input_line.end() - 1, input_line.end());
    g_attributes.push_back(input_line);
    g_class_of_attr.assign(g_attributes.size(), vector<class_t>());
    set<class_t> set_of_class[g_attributes.size()];

    int class_idx = 0;
    // read every line from training file to init data tuple
    while (getline(g_fs_training, input_line)) {
        tuple.clear();
        class_idx = 0;
        while ((pos = input_line.find(delimiter)) != string::npos) {
            attribute = input_line.substr(0, pos);
            input_line.erase(0, pos + delimiter.length());
            tuple.push_back(attribute);
            set_of_class[class_idx].insert(attribute);
            class_idx++;
        }
        // erase last new line character ('\n')
        input_line.erase(input_line.end() - 1, input_line.end());
        tuple.push_back(input_line);
        set_of_class[class_idx].insert(input_line);
        g_training_data.push_back(tuple);
    }

    // transform classes of attribute from set to vector
    for (unsigned int i = 0; i < g_class_of_attr.size(); i++) {
        for (auto cls : set_of_class[i]) {
           g_class_of_attr[i].push_back(cls); 
        }
    }

    g_fs_training.close();
}

/**
 * Build the classification model with decision tree
 */
void BuildDecisionTree() {
    g_tree = new DecisionTree(g_training_data, g_attributes, g_class_of_attr);
}

/**
 * Classfy the test tuple with the decision tree and print the result 
 */
void TestDecisionTree() {
    string input_line;
    string attribute;
    string delimiter = "\t";
    size_t pos = 0;

    // read first line from test file to init attributes
    getline(g_fs_test, input_line);
    // parse each attributes
    while ((pos = input_line.find(delimiter)) != string::npos) {
        attribute = input_line.substr(0, pos);
        input_line.erase(0, pos + delimiter.length());
        g_test_attr.push_back(attribute);
    }
    // erase last new line character ('\n')
    input_line.erase(input_line.end() - 1, input_line.end());
    g_test_attr.push_back(input_line);

    // print the attributes to result file
    int size = g_attributes.size();
    for (int i = 0; i < size; i++) {
        g_fs_result << g_attributes[i];
        if (i < size - 1) {
           g_fs_result << "\t"; 
        }
    }
    g_fs_result << endl;

    tuple_t tuple;
    string classified_attr;
    // read every line from test file to init test tuple
    while (getline(g_fs_test, input_line)) {
        tuple.clear();
        classified_attr.clear();
        while ((pos = input_line.find(delimiter)) != string::npos) {
            attribute = input_line.substr(0, pos);
            input_line.erase(0, pos + delimiter.length());
            tuple.push_back(attribute);
        }
        // erase last new line character ('\n')
        input_line.erase(input_line.end() - 1, input_line.end());
        tuple.push_back(input_line);

        // Classfy the test tuple by searching decision tree
        classified_attr = g_tree->SearchAttribute(tuple, g_test_attr);
        tuple.push_back(classified_attr);

        // print the result tuple to result file
        int size = tuple.size();
        for (int i = 0; i < size; i++) {
            g_fs_result << tuple[i];
            if (i < size - 1) {
               g_fs_result << "\t"; 
            }
        }
        g_fs_result << endl;
    }

    g_fs_test.close();
    g_fs_result.close();
}
