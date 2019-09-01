//
// Created by sergey on 01.09.19.
//

#include "Xycut.h"
#include <iostream>
#include <vector>

#include "common.h"


cv::Mat Xycut::image_without_white_borders() {
    cv::Mat hist;
    //horizontal
    reduce(image, hist, 0, cv::REDUCE_SUM, CV_32F);
    std::vector<cv::Point> locations;
    cv::findNonZero(hist, locations);


    int left = locations.at(0).x;
    int right = locations.at(locations.size()-1).x;
    //vertical
    reduce(image, hist, 1, cv::REDUCE_SUM, CV_32F);
    cv::findNonZero(hist, locations);

    int upper = locations.at(0).y;
    int lower = locations.at(locations.size()-1).y;

    cv::Rect rect(left, upper, right-left, lower-upper);

    return image(rect);
}

void Xycut::xycut_step(cv::Mat img, double threshold) {

    cv::Size s = img.size();
    int h = s.height;
    int w = s.width;

    cv::Mat hist;
    //horizontal
    reduce(img, hist, 0, cv::REDUCE_SUM, CV_32F);
    std::vector<std::tuple<int,int>> zr = zero_runs_hor(hist);

    int maxind = max_ind(zr, threshold);

    if (maxind >= 0) {
        int _from = std::get<0>(zr.at(maxind));
        int _to = std::get<1>(zr.at(maxind));
        cv::Rect left_rect(0,0,_from,h);
        cv::Rect right_rect(_to,0,w-_to,h);
        cv::Mat left_image = img(left_rect);
        cv::Size s = left_image.size();
        int h = s.height;
        int w = s.width;
        if (h > 0 && w > 0) {
            cv::Mat left_image = img(left_rect);
            xycut_step(left_image, threshold);
        }
        cv::Mat right_image = img(right_rect);
        s = right_image.size();
        h = s.height;
        w = s.width;
        if (h > 0 && w > 0) {
            cv::Mat right_image = img(right_rect);
            xycut_step(right_image, threshold);
        }

    } else {
        reduce(img, hist, 1, cv::REDUCE_SUM, CV_32F);
        zr = zero_runs(hist);
        int maxind = max_ind(zr, threshold);

        if (maxind >= 0) {
            int _from = std::get<0>(zr.at(maxind));
            int _to = std::get<1>(zr.at(maxind));
            cv::Rect upper_rect(0, 0, w, _from);
            cv::Rect lower_rect(0, _to, w, h-_to);
            cv::Mat upper_image = img(upper_rect);
            cv::Size s = upper_image.size();
            int h = s.height;
            int w = s.width;
            if (h > 0 && w > 0) {
                cv::Mat left_image = img(upper_rect);
                xycut_step(left_image, threshold);
            }
            cv::Mat lower_image = img(lower_rect);
            s = lower_image.size();
            h = s.height;
            w = s.width;
            if (h > 0 && w > 0) {
                cv::Mat right_image = img(lower_rect);
                xycut_step(right_image, threshold);
            }
        }


    }




    std::cout << "ok" << std::endl;

}
