//
// Created by sergey on 01.09.19.
//

#ifndef FLOW_READER_XYCUT_H
#define FLOW_READER_XYCUT_H

#include <opencv2/opencv.hpp>

class Xycut {

public:
    Xycut(cv::Mat image) {
        this->image = image.clone();
    }
    cv::Mat image_without_white_borders();
    void xycut_step(cv::Mat img, double threshold);
private:
    cv::Mat image;


};


#endif //FLOW_READER_XYCUT_H
