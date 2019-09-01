#include <iostream>
#include "flow.h"
#include <opencv2/opencv.hpp>
#include "Xycut.h"

int main() {

    //Page p = Page("littlewood.png");
    //std::cout << p.height << " " << p.width << std::endl;
    cv::Mat image = cv::imread("out2.jpg", 0);

    cv::Mat copy = image.clone();
    cv::bitwise_not(copy, copy);
    cv::threshold(copy, copy, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::Mat labeled(image.size(), image.type());
    cv::Mat rectComponents = cv::Mat::zeros(cv::Size(0, 0), 0);
    cv::Mat centComponents = cv::Mat::zeros(cv::Size(0, 0), 0);
    connectedComponentsWithStats(copy, labeled, rectComponents, centComponents);

    int count = rectComponents.rows - 1;
    double heights[count];

    for (int i = 1; i < rectComponents.rows; i++) {
        int h = rectComponents.at<int>(cv::Point(3, i));
        heights[i - 1] = h;
    }

    cv::Scalar m, stdv;
    cv::Mat hist(1, count, CV_64F, &heights);
    meanStdDev(hist, m, stdv);
    double threshold = m(0);

    Xycut xycut(copy);
    cv::Mat without_borders = xycut.image_without_white_borders();
    xycut.xycut_step(without_borders, threshold);

    cv::imwrite("out.png", without_borders);

    //Page np = p.reflow(p.width/2, p.height*2);

    //cv::imwrite("reflow.png", np.mat);


    return 0;
}