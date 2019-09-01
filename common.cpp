//
// Created by sergey on 01.09.19.
//
#include "common.h"


int max_ind(std::vector<std::tuple<int,int>> zr, double threshold) {
    std::vector<std::tuple<int,int>> gaps;
    for (int i=0;i<zr.size();i++) {
        int gap = std::get<1>(zr.at(i)) - std::get<0>(zr.at(i));
        if (gap > threshold) {
            gaps.push_back(std::make_tuple(i, std::get<1>(zr.at(i)) - std::get<0>(zr.at(i))));
        }
    }
    int max = -1;
    int maxind = -1;
    if (gaps.size() > 0) {
        for (int i=0;i<gaps.size();i++) {
            int ind = std::get<0>(gaps.at(i));
            int gap = std::get<1>(gaps.at(i));

            if (gap > max){
                max = gap;
                maxind = ind;
            }
        }
        return maxind;
    } else {
        return -1;
    }
}

std::vector<std::tuple<int,int>> zero_runs_hor(const cv::Mat& hist) {
    int w = hist.cols;

    std::vector<std::tuple<int, int>> return_value;

    int pos = 0;
    for (int i = 0; i < w; i++) {
        if ((i == 0 && hist.at<float>(0, i) == 0) ||
            (i > 0 && hist.at<float>(0, i) == 0 && hist.at<float>(0,i - 1) > 0)) {
            pos = i;
        }

        if ((i == w - 1 && hist.at<float>(0,i) == 0) ||
            (i < w - 1 && hist.at<float>(0,i) == 0 && hist.at<float>(0,i + 1) > 0)) {
            return_value.push_back(std::make_tuple(pos, i));
        }
    }
    return return_value;
}

std::vector<std::tuple<int,int>> zero_runs(const cv::Mat& hist) {
    int w = hist.rows;

    std::vector<std::tuple<int, int>> return_value;

    int pos = 0;
    for (int i = 0; i < w; i++) {
        if ((i == 0 && hist.at<float>(i, 0) == 0) ||
            (i > 0 && hist.at<float>(i, 0) == 0 && hist.at<float>(i - 1,0) > 0)) {
            pos = i;
        }

        if ((i == w - 1 && hist.at<float>(i, 0) == 0) ||
            (i < w - 1 && hist.at<float>(i, 0) == 0 && hist.at<float>(i + 1,0) > 0)) {
            return_value.push_back(std::make_tuple(pos, i));
        }
    }
    return return_value;
}



