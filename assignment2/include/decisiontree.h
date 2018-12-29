/**
 * Implement a decision tree for classifying the test set
 *
 * @author      Kwangil Cho
 * @since       2017-03-31
 * @file        decisiontree.h
 */
#ifndef __ASSIGNMENT2_INCLUDE_DECISION_TREE_H__
#define __ASSIGNMENT2_INCLUDE_DECISION_TREE_H__
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <cmath>

using namespace std;

typedef string class_t;
typedef vector<string> tuple_t;

class DecisionTree {
public:
    string attribute_; // the classifier index of the tree node
    vector<class_t> classes_; // children classes of the tree node
    vector<DecisionTree> branches_; // children classes' node
    bool is_leaf_; // if the node is leaf, it means the classification process has been done

    vector<tuple_t> training_data_; // training data for select the attribute
    tuple_t attributes_; // attributes of training data
    vector<vector<class_t>> class_of_attr_; // classes of each attribute
    double info_entropy_before_; // Expected information(information entropy) before split

    /**
     * Generate DecisionTree recursively with greedy manner
     *  - Use information ratio for selecting attribute
     *  - Check the stop splitting condition
     *
     * @param[in]   training_data    tuple data for training the model at each level
     * @param[in]   attributes       attributes for training the model at each level
     * @param[in]   class_of_attr    sub classes of each attribute 
     */
    DecisionTree(vector<tuple_t> &training_data, tuple_t &attributes, vector<vector<class_t>> &class_of_attr)
    : is_leaf_(false), training_data_(training_data), attributes_(attributes), class_of_attr_(class_of_attr) {

        GetInfoEntropyBeforeSplit();
        if(ContinueSplit()) {
            vector<double> info_ratio;
            UseInformationRatio(info_ratio);
            SelectAttribute(info_ratio);
            BuildSubTree();
        }
    }

    /**
     * Calculate the expected information (information entropy)
     *
     * Let p(i) be the probability that an arbitrary tuple in D belongs to class C(i),
     * estimated by |C(i,D)|/|D|
     *
     * Expected information(entropy): Info(D) = -Sigma{ p(i) * log(p(i)) / log(2) }
     *  
     * @param[in]   factors    information table with one tuple
     * return       information_entropy calculated information entropy
     */
    double GetInformationEntropy(vector<int> &factors) {
       double denominator = 0.0;
       for (auto factor : factors) {
           denominator += factor;
       }

       double info_entropy = 0.0; 
       double probability = 0.0;
       for (auto factor : factors) {
           if (factor != 0) {
               probability = (double)factor / denominator;
               info_entropy -= probability * log(probability) / log(2);
           }
       }

       return info_entropy;
    }

    /**
     * Calculate the expected information (information entropy)
     *
     * Expected information(entropy):
     * Info(D) with A attribute = Sigma{ |D(i)|/|D| * Info(D(i)) }
     *  
     * @param[in]   table    information table with multiple tuple(class)
     * return       information_entropy calculated information entropy
     */
    double GetInformationEntropy(vector<vector<int>> &table) {
       double denominator = 0.0;
       for (auto factors : table) {
           for (auto factor : factors) {
               denominator += factor;
           }
       }

       double info_entropy = 0.0; 
       double probability = 0.0;
       for (auto factors : table) {
           probability = 0.0;
           for (auto factor : factors) {
               probability += factor; 
           }
           probability /= denominator;
           info_entropy += probability * GetInformationEntropy(factors);
       }

       return info_entropy;
    }

    /**
     * Calculate the information entropy before split the decision branches
     * Get expected information(entropy) by counting the classes of
     * attribute now being classified
     */
    void GetInfoEntropyBeforeSplit() {
        int finding_attr_idx = class_of_attr_.size() - 1;

        vector<int> info_table;
        info_table.assign(class_of_attr_[finding_attr_idx].size(), 0);

        for (auto tuple : training_data_) {
            int idx = 0;
            for (auto cls : class_of_attr_[finding_attr_idx]) {
                if (cls == tuple[finding_attr_idx]) {
                   info_table[idx]++; 
                } else {
                    idx++;
                }
            }
        }
       info_entropy_before_ = GetInformationEntropy(info_table);
    }

    /**
     * Calculate the information ratio of each candidate attribute 
     *
     * @param[out]   info_ratio    information ratio list of candidate attributes
     */
    void UseInformationRatio(vector<double> &info_ratio) {
        int finding_attr_idx = class_of_attr_.size() - 1;
        int compared_class_idx;
        int comparing_class_idx;
        for (int i = 0; i < finding_attr_idx; i++) {
            vector<vector<int>> info_table;
            double gain = 0.0;
            double ratio = 0.0;
            double split_info = 0.0;

            info_table.assign(class_of_attr_[i].size(), vector<int>());
            for (auto &tuple : info_table) {
                tuple.assign(class_of_attr_[finding_attr_idx].size(), 0);
            }

            for (auto tuple : training_data_) {
                compared_class_idx = 0;
                for (auto cls : class_of_attr_[i]) {
                   if (cls == tuple[i]) {
                      break; 
                   } else {
                       compared_class_idx++;
                   }
                }

                comparing_class_idx = 0;
                for (auto cls : class_of_attr_[finding_attr_idx]) {
                   if (cls == tuple[finding_attr_idx]) {
                      break; 
                   } else {
                       comparing_class_idx++;
                   }
                }
                info_table[compared_class_idx][comparing_class_idx]++;
            }

            // get information gain which is an index of getting information
            gain = info_entropy_before_ - GetInformationEntropy(info_table);
            // get split information for revising the distortion from info gain
            split_info = GetSplitInformation(info_table);
            // get information ratio from info gain and split info
            ratio = gain / split_info;
            info_ratio.push_back(ratio);
        }

    }

    /**
     * Calculate the split information
     *
     * Split information(entropy):
     * SplitInfo(D) with A attribute = - Sigma{ |D(i)|/|D| * log(|D(i)|/|D|) / log(2) }
     *  
     * @param[in]   table    information table with multiple tuple(class)
     * return       split_info calculated split information 
     */
    double GetSplitInformation(vector<vector<int>> &table) {
       double denominator = 0.0;
       for (auto factors : table) {
           for (auto factor : factors) {
               denominator += factor;
           }
       }

       double split_info = 0.0; 
       double probability = 0.0;
       for (auto factors : table) {
           probability = 0.0;
           for (auto factor : factors) {
               probability += factor; 
           }
           if(probability != 0) {
               probability /= denominator;
               split_info -= probability * log(probability) / log(2);
           }
       }

       return split_info;
    }

    /**
     * Select the spliiting attribute by comparing the info ratio of each candidate
     * Select the maximum information ratio from the cadidate attributes
     *
     * @param[in]   info_ratio    information ratio list of candidate attributes
     */
    void SelectAttribute(vector<double> &info_ratio) {
        double max = 0.0;
        int max_idx = 0;
        
        for (unsigned int i = 0; i < info_ratio.size(); i++) {
            if (info_ratio[i] > max) {
               max = info_ratio[i];
               max_idx = i; 
            }
        }

        attribute_ = attributes_[max_idx];
        classes_ = class_of_attr_[max_idx];
    }

    /**
     * Decide whether continue spliiting or stop
     * 3 criteria for stop spliiting:
     *  - no more attribute to split -> majority voting
     *  - all samples for a given node belong to the one same class (decision made)
     *  - no sample left 
     *
     * return       true/false    continue to split(positive) / stop to split(negative)
     */
    bool ContinueSplit() {
        int finding_attr_idx = class_of_attr_.size() - 1;
        // [stop] no more attribute to being split criteria
        if (class_of_attr_.size() < 2) {
            MajorityVote();
            is_leaf_ = true;
            return false;
        // [stop] already splitted to one side which means info entropy = 0
        } else if (info_entropy_before_ == 0) {
            attribute_ = training_data_[0][finding_attr_idx];
            is_leaf_ = true;
            return false;
        } else {
            return true;
        }
    }

    /**
     * Make the decision with majority vote by counting the tuple
     * Select the attribute with max vote
     */
    void MajorityVote() {
        int finding_attr_idx = class_of_attr_.size() - 1;
        vector<int> cnt(class_of_attr_[finding_attr_idx].size());

        int idx = 0;
        for (auto tuple : training_data_) {
            idx = 0;
            if (tuple[finding_attr_idx] == class_of_attr_[finding_attr_idx][idx] ) {
                cnt[idx]++;
            } else {
                idx++;
            }
        }

        idx = 0;
        int max = 0;
        for (unsigned int i = 0; i < cnt.size(); i++) {
            if (cnt[i] > max) {
               max = cnt[i];
               idx = i;
            }
        }

        attribute_ = class_of_attr_[finding_attr_idx][idx];
    }

    /**
     * Build subtree w.r.t already selected attribute
     * each child node would inherit the filtered training data by its branch class
     */
    void BuildSubTree() {
        int split_attr_idx = 0;
        for (auto attr : attributes_) {
            if (attribute_ == attr) {
               break; 
            } else {
                split_attr_idx++;
            }
        }

        int split_class_idx = 0;
        vector<vector<tuple_t>> training_data_per_class;
        training_data_per_class.assign(classes_.size(), vector<tuple_t> ());
        for (auto tuple : training_data_) {
            split_class_idx = 0;
            for (auto cls : classes_) {
                if (cls == tuple[split_attr_idx]) {
                    training_data_per_class[split_class_idx].push_back(tuple);
                } else {
                    split_class_idx++;
                }
            }
        }

        attributes_.erase(attributes_.begin() + split_attr_idx);
        class_of_attr_.erase(class_of_attr_.begin() + split_attr_idx);
        for (auto &training_data : training_data_per_class) {
            for (auto &tuple : training_data) {
                tuple.erase(tuple.begin() + split_attr_idx);
            }
        }

        int size = classes_.size();
        vector<int> cls_idx;
        for (int i = 0; i < size; i++) {
            // if there is no data for training, no branch(node)
            if (training_data_per_class[i].size() == 0) {
                classes_.erase(classes_.begin() + i);
            } else {
                cls_idx.push_back(i);
            }
        }

        size = cls_idx.size();
        for (int i = 0; i < size; i++) {
            branches_.push_back(DecisionTree (training_data_per_class[cls_idx[i]], attributes_, class_of_attr_));
        }
    }

    /**
     * Search the tree and decision attribute for classification
     * This function is called recursively until it reaches to the leaf(decision node)
     *
     * return   attribute       the result of classification
     */
    string SearchAttribute(tuple_t &tuple, tuple_t &test_attr) {
        if (is_leaf_) {
            return attribute_;
        } else {
            int finding_attr_idx = 0;
            for (auto attr : test_attr) {
                if (attribute_ == attr) {
                   break; 
                } else {
                    finding_attr_idx++;
                }
            }

            unsigned int finding_cls_idx = 0;
            for (auto cls : classes_) {
                if (cls == tuple[finding_attr_idx]) {
                   break; 
                } else {
                    finding_cls_idx++;
                }
            }

            // if the tuple that the model cannot classify,
            // select the first branch for decision making
            if(branches_.size() <= finding_cls_idx) {
                finding_cls_idx = 0;
            }

            return branches_[finding_cls_idx].SearchAttribute(tuple, test_attr);
        }
    }
};

void InitCLA(const char*[]);
void InitTrainingData();
void BuildDecisionTree();
void TestDecisionTree();

#endif // __ASSIGNMENT2_INCLUDE_DECISION_TREE_H__
