/**
 * Perform clustering on a given data set by using DBSCAN.
 *
 * @author      Kwangil Cho
 * @since       2017-05-03
 * @file        DBSCAN.h
 */

#ifndef __ASSIGNMENT3_INCLUDE_DBSCAN_H__
#define __ASSIGNMENT3_INCLUDE_DBSCAN_H__

#include <vector>

using namespace std;

enum class ObjectType {
    NOT_DEFINED,
    CORE,
    BORDER,
    OUTLIER
};

struct object {
    double x;
    double y;

    ObjectType type;
    int cluster_num;
    std::vector<int> neighbors;
};

void InitCLA(const char *argv[]);
void InitObject();
void ShuffleVector(vector<int> &v);
void InitInputPath(const char *path);
void PrintOriginalObjects();
void DBSCAN();
void FindAllNeighbors();
void FindAllCores();
bool IsObjectRemained();
int SelectStartObject();
void MarkObjectVisted(int oid);
bool IsCorePoint(object &o);
void ExpandClusterDFS(object &center, int cluster_num);
void ExpandClusterBFS(int center, int cluster_num);
void ReorganizeCluster();
void PrintCluster();
    
#endif // __ASSIGNMENT3_INCLUDE_DBSCAN_H__
