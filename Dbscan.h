//
// Created by sergey on 17.03.19.
//

#ifndef FLOW_READER_MYDBSCAN_H
#define FLOW_READER_MYDBSCAN_H

#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include<stack>
#include <iostream>
#include <chrono>
using namespace std::chrono;

using namespace std;

class Dbscan {

public:
    Dbscan(int minPts, double eps, std::vector<int> points) : minPts(minPts), eps(eps), points(points) {
        //auto start = high_resolution_clock::now();
        initDistances();
        //auto stop = high_resolution_clock::now();
    }
    void run();
    map<int, pair<int,int>> getLimits();
private:
    vector<int> points;
    int minPts;
    double eps;
    map<int, int> labels;
    map<pair<int,int>, double> distances;
    map<int,vector<int>> neighbors;
    void initDistances();
    vector<int> findNeighbors(int i) {
        if (neighbors.find(i) != neighbors.end()) {
            return neighbors[i];
        } else {
            return vector<int>{};
        }
    }
    void processNeighbors(int i, int ind);
};


#endif //FLOW_READER_MYDBSCAN_H
