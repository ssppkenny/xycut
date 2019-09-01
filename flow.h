//
// Created by sergey on 16.03.19.
//

#ifndef FLOW_READER_WORD_H
#define FLOW_READER_WORD_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>


struct Word {
    cv::Point leftUpper;
    int width;
    int height;
    int hindent;
    int vindent;
};

struct Line {
    int height;
    std::vector<Word> words;
    Line(int height) : height(height) {
        words = std::vector<Word>();
    }
};

struct LineDescription {
    int baseupper;
    int baselower;
    int upper;
    int lower;

    LineDescription() : baseupper(0), baselower(0), upper(0), lower(0) {

    }

    LineDescription(int baseupper, int baselower) : baseupper(baseupper), baselower(baselower), upper(0), lower(0) {

    }
    LineDescription(int baseupper, int baselower, int upper, int lower) : baseupper(baseupper), baselower(baselower), upper(upper), lower(lower) {

    }
};

struct Page {
    int width;
    int height;
    int lineHeight;
    cv::Mat mat;
    std::vector<Line> lines;
    Page reflow(int width, int height);
    Page() {};
    explicit Page(char *filename);
    void parseImage();
    void calcBetweens(std::map<int,LineDescription>& ld);
    std::vector<std::pair<int,int>> getWordPositions(cv::Mat hist);
};


#endif //FLOW_READER_WORD_H
