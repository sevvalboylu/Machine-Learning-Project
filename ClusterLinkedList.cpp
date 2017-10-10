//
//  ClusterLinkedList.cpp
//  agglomerative_last_version
//
//  Created by Bahadır on 28.07.2017.
//  Copyright © 2017 Bahadir. All rights reserved.
//
//-------------------------------------------------------------------------------


#include "ClusterLinkedList.hpp"

// Default Constructor
//
LinkedClusters::LinkedClusters() {
    
    headPtr = new clusterNode;
}


//-------------------------------------------------------------------------------


// Constructor
// Detailed visualization is in the folder. Check 'LinkedCluster.png'
//
LinkedClusters::LinkedClusters(string datafile) {
    
    ifstream data;
    data.open(datafile);
    string line;
    
    if (data.is_open()) {
        cout << "Dosya basarili sekilde acildi!\n";
        data.clear();
        data.seekg(0, ios::beg);
        
        headPtr = new clusterNode;
        
        int id_counter = 0;
        
        clusterNode * clusterTracker;
        
        clusterTracker = headPtr;
        
        while(getline(data, line)) {
            
            id_counter++;
            
            clusterTracker->right = new timeNode(atoi(line));
            clusterTracker->right->left = clusterTracker;
            clusterTracker->right->right = new clusterNode(id_counter);
            clusterTracker->right->right->left = clusterTracker->right;
            
            clusterTracker = clusterTracker->right->right;
            
        }
        
        // Last node is not exist so deleting it.
        
        clusterTracker->left->left->right = NULL;
        
        delete clusterTracker->left;
        delete clusterTracker;
        
        node_count = --id_counter;
        printf("%d tane deger eklendi\n\n\n\n", id_counter);
    }
    data.close();
}


//-------------------------------------------------------------------------------


// Destructor
//
LinkedClusters::~LinkedClusters() {
    
    clusterNode * clusterKiller;
    
    clusterKiller = headPtr;
    
    while(clusterKiller->right != NULL) {
        
        clusterKiller = clusterKiller->right->right;
        delete clusterKiller->left->left;
        delete clusterKiller->left;
    }
    
    delete clusterKiller;
    cout << "Destruction tamamlandi!" << endl;
}


//-------------------------------------------------------------------------------


// Find min time. Return the id of the previous clusterNode.
//
int LinkedClusters::find_min_time() {
    
    int min = INT_MAX;
    int id = 0;
    clusterNode * clusterTracker = headPtr;
    
    while(clusterTracker->right != NULL) {
        
        if (clusterTracker->right->time < min) {
            min = clusterTracker->right->time;
            id = clusterTracker->cluster_id;
        }
        
        clusterTracker = clusterTracker->right->right;
    }
    
    //printf("\nMinimum time: %d\n",min);
    //printf("id: %d\n\n", id);
    
    
    return id;
}


//-------------------------------------------------------------------------------

//---------------//
// COMPLETE LINK //
//---------------//
//
// Check header file to understand what complete linkage clustering is.
//
void LinkedClusters::complete_link_cluster(int cluster_num) {
    
    cout << "----------------------------\n";
    cout << "Complete Clustering started!\n\n";
    cout << "Complete Clustering in process...\n\n";
    
    int iter = node_count - cluster_num;
    
    for (int i = 0; i < iter; i++) {
        
        int id = find_min_time();
        clusterNode * clusterTracker = headPtr;
        
        // get to the clusterNode next to the min timeNode
        //
        while (clusterTracker->cluster_id != id) {
            clusterTracker = clusterTracker->right->right;
        }
        
        // Three condition...
        //
        // Insert the new clustered node
        //
        clusterNode * new_clusterPtr = new clusterNode(id);
        
        if (clusterTracker->right->right->right == NULL) { // Cluster at the end of the list.
            
            clusterTracker->left->time += clusterTracker->right->time;
            clusterTracker->left->right = new_clusterPtr;
            new_clusterPtr->left = clusterTracker->left;
            new_clusterPtr->right = NULL;
            
            delete clusterTracker->right->right;
            delete clusterTracker->right;
            delete clusterTracker;
            
        }
        else if (clusterTracker->left == NULL) { // Cluster at the beggining of the list.
            
            clusterTracker->right->right->right->time += clusterTracker->right->time;
            
            new_clusterPtr->left = NULL;
            new_clusterPtr->right = clusterTracker->right->right->right;
            
            new_clusterPtr->right->left = new_clusterPtr;
            
            headPtr = new_clusterPtr;
            
            delete clusterTracker->right->right;
            delete clusterTracker->right;
            delete clusterTracker;
            
            // Change id
            
            clusterTracker = new_clusterPtr->right->right;
            
            while (clusterTracker->right != NULL) {
                clusterTracker->cluster_id --;
                clusterTracker = clusterTracker->right->right;
            }
            
            clusterTracker->cluster_id --;
            
        }
        else {
            
            //add time
            clusterTracker->left->time += clusterTracker->right->time;
            clusterTracker->right->right->right->time += clusterTracker->right->time;
            
            clusterTracker->left->right = new_clusterPtr;
            new_clusterPtr->left = clusterTracker->left;
            
            clusterTracker->right->right->right->left = new_clusterPtr;
            new_clusterPtr->right = clusterTracker->right->right->right;
            
            delete clusterTracker->right->right;
            delete clusterTracker->right;
            delete clusterTracker;
            
            // Change id
            
            clusterTracker = new_clusterPtr->right->right;
        
            
            while (clusterTracker->right != NULL) {
                clusterTracker->cluster_id --;
                clusterTracker = clusterTracker->right->right;
            }
            
            clusterTracker->cluster_id --;
            
        }
        
    
    }
    
    node_count = cluster_num;
    cout << "Complete Clustering completed! \n";
    cout << "------------------------------\n\n";
}


//-------------------------------------------------------------------------------


//-------------//
// SINGLE LINK //
//-------------//
//
// Check header file to see what single linkage clustering is.
//
void LinkedClusters::single_link_cluster(int cluster_num) {
    
    cout << "--------------------------\n";
    cout << "Single Clustering started!\n\n";
    
    cout << "Single Clustering in process...\n\n";
    
    
    int iter = node_count - cluster_num;
    
    for (int i = 0; i < iter; i++) {
        
        int id = find_min_time();
        clusterNode * clusterTracker = headPtr;
        
        // get to the clusterNode next to the min timeNode
        //
        while (clusterTracker->cluster_id != id) {
            clusterTracker = clusterTracker->right->right;
        }
        
        // Three condition...
        //
        // Insert the new clustered node
        //
        clusterNode * new_clusterPtr = new clusterNode(id);
        
        if (clusterTracker->right->right->right == NULL) { // Cluster at the end of the list.
            
            clusterTracker->left->right = new_clusterPtr;
            new_clusterPtr->left = clusterTracker->left;
            new_clusterPtr->right = NULL;
            
            delete clusterTracker->right->right;
            delete clusterTracker->right;
            delete clusterTracker;
            
        }
        else if (clusterTracker->left == NULL) { // Cluster at the beggining of the list.
            
            
            new_clusterPtr->left = NULL;
            new_clusterPtr->right = clusterTracker->right->right->right;
            
            new_clusterPtr->right->left = new_clusterPtr;
            
            headPtr = new_clusterPtr;
            
            delete clusterTracker->right->right;
            delete clusterTracker->right;
            delete clusterTracker;
            
            // Change id
            
            clusterTracker = new_clusterPtr->right->right;
            
            while (clusterTracker->right != NULL) {
                clusterTracker->cluster_id --;
                clusterTracker = clusterTracker->right->right;
            }
            
            clusterTracker->cluster_id --;
            
        }
        else {
            
            clusterTracker->left->right = new_clusterPtr;
            new_clusterPtr->left = clusterTracker->left;
            
            clusterTracker->right->right->right->left = new_clusterPtr;
            new_clusterPtr->right = clusterTracker->right->right->right;
            
            delete clusterTracker->right->right;
            delete clusterTracker->right;
            delete clusterTracker;
            
            // Change id
            
            clusterTracker = new_clusterPtr->right->right;
            
            
            while (clusterTracker->right != NULL) {
                clusterTracker->cluster_id --;
                clusterTracker = clusterTracker->right->right;
            }
            
            clusterTracker->cluster_id --;
            
        }
        
        
    }
    
    node_count = cluster_num;
    cout << "Single Clustering completed! \n";
    cout << "----------------------------\n\n";
    
    
    
}


//-------------------------------------------------------------------------------

//--------------//
// AVERAGE LINK //
//--------------//
//
// Check header file to understand what average linkage clustering is.
//
void LinkedClusters::average_link_cluster(int cluster_num) {
        
        cout << "---------------------------\n";
        cout << "Average Clustering started!\n\n";
        cout << "Average Clustering in process...\n\n";
    
        int iter = node_count - cluster_num;
        
        for (int i = 0; i < iter; i++) {
            
            int id = find_min_time();
            clusterNode * clusterTracker = headPtr;
            
            // get to the clusterNode next to the min timeNode
            //
            while (clusterTracker->cluster_id != id) {
                clusterTracker = clusterTracker->right->right;
            }
            
            // Three condition...
            //
            // Insert the new clustered node
            //
            clusterNode * new_clusterPtr = new clusterNode(id);
            
            if (clusterTracker->right->right->right == NULL) { // Cluster at the end of the list.
                
                
                clusterTracker->left->time += (clusterTracker->right->time)/2;
                clusterTracker->left->right = new_clusterPtr;
                new_clusterPtr->left = clusterTracker->left;
                new_clusterPtr->right = NULL;
                
                delete clusterTracker->right->right;
                delete clusterTracker->right;
                delete clusterTracker;
                
            }
            else if (clusterTracker->left == NULL) { // Cluster at the beggining of the list.
                
                clusterTracker->right->right->right->time += (clusterTracker->right->time)/2;
                
                new_clusterPtr->left = NULL;
                new_clusterPtr->right = clusterTracker->right->right->right;
                
                new_clusterPtr->right->left = new_clusterPtr;
                
                headPtr = new_clusterPtr;
                
                delete clusterTracker->right->right;
                delete clusterTracker->right;
                delete clusterTracker;
                
                // Change id
                
                clusterTracker = new_clusterPtr->right->right;
                
                while (clusterTracker->right != NULL) {
                    clusterTracker->cluster_id --;
                    clusterTracker = clusterTracker->right->right;
                }
                
                clusterTracker->cluster_id --;
                
            }
            else {
                
                //add time
                clusterTracker->left->time += (clusterTracker->right->time)/2;
                clusterTracker->right->right->right->time += (clusterTracker->right->time)/2;
                
                clusterTracker->left->right = new_clusterPtr;
                new_clusterPtr->left = clusterTracker->left;
                
                clusterTracker->right->right->right->left = new_clusterPtr;
                new_clusterPtr->right = clusterTracker->right->right->right;
                
                delete clusterTracker->right->right;
                delete clusterTracker->right;
                delete clusterTracker;
                
                // Change id
                
                clusterTracker = new_clusterPtr->right->right;
                
                
                while (clusterTracker->right != NULL) {
                    clusterTracker->cluster_id --;
                    clusterTracker = clusterTracker->right->right;
                }
                
                clusterTracker->cluster_id --;
                
            }
            
            
        }
    
        node_count = cluster_num;
        cout << "Average Clustering completed! \n";
        cout << "-----------------------------\n\n";
    
}


//-------------------------------------------------------------------------------


void LinkedClusters::print(){
    
    clusterNode * clusterTracker = headPtr;
    
    while (clusterTracker->right != NULL) {
        
        cout << "|" << clusterTracker->cluster_id << "|" << endl << endl;
        cout << "-(" << clusterTracker->right->time << ")-" << endl << endl;

        
        clusterTracker = clusterTracker->right->right;
    }
    
    cout << "-|" << clusterTracker->cluster_id << "|-" << endl << endl;
    
}


void LinkedClusters::labeler(vector<int> labels) {
    
    clusterNode * clusterTracker = headPtr;
    
    for (int i = 0; i < labels.size(); i++) {
        
        clusterTracker->label = labels[i];
        clusterTracker = clusterTracker->right->right;
        
    }
    
    
}


void LinkedClusters::write_to_file(string filename, int size) {
    
    ofstream file;
    file.open(filename);
    // Store the percentage data
    vector<int> store_percentage(size);
    
    if (file.is_open()) {
        cout << filename << " is opened succesfully!" << endl;
        timeNode * timeTracker = headPtr->right;
        
        int total = 0;
        
        while (timeTracker != NULL) {
            // Total time;
            total += timeTracker->time;
            // Pointer moves like Jagger...
            timeTracker =timeTracker->right->right;
        }
        
        timeTracker = headPtr->right;
        int count = 0;
        
        
        
        while (timeTracker != NULL) {
            
            double in_thousand = ((double)timeTracker->time / (double)total)*1000 + 0.5; //!!!
            
            store_percentage[count] = in_thousand;
            
            cout << count << ". node's percentage(in 1000): ";
            printf("%.f ", in_thousand);
            cout << endl;
            count++;
            timeTracker = timeTracker->right->right;
            
        }
        
        
        // Let's write it to the file!
        
        clusterNode * clusterTacker = headPtr;
        
        int k = 0;
        
        while(clusterTacker->right != NULL) {
            
            for (int i = 0; i < store_percentage[k]; i++) {
                
                file << "C" + itoa(clusterTacker->label) << endl;
                
            }
            
            k++;
            clusterTacker = clusterTacker->right->right;
            
        }
        
        
        for (int i = 0; i < store_percentage[k]; i++) {
            
            file << "C" + itoa(clusterTacker->label) << endl;
            
        }
        
        cout << "Jobs done!!!" << endl;
        
        
    }
    
    file.close();
}




// Old percentage calculator
// Works with 5 cluster
/*
 
void LinkedClusters::calculate_percentage(string file_name) {
    
    clusterNode * clusterTracker = headPtr;
    
    int total = 0;
    
    double p1, p2, p3, p4, p5;
    
    
    while (clusterTracker->right != NULL) {
        total += clusterTracker->right->time;
        clusterTracker = clusterTracker->right->right;
    }
    
    clusterTracker = headPtr;
    
    p1 = ((double)clusterTracker->right->time / (double)total);
    clusterTracker = clusterTracker->right->right;
    
    p2 = ((double)clusterTracker->right->time / (double)total);
    clusterTracker = clusterTracker->right->right;
    
    p3 = ((double)clusterTracker->right->time / (double)total);
    clusterTracker = clusterTracker->right->right;
    
    p4 = ((double)clusterTracker->right->time / (double)total);
    clusterTracker = clusterTracker->right->right;
    
    p5 = ((double)clusterTracker->right->time / (double)total);
    
    printf("Total added time: %d\n\n", total);
    
    printf("Cluster 1: %.3f %s \n", p1, "%");
    printf("Cluster 2: %.3f %s \n", p2, "%");
    printf("Cluster 3: %.3f %s \n", p3, "%");
    printf("Cluster 4: %.3f %s \n", p4, "%");
    printf("Cluster 5: %.3f %s \n", p5, "%");
    
    int pi1, pi2, pi3, pi4, pi5;
    
    pi1 = (int) (p1*1000+0.5);
    pi2 = (int) (p2*1000+0.5);
    pi3 = (int) (p3*1000+0.5);
    pi4 = (int) (p4*1000+0.5);
    pi5 = (int) (p5*1000+0.5);
    
    cout << "\n------------------------";
    cout << "\nC?,  C?,  C?,  C?,  C?";
    printf("\n%d, %d, %d, %d, %d\n", pi1, pi2, pi3, pi4, pi5);
    cout << "--------------------------\n\n";
    
    ofstream predictFile;
    predictFile.open(file_name);
    
    if (predictFile.is_open()) {
        
        // Labels are assinged by human interaction.
        // If necessary, it could have found by checking the labels from the predictAll file.
        //
        for (int i = 0; i < pi1; i++) {
            predictFile << "C3\n";
        }
        
        for (int i = 0; i < pi2; i++) {
            predictFile << "C4\n";
        }
        
        for (int i = 0; i < pi3; i++) {
            predictFile << "C5\n";
        }
        
        for (int i = 0; i < pi4; i++) {
            predictFile << "C3\n";
        }
        
        for (int i = 0; i < pi5; i++) {
            predictFile << "C5\n";
        }
        
    }
    
    predictFile.close();
    
}

 */

void LinkedClusters::confusion_matrix(string filename) {
    
    ofstream confusion;
    confusion.open("confisuon_matrix.txt");
    
    ifstream real_label;
    real_label.open("min_label.txt");
    
    ifstream predict;
    predict.open(filename);
    
    if (real_label.is_open() && predict.is_open()) {
        
        cout << "files opened successfully.\n";
        
        string liner, linep;
        
        vector<vector<int>> conf_matrix(5,vector<int>(5,0));
        
        int p,r;
        int correct = 0;
        
        cout << "P  |  R" << endl;
        int line_num = 1;
        while (getline(real_label, liner) && getline(predict, linep)) {
            
            
            
            p = (int)linep[1]-49;
            r = (int)liner[1]-49;
            
            conf_matrix[p][r]++;
            
            if (linep == liner) {
                correct++;
                cout << linep << " | " << liner << " .. "<< "-1- " << line_num++ << endl;
            }
            
            else {
                cout << linep << " | " << liner << " .. "<< "-0- " << line_num++ << endl;
            }
            
            
        }
        
        cout << "Correct: " << correct << endl;
        cout << "Confusion Matrix: " << endl << endl;
        
        for (int i = 0; i < conf_matrix.size(); i++) {
            for (int k = 0; k < conf_matrix[0].size(); k++) {
                
                cout << conf_matrix[i][k] << "\t";
                confusion << conf_matrix[i][k] << "\t";
            }
            cout << endl << endl;
            confusion << endl;
        }
        
    }
    
    
}















