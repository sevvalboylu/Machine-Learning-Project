//
//  ClusterLinkedList.hpp
//  agglomerative_last_version
//
//  Created by Bahadır on 28.07.2017.
//  Copyright © 2017 Bahadir. All rights reserved.
//

#ifndef ClusterLinkedList_hpp
#define ClusterLinkedList_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "strutils.h"

using namespace std;


struct timeNode;

//
// This node represents the processes we send to the servers.
// Between them, we have timeNodes which contain the iteration time.
// For visual representation check the LinkedCluster.png
//
struct clusterNode {
    int cluster_id;
    int label;
    timeNode * right;
    timeNode * left;
    
    clusterNode () // default constructor
    :cluster_id(0), right(NULL), left(NULL), label(-1)
    {}
    
    clusterNode(int id) // constructor
    :cluster_id(id), right(NULL), left(NULL), label(-1)
    {}
};


struct timeNode {
    int time;
    clusterNode * right;
    clusterNode * left;
    
    timeNode(int ms)
    :time(ms), right(NULL), left(NULL)
    {}
    
};





class LinkedClusters {
    
    int node_count;
    
    
public:
    
    LinkedClusters();                    // Default Constructor
    LinkedClusters(string datafile);     // Constructor
    ~LinkedClusters();                   // Destructor
    
    int find_min_time();                 // Finds min timeNode. Returns the id of the previous clusterNode.
    
    
    // cluster_num --> the number of clusters needed.
    
    void complete_link_cluster(int cluster_num); //complete linkage: dist(A,B) = max(dist(a,b) : a in A, b in B)

    void single_link_cluster(int cluster_num);   // single linkage: dist(A,B) = min(dist(a,b) : a in A, b in B).
    
    void average_link_cluster(int cluster_num);  // unweighted average linkage: dist(A,B) = average(dist(a,b) : a in A, b in B)

    void print(); // prints the linked list
    
    void labeler(vector<int> labels); //
        
    void write_to_file(string filename, int size);
    
    
    
    
    
    /*
    
     void calculate_percentage(string file_name); // Calculates the percentage(equivalent to 'cluster area') writes them to files. Human interaction needed. Check the predicted values. Assign appropriate label.
    
     */
    
    
    void confusion_matrix(string filename);
    
private:
    clusterNode* headPtr;
    
};

#endif /* ClusterLinkedList_hpp */








