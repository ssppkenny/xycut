//
// Created by sergey on 17.03.19.
//

#include "Dbscan.h"

void Dbscan::initDistances() {
    for (int i = 0; i < points.size(); i++) {
        labels[i] = 0;

        int ind = i;
        while (ind < points.size() && abs(points[ind] - points[i]) < eps) {
            ind++;
            if (abs(points[ind] - points[i]) < eps) {
                if (ind < points.size()) {
                    processNeighbors(i, ind);
                }
            }
        }
        ind = i;
        while (ind >= 0 && abs(points[ind] - points[i]) < eps) {
            ind--;
            if (abs(points[ind] - points[i]) < eps) {
                if (ind >= 0) {
                    processNeighbors(i, ind);
                }
            }
        }
    }
}

void Dbscan::processNeighbors(int i, int ind) {
    if (neighbors.find(i) != neighbors.end()) {
        vector<int> nbs = neighbors[i];
        nbs.push_back(ind);
        neighbors[i] = nbs;
    } else {
        vector<int> nbs = {ind};
        neighbors[i] = nbs;
    }
}

void Dbscan::run() {
    int cluster = 0;
    for (int i = 0; i < points.size(); i++) {
        if (labels[i] != 0) {
            continue;
        }
        vector<int> nbs = findNeighbors(i);
        if (nbs.size() < minPts) {
            labels[i] = -1;
            continue;
        }
        cluster += 1;
        labels[i] = cluster;

        stack<int> stack;
        for (int j = 0; j < nbs.size(); j++) {
            stack.push(nbs[j]);
        }

        while (!stack.empty()) {
            int j = stack.top();
            stack.pop();

            if (labels[j] == -1) {
                labels[j] = cluster;
            }
            if (labels[j] != 0) {
                continue;
            }
            labels[j] = cluster;
            vector<int> newNbs = findNeighbors(j);
            if (newNbs.size() >= minPts) {
                for (int k = 0; k < newNbs.size(); k++) {
                    stack.push(newNbs[k]);
                }
            }
        }
    }
}

map<int, pair<int, int>> Dbscan::getLimits() {
    map<int, pair<int, int>> limits;
    for (auto it = labels.begin(); it != labels.end(); it++) {
        int cluster = it->second;
        int lineNo = points[it->first];
        if (limits.find(cluster) == limits.end()) {
            limits[cluster] = make_pair(lineNo, lineNo);
        } else {
            pair<int, int> current = limits[cluster];
            if (current.first > lineNo) {
                current.first = lineNo;
            }
            if (current.second < lineNo) {
                current.second = lineNo;
            }
            limits[cluster] = current;
        }
    }
    return limits;
}
