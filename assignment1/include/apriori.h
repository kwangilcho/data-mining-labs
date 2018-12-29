/**
 * Implement the apriori algorithm for finding association rules
 *
 * @author Kwangil Cho
 * @since  2017-03-14
 * @file        apriori.h
 */

#ifndef __ASSIGNMENT1_INCLUDE_APRIORI_H__
#define __ASSIGNMENT1_INCLUDE_APRIORI_H__
#include <vector>
#include <map>
#include <set>

using namespace std;

typedef set<int> itemset_t;
typedef map<set<int>, int> itemsets_t;

struct trx_t {
    itemset_t items;
};

void InitCLA(const char*[]);
void InitTrx();
void Apriori();
void ScanFirstFrequentPattern();
void SelfJoin(int);
void GenerateCandidates(int, itemset_t&, vector<itemset_t>&);
bool IsCandidateExist(int);
void Prune(int);
void ApplyDownwardClosureProperty(int);
void RemoveUnqualifiedCandidates(int);
void ApplyAssociationRule(int&);
void MakeSubsets(itemset_t, vector<itemset_t>&);
void PrintAssociationSet(itemset_t&, itemset_t&);

#endif // __ASSIGNMENT1_INCLUDE_APRIORI_H__
