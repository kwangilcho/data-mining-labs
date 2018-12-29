/**
 * Perform clustering on a given data set by using DBSCAN.
 *
 * @author      Kwangil Cho
 * @since       2017-05-03
 * @file        DBSCAN.cc
 */

#include "DBSCAN.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <queue>
#include <cmath>
#include <climits>
#include <ctime>

fstream  g_fs_input; // input file stream
ofstream g_fs_output; // output file stream
string g_input_file_name; // input file name
int g_num_cluster; // number of cluster that has been formed
vector<struct object> g_objects; // all input objects
vector<int> g_unvisited_objs; // unvisited objects
vector<bool> g_valid_clusters; // valid clusters for reorganization

struct {
    int num_clusters; // expected number of clusters
    int Eps; // distance for finding neighbors
    int MinPts; // minimum number of neighbor points to be a core point
} g_cluster_config; 

/**
 * Calculate the distance between two input objects
 *
 * @param[in]   object a, b
 * @return      distance
 */
inline double Distance(object &a, object &b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

/**
 * Determine if the target object is reachable from ceneter object 
 * Reachable decision: distance between center and target object is less than or equal to epsilon
 *
 * @param[in]   center, target object
 * @return      whether reachable or not
 */
inline bool IsReachable(object &center, object &target) {
    return Distance(center, target) <= g_cluster_config.Eps; 
}

/**
 * @param[in]   command line arguments as below:
 *              argv[0] executable file
 *              argv[1] input data file
 *              argv[2] n: number of clusters
 *              argv[3] Eps: maximum radius of the neighborhood
 *              argv[4] MinPts: minimum number of points 
 *                              in an Eps-neighborhood of a given point
 * @return      0
 */
int main(int argc, const char *argv[]) {
    if (argc == 5) {
        InitCLA(argv);
        InitObject();
        PrintOriginalObjects();
        DBSCAN();
        ReorganizeCluster();
        PrintCluster();
    } else {
        cout << "Invalid usage. Please check the arguments." << endl
           << "$ [executable file] [input data file] [number of clusters] [Eps] [MinPts]" << endl;
    }

    return 0;
}

/**
 * Open in/out file with Command Line Arguments
 * Configuration setup
 *
 * @param[in]   argv   input command line arguments 
 */
void InitCLA(const char *argv[]) {
    // open input data file
    g_fs_input.open(argv[1], ifstream::in);
    if (g_fs_input.is_open() == false) {
        cout << "Can't open input data file. Program terminated." << endl;
        exit(0);
    }

    // initialize input file name from path
    InitInputPath(argv[1]);

    // setup the configuration value
    g_cluster_config.num_clusters = atoi(argv[2]);
    g_cluster_config.Eps = atoi(argv[3]);
    g_cluster_config.MinPts = atoi(argv[4]);

    cout << "input file path: " << argv[1] << " ";
    cout << "num_clusters: " << g_cluster_config.num_clusters << " ";
    cout << "Eps: " << g_cluster_config.Eps << " ";
    cout << "MinPts: " << g_cluster_config.MinPts << endl;
}

/**
 * Parse the input file path and get the name of file for output prefix
 *
 * @param[in]   path  path of input file 
 */
void InitInputPath(const char *path) { size_t pos;
    string delimiter = "/";
    g_input_file_name = path;

    // erase the pre-path with delimiter '/'
    while ((pos = g_input_file_name.find(delimiter)) != string::npos) {
        g_input_file_name.erase(0, pos + delimiter.length());
    }

    // erase the extension which is behind the delimiter '.'
    if ((pos = g_input_file_name.find('.')) != string::npos) {
        g_input_file_name.erase(pos, g_input_file_name.length());
    }
}

/**
 * Initialize the objects' x, y coordinate information from input file
 *
 * @param[in]   path  path of input file 
 */
void InitObject() {
    int oid;
    object input_object;
    input_object.type = ObjectType::NOT_DEFINED;

    // get the object data from the input file
    while(g_fs_input >> oid) {
        g_fs_input >> input_object.x >> input_object.y;
        g_unvisited_objs.push_back(oid);
        g_objects.push_back(input_object);
    }

    // shuffle the unvisited object vector
    ShuffleVector(g_unvisited_objs);
    g_fs_input.close();
}

/**
 * Shulffle the unvisited object vector for start point random pick
 *
 * @param[in]   unvisited object vector
 */
void ShuffleVector(vector<int> &v) {
    srand(time(NULL));
    int size = v.size();
    int rnd_idx;
    int tmp;
    for (int i = size - 1; i > 0; i--) {
        rnd_idx = rand() % (i + 1);
        tmp = v[rnd_idx];
        v[rnd_idx] = v[i];
        v[i] = tmp;
    }
}

/**
 * Print the original objects to output file path before clustering 
 */
void PrintOriginalObjects() {
    string path_base = "./data/output/";
    string path = path_base + g_input_file_name + "_original.txt";
    g_fs_output.open(path, ifstream::out);
    if (g_fs_output.is_open() == false) {
        cout << "Can't open output data file. Program terminated." << endl;
        exit(0);
    }
    for (auto obj : g_objects) {
        g_fs_output << obj.x << "\t" << obj.y << "\t" << endl;
    }
    g_fs_output.close();
}

/**
 * Density-based spatial clustering of applications with noise (DBSCAN)  
 * 1. Pick the start point which can form a new cluster 
 * 2. if the start point is a core point, expand the cluster
 * 3. if it is not a start point, which means its number of neighbor is not sufficient,
 *    the point becomes outlier at first(the state can be changed to border in future).
 * 4. Repeat the step 1~3 while there is no object remained.
 */
void DBSCAN() {
    // retrieve all reachable points from point p w.r.t Eps
    FindAllNeighbors();
    // find all core points with point's number of neighbors
    FindAllCores();
    while(IsObjectRemained()) {
        // select a start point p randomly
        int p = SelectStartObject();

        // if the point is core, expand the cluster
        if(IsCorePoint(g_objects[p])) {
            g_objects[p].type = ObjectType::CORE;
            g_objects[p].cluster_num = g_num_cluster;
            ExpandClusterDFS(g_objects[p], g_num_cluster++);
            // ExpandClusterBFS(p, g_num_cluster++);
        } else {
            // if not, mark as outlier
            g_objects[p].type = ObjectType::OUTLIER;
        }
    }
}

/**
 * Retrieve all reachable points(neighbors) from each point
 */
void FindAllNeighbors() {
    int end = g_objects.size();
    for (int i = 0; i < end; i++) {
        for (int j = 0; j < end; j++) {
            if (IsReachable(g_objects[i], g_objects[j])) {
               g_objects[i].neighbors.push_back(j); 
            }
        }
    }
}

/**
 * Retrieve all core points and print out their x, y coordinate
 */
void FindAllCores() {
    string path_base = "./data/output/";
    string path = path_base + g_input_file_name + "_cores.txt";
    g_fs_output.open(path, ifstream::out);
    if (g_fs_output.is_open() == false) {
        cout << "Can't open output data file. Program terminated." << endl;
        exit(0);
    }
    for (auto obj : g_objects) {
        if (IsCorePoint(obj)) {
           g_fs_output << obj.x << "\t" << obj.y << "\t" << endl;
        }
    }
    g_fs_output.close();
}

/**
 * Determine whether there unvisited objects exist or not
 */
bool IsObjectRemained() {
    return g_unvisited_objs.size() > 0;
}

/**
 * Pick the one object for clustering start point from shuffled obeject vector 
 */
int SelectStartObject() {
    int oid = g_unvisited_objs[0];
    MarkObjectVisted(oid);
    return oid;
}

/**
 * Remove given object id from unvisited object vector 
 *
 * @param[in]  oid   object id to be removed 
 */
void MarkObjectVisted(int oid) {
    int idx = 0;
    for (auto obj : g_unvisited_objs) {
        if (obj == oid) {
            g_unvisited_objs.erase(g_unvisited_objs.begin() + idx);
            break;
        } else {
           idx++; 
        }
    }
}

/**
 * Determine whether the object is core or not
 *
 * @param[in]  examined object 
 */
bool IsCorePoint(object &o) {
    return (int)o.neighbors.size() >= g_cluster_config.MinPts;
}

/**
 * Expand cluster with DFS approach
 * All the expanded objects are same cluster
 *
 * @param[in]  center           new core point of expanding cluster 
 * @param[in]  cluster_num      cluster number 
 */
void ExpandClusterDFS(object &center, int cluster_num) {
    // check the neighbor objects as it is core
    for (auto neighbor : center.neighbors) {
        // if it is already classified as core, skip
        if(g_objects[neighbor].type != ObjectType::CORE) {
            MarkObjectVisted(neighbor);
            g_objects[neighbor].cluster_num = cluster_num;

            // if it is a core object,
            // expand the cluster with same cluster number recursively
            if(IsCorePoint(g_objects[neighbor])) {
                g_objects[neighbor].type = ObjectType::CORE;
                ExpandClusterDFS(g_objects[neighbor], cluster_num);
            } else {
                // if it is not a core, it is border. (could be outlier)
                g_objects[neighbor].type = ObjectType::BORDER;
            }
        }
    }
}

/**
 * Expand cluster with BFS approach
 * All the expanded objects are same cluster
 *
 * @param[in]  center           new core point of expanding cluster 
 * @param[in]  cluster_num      cluster number 
 */
void ExpandClusterBFS(int center, int cluster_num) {
    int p = center;
    std::queue<int> queue;
    
    queue.push(p);
    while (!queue.empty()) {
        p = queue.front();
        queue.pop();

        // check the neighbor objects as it is core
        for (auto neighbor : g_objects[p].neighbors) {
            // if it is already classified as core, skip
            if(g_objects[neighbor].type != ObjectType::CORE) {
                MarkObjectVisted(neighbor);
                g_objects[neighbor].cluster_num = cluster_num;

                // if it is a core object,
                // expand the cluster with same cluster number
                if(IsCorePoint(g_objects[neighbor])) {
                    g_objects[neighbor].type = ObjectType::CORE; queue.push(neighbor);
                } else {
                    // if it is not a core, it is border. (could be outlier)
                    g_objects[neighbor].type = ObjectType::BORDER;
                }
            }
        }
    }
}

/**
 * Reorganize the cluster to expected number of cluster
 * 0. If the expected number of cluster is smaller than DBSCAN found, do this routine
 * 1. Sort the clusters in descending order
 * 2. Calculate how many cluster should merge
 * 3. From the smallest cluster, merge the cluster into the nearest cluster
 *    here 'nearest cluster' means the closest distance between centroids
 * 4. After merging, change the cluster number from old one to new one
 */
void ReorganizeCluster() {
    // calculate how many cluster would be merged
    int mergeable_cnt = g_num_cluster - g_cluster_config.num_clusters;
    g_valid_clusters.assign(g_num_cluster, bool(true));
    // if there is a mergeable cluster
    if (mergeable_cnt > 0) {
        struct cluster {
            int size = 0;
            double center_x = 0.0;
            double center_y = 0.0;
        };

        std::vector<struct cluster> clusters(g_num_cluster);

        int cluster_num;
        // calculate the centorid of each cluster
        for (auto obj : g_objects) {
            cluster_num = obj.cluster_num;
            clusters[cluster_num].size++;
            clusters[cluster_num].center_x += obj.x;
            clusters[cluster_num].center_y += obj.y;
        }

        for (int i = 0; i < g_num_cluster; i++) {
            clusters[i].center_x /= clusters[i].size;
            clusters[i].center_y /= clusters[i].size;
        }

        // sort the clusters in descending order and find out merge target
        for (int i = 0; i < mergeable_cnt; i++) {
            int min_idx;
            int min = INT_MAX;
            int idx = 0;
            for (auto &cluster : clusters) {
                if (g_valid_clusters[idx] && min > cluster.size) {
                    min = cluster.size;
                    min_idx = idx;
                } 
                idx++;
            }
            g_valid_clusters[min_idx] = false;
        }

        int src_idx = 0;
        for (auto src_cluster : clusters) {
            if (g_valid_clusters[src_idx] == false) {
                int min_idx;
                int min = INT_MAX;
                int dst_idx = 0;

                // for the target cluster, find out the nearest cluster to be 
                for (auto dst_cluster : clusters) {
                    double dst = sqrt((src_cluster.center_x - dst_cluster.center_x) 
                            * (src_cluster.center_x - dst_cluster.center_x) 
                            + (src_cluster.center_y - dst_cluster.center_y) 
                            * (src_cluster.center_y - dst_cluster.center_y));
                    if (g_valid_clusters[dst_idx] && min != 0 && min > dst) {
                        min = dst;
                        min_idx = dst_idx;
                    } 
                    dst_idx++;
                }

                // change the cluster number to new one 
                for (auto &obj : g_objects) {
                    if (obj.cluster_num == src_idx) {
                        obj.cluster_num = min_idx;
                    }
                }
            }
            src_idx++;
        }
        g_num_cluster = g_cluster_config.num_clusters;
    }
}

/**
 * Print clustered object data to output path
 * 1. Print x, y coordinate for each cluster
 * 2. Print object id for each cluster
 */
void PrintCluster() {
    ofstream fs_cluster_id;
    int i = 0;
    int idx = 0;
    string path_base = "./data/output/";
    for (auto cluster : g_valid_clusters) {
        if (cluster) {
            string path = path_base + g_input_file_name + "_xy_" + to_string(i) + ".txt";
            g_fs_output.open(path, ifstream::out);
            if (g_fs_output.is_open() == false) {
                cout << "Can't open output data file. Program terminated." << endl;
                exit(0);
            }
            path = path_base + g_input_file_name + "_cluster_" + to_string(i) + ".txt";
            fs_cluster_id.open(path, ifstream::out);
            if (fs_cluster_id.is_open() == false) {
                cout << "Can't open output data file. Program terminated." << endl;
                exit(0);
            }

            int size = g_objects.size();
            for (int j = 0; j < size; j++) {
                if (g_objects[j].cluster_num == idx && g_objects[j].type != ObjectType::OUTLIER) {
                   g_fs_output << g_objects[j].x << "\t" << g_objects[j].y << "\t" << endl;
                   fs_cluster_id << j << endl;
                }
            }
            i++;
            g_fs_output.close();
            fs_cluster_id.close();
        }
        idx++;
    }
    cout << "output file path: " << path_base << endl;
}
