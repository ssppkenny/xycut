//
// Created by sergey on 01.09.19.
//

#include <tuple>
#include <opencv2/opencv.hpp>

#ifndef FLOW_READER_COMMON_H
#define FLOW_READER_COMMON_H

std::vector<std::tuple<int,int>> zero_runs_hor(const cv::Mat& hist);
std::vector<std::tuple<int,int>> zero_runs(const cv::Mat& hist);
int max_ind(std::vector<std::tuple<int,int>> zr, double threshold);

#endif //FLOW_READER_COMMON_H
