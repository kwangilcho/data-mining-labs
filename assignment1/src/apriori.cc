/**
 * Implement the apriori algorithm for finding association rules
 *
 * @author      Kwangil Cho
 * @since       2017-03-14
 * @file        apriori.cc
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <utility>
#include <algorithm>
#include <iomanip>
#include "apriori.h"

double g_min_sup; // minimum support is probability
int    g_min_sup_cnt; // minimum support count = # of trxes * minimum support 
fstream  g_fs_input; // input file stream
ofstream g_fs_output; // output file stream
vector<trx_t> g_trxes; // list of transactions
vector<itemsets_t> g_total_itemsets; // frequent itemsets of each level

/**
 * @param[in]   command line arguments as below:
 *              argv[0] executable file
 *              argv[1] minimum support(integer)
 *              argv[2] input file
 *              argv[3] output file
 * @return      0
 */
int main(int argc, const char *argv[]) {
    if (argc == 4) {
        InitCLA(argv);
        InitTrx();
        Apriori();
    } else {
        cout << "Invalid usage. Please check the arguments." << endl
           << "$ [executable file] [minimum support] [input.txt] [output.txt]" << endl;
    }

    g_fs_input.close();
    g_fs_output.close();

    return 0;
}

/**
 * Initialize minimum support and open in/out file with Command Line Arguments
 *
 * @param[in]   argv   input command line arguments 
 */
void InitCLA(const char *argv[]) {

    // get minimum support from input and set as probability
    g_min_sup = (double)atoi(argv[1]) / 100;
    if (g_min_sup > 100 || g_min_sup < 0) {
        cout << "Minimum support should be in the 0 ~ 100 range. Program terminated." << endl;
        exit(0);
    }

    // open input file
    g_fs_input.open(argv[2], ifstream::in);
    if (g_fs_input.is_open() == false) {
        cout << "Can't open input text file. Program terminated." << endl;
        exit(0);
    }

    // open output file
    g_fs_output.open(argv[3], ofstream::out);
    if (g_fs_output.is_open() == false) {
        cout << "Can't open output text file. Program terminated." << endl;
        exit(0);
    }

    // set the output precision to the second decimal place
    g_fs_output << fixed << setprecision(2);
}

/**
 * Initialize transaction info by reading and parsing the input file
 */
void InitTrx() {
    trx_t trx;
    string items;
    string item;
    string delimiter = "\t";
    size_t pos = 0;

    // read every line from input file
    while (getline(g_fs_input, items)) {
        trx.items.clear();
        // parse and find out the item for making itemset
        while ((pos = items.find(delimiter)) != string::npos) {
            item = items.substr(0, pos);
            items.erase(0, pos + delimiter.length());
            trx.items.insert(stoi(item));
        }
        trx.items.insert(stoi(items));
        g_trxes.push_back(trx);
    }

    // set minimum support count by minimum support and the total size of trxes
    g_min_sup_cnt = g_min_sup * g_trxes.size();
    g_fs_input.close();
}

/**
 * Main routine of Apriori algorithm
 *
 * 1. Scan the first frequent patterns which have 1-item sets
 * 2. To generate candidates, do selfjoin with previous level frequent patterns
 * 3. Prune the generated candidates by downward closure property 
 * 4. Find out new frequent patterns at the level by scanning with left candidates
 * 5. Apply association rule to each frequent pattern
 */
void Apriori() {
    ScanFirstFrequentPattern();
    for (int level = 2; level > 0; level++) {
        SelfJoin(level);
        Prune(level);
        ApplyAssociationRule(level);
    }
}

/**
 * 1. Scan the first frequent patterns which have 1 item-sets
 *
 * 1) for each transaction in DB(input file), find a item for making 1-item set
 * 2) If it is new, make new 1-item set
 * 3) If it isn't, which means the itemset already exists, just increase the support
 * 4) After building new 1-item sets filter out not frequent patterns 
 */
void ScanFirstFrequentPattern() {
    itemsets_t dummy;
    g_total_itemsets.push_back(dummy);

    itemsets_t itemsets;
    itemsets_t::iterator it;
    itemset_t itemset;

    // scan all transactions
    for (auto trx : g_trxes) {
        // scan all items in each transaction 
        // for building 1-item itemsets
        for (auto item_in_db : trx.items) {
            //init itemset
            itemset.clear();
            itemset.insert(item_in_db);

            //find itemset in itemsets
            it = itemsets.find(itemset);
            //if it is in itemsets, increase support
            if(it != itemsets.end()) {
                (it->second)++;
            //if it isn't, make new one
            } else {
                itemsets.insert(pair<set<int>, int> (itemset, 1));
            }
        }
    }

    // find out frequent itemsets
    for (it = itemsets.begin(); it != itemsets.end();) {
        if (it->second < g_min_sup_cnt) {
            it = itemsets.erase(it);
        } else {
            it++;
        }
    }

    // make it level 1 frequent itemsets
    g_total_itemsets.push_back(itemsets);
}

/**
 * 2. To generate candidates, do selfjoin with previous level frequent patterns
 * 1) It is not efficient way that just selfjoining like matrix multiplication
 *    because there can be a lot of redundant data(set)
 * 2) To generate candidates in smart way, use the combination
 *
 * @param[in]   level   the level that generates candidates with its previous level
 */
void SelfJoin(int level) {
    itemsets_t &prev_itemsets = g_total_itemsets[level - 1];
    itemsets_t curr_itemsets;
    itemset_t all_items;

    for (auto itemset : prev_itemsets) {
        for (auto item : itemset.first) {
            all_items.insert(item);
        }
    }

    vector<itemset_t> candidates;
    GenerateCandidates(level, all_items, candidates);

    for (auto candidate : candidates) {
        curr_itemsets.insert(pair<set<int>, int> (candidate, 0));
    }

    g_total_itemsets.push_back(curr_itemsets);
}

/**
 * 2. To generate candidates, do selfjoin with previous level frequent patterns
 *
 * 2) To generate candidates in smart way, use the combination
 *    - nCr
 *    - use permutation and validation array(filter array) to make the combination
 *
 * @param[in]   level       the level that generates candidates with its previous level
 * @param[in]   all_items   the set which contains all kinds of items in the previous level
 * @param[out]  candidates  the list of candidates that generated by combination
 */
void GenerateCandidates(int level, itemset_t &all_items, vector<itemset_t> &candidates) {
    int n = all_items.size(); 
    int r = level;

    vector<bool> v(n);
    fill(v.begin(), v.begin() + r, true);
    vector<int> v_items;

    for (auto item : all_items) {
        v_items.push_back(item);
    }

    do {
        itemset_t candidate;
        for (int i = 0; i < n; i++) {
            if (v[i]) {
                candidate.insert(v_items[i]);
            }
        } 

        // because duplicated value can be generated,
        // should check the candidate size
        if ((int)candidate.size() == level) {
            candidates.push_back(candidate);
        } 
    } while (prev_permutation(v.begin(), v.end()));
}

/**
 * Check out whether the candidate exists or not in that level itemsets
 *
 * @param[in]   level       the level of target candidate itemsets
 * @return      bool        if there is any candidate exist true, else false
 */
bool IsCandidateExist(int level) {
    return g_total_itemsets[level].size() > 0;
}

/**
 * 3. Prune the generated candidates by downward closure property 
 * 4. Find out new frequent patterns at the level by scanning with left candidates
 *
 * @param[in]   level       the level that generated candidates with its previous level
 */
void Prune(int level) {
    if (IsCandidateExist(level)) {
        ApplyDownwardClosureProperty(level);
        RemoveUnqualifiedCandidates(level);
    } else {
        level = -1;
    }
}

/**
 * 3. Prune the generated candidates by downward closure property 
 * If the candidate that is generated from selfjoin is frequent,
 * all of its previous level subsets should be the frequent itemsets.
 * If it isn't, the candidate can't be the frequent itemset
 *
 * @param[in]   level       the level that generated candidates with its previous level
 */
void ApplyDownwardClosureProperty(int level) {
    itemsets_t &prev_itemsets = g_total_itemsets[level - 1];
    itemsets_t &curr_itemsets = g_total_itemsets[level];
    itemsets_t::iterator curr_it = curr_itemsets.begin();

    for (; curr_it != curr_itemsets.end();) {
        vector<itemset_t> subsets;
        itemset_t itemset = curr_it->first;
        GenerateCandidates(level - 1, itemset, subsets);

        bool is_valid = true;
        for (auto subset : subsets) {
            itemsets_t::iterator it = prev_itemsets.find(subset);
            if(it != prev_itemsets.end()) {
                continue;
            } else {
                is_valid = false;
                break;
            }
        }

        if (is_valid) {
            curr_it++;
        } else {
            curr_it = curr_itemsets.erase(curr_it);
        }
    }
}

/**
 * 4. Find out new frequent patterns at the level by scanning with left candidates
 *
 * @param[in]   level       the level that generated candidates with its previous level
 */
void RemoveUnqualifiedCandidates(int level) {
    itemsets_t &curr_itemsets = g_total_itemsets[level];
    itemsets_t::iterator it;
    int match_cnt;

    for (auto trx : g_trxes) {
        for (it = curr_itemsets.begin(); it != curr_itemsets.end(); it++) {
            itemset_t curr_itemset = it->first; 
            match_cnt = 0;

            for (auto item_in_trx : trx.items) {
                for (auto item_in_curr_itemset : curr_itemset) {
                    if (item_in_trx == item_in_curr_itemset) {
                       match_cnt++; 
                    }
                }
            }

            if (match_cnt == level) {
                it->second++;
            }
        }
    }

    for (it = curr_itemsets.begin(); it != curr_itemsets.end();) {
        if (it->second < g_min_sup_cnt) {
            it =  curr_itemsets.erase(it);
        } else {
            it++;
        }
    }
}

/**
 * 5. Apply association rule to each frequent pattern
 * Make subsets and their counter part set which is associative set from the frequent itemset
 * Find out and print the relationship between the sets with support and confidence
 *
 * @param[in]   level       the level that generated candidates with its previous level
 */
void ApplyAssociationRule(int &level) {
    if (IsCandidateExist(level)) {
        itemsets_t &curr_itemsets = g_total_itemsets[level];
        for (auto itemset : curr_itemsets) {
            vector<itemset_t> subsets;
            MakeSubsets(itemset.first, subsets);
            for (auto subset : subsets) {
                int curr_subset_cnt = 0;
                int asc_subset_cnt = 0;

                // make associative itemset
                itemset_t asc_set = itemset.first; 
                for (auto item_in_subset : subset) {
                   asc_set.erase(item_in_subset); 
                }

                for (auto trx : g_trxes) {
                    bool is_in_db = true;
                    itemset_t::iterator it;

                    // is subset item in subset?
                    for (auto item : subset) {
                        it = trx.items.find(item);
                        if(it == trx.items.end()) {
                            is_in_db = false;
                            break;
                        } else {
                            continue;
                        }
                    }

                    // is subset item in ascset?
                    if (is_in_db) {
                        curr_subset_cnt++;
                        for (auto item : asc_set) {
                            it = trx.items.find(item);
                            if(it == trx.items.end()) {
                                is_in_db = false;
                                break;
                            } else {
                                continue;
                            }
                        }
                    }

                    if (is_in_db) {
                        asc_subset_cnt++;
                    }
                }

                PrintAssociationSet(subset, asc_set);
                //support
                g_fs_output << (double)itemset.second / (double)g_trxes.size() * 100  << "\t";
                //confidence
                double confidence = (double)asc_subset_cnt / (double)curr_subset_cnt * 100 * 100;
                confidence = round(confidence);
                confidence /= 100;
                
                g_fs_output << confidence << endl;
            }
        }
    } else {
        level = -1; 
    }
}

/**
 * Make all possible subsets excludes empty set and identical set by combination
 * If itemset has n items, 2^n - 2 subsets are being generated
 *
 * @param[in]   subset  subset of the frequent itemset
 * @param[in]   asc_set subset which is the counter part of subset
 */
void MakeSubsets(itemset_t set, vector<itemset_t> &subsets) {
    vector<int> v_itemset; 

    for (auto item : set) {
        v_itemset.push_back(item);
    }

    int num_of_elements = v_itemset.size();
    int num_of_subsets  = 1 << v_itemset.size();

    itemset_t subset;
    for (int i = 1; i < num_of_subsets - 1; i++) {
        subset.clear();
        for (int j = 0; j < num_of_elements; j++) {
            if ((1 << j) & i) {
                subset.insert(v_itemset[j]);
            }
        }
        subsets.push_back(subset);
    }
}

/**
 * Print the subset and its counter part associative set
 *
 * @param[in]   subset  subset of the frequent itemset
 * @param[in]   asc_set subset which is the counter part of subset
 */
void PrintAssociationSet(itemset_t &subset, itemset_t &asc_set) {
    itemset_t::iterator it = subset.begin();
    int set_size = subset.size();

    // print subset items
    g_fs_output << "{";
    for (int i = 0; i < set_size; i++) {
        g_fs_output << *it;
        if (i < set_size - 1) {
            g_fs_output << ",";
        }
        it++;
    }
    g_fs_output << "}\t";

    // print associative set items
    it = asc_set.begin();
    set_size = asc_set.size();
    g_fs_output << "{";
    for (int i = 0; i < set_size; i++) {
        g_fs_output << *it;
        if (i < set_size - 1) {
            g_fs_output << ",";
        }
        it++;
    }
    g_fs_output << "}\t";
}
