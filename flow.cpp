//
// Created by sergey on 16.03.19.
//
#include <opencv2/opencv.hpp>
#include "flow.h"
#include "Dbscan.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;


Page Page::reflow(int width, int height) {
    Page p = Page();
    p.width = width;
    p.height = height;
    cv::Mat newMat(height, width, CV_8UC1, cv::Scalar(255,255,255));

    cv::Mat grayscale;
    cv::cvtColor(mat,grayscale, cv::COLOR_BGR2GRAY);

    int currentVertPos = lineHeight;

    vector<Word> allwords;
    for (Line line : lines) {
        vector<Word> words = line.words;
        for (Word w : words) {
            allwords.push_back(w);
        }
    }

    int currentHorPos = 30;
    Line line(lineHeight);
    int count = 1;
    for (Word w : allwords) {

        cv::Range cols(w.leftUpper.x, w.leftUpper.x+w.width);
        cv::Range rows(w.leftUpper.y, w.leftUpper.y+w.height);
        cv::Mat wordRect = grayscale(rows, cols);
        //cv::imwrite(string("word") + std::to_string(count) + string(".png"), wordRect);
        count++;
        if (currentHorPos + w.width < p.width) {
            line.words.push_back(w);

            try {
                cv::Range newrows(currentVertPos+w.vindent, currentVertPos+w.vindent+w.height);
                cv::Range newcols(currentHorPos, currentHorPos+w.width);

                wordRect.copyTo(newMat(newrows, newcols));
                currentHorPos += w.width;
            }
            catch (...) {
                cout << "exception" << endl;
            }


        } else {
            lines.push_back(line);

            line = Line(lineHeight);
            currentVertPos += lineHeight;
            currentHorPos = 30;

            cv::Range newrows(currentVertPos+w.vindent, currentVertPos+w.vindent+w.height);
            cv::Range newcols(currentHorPos, currentHorPos+w.width);

            wordRect.copyTo(newMat(newrows, newcols));
            currentHorPos += w.width;

            line.words.push_back(w);

        }
    }
    cv::imwrite("newMat.png", newMat);
    p.mat = newMat;

    return p;
}

void Page::calcBetweens(std::map<int,LineDescription>& ld) {

    map<int,pair<int,int>> betweens;

    for (int i = 1; i<=ld.size();i++) {
        int upper = ld[i].baseupper;
        int lower = ld[i].baselower;
        if (i==1) {
            if (ld.size() > 1) {
                int nextupper = ld[i+1].baseupper;
                betweens[i] = make_pair(lower/2, lower + (nextupper - lower)/2);
            } else {
                betweens[i] = make_pair(lower/2, lower + (height - lower)/2);
            }
        } else if (i==ld.size()) {
            if (ld.size() > 1) {
                 int prevlower = ld[i-1].baselower;

                 betweens[i] = make_pair(upper - (upper- prevlower)/2, lower + (height-lower)/2);
            }
        } else {
            int nextupper = ld[i+1].baseupper;
            int prevlower = ld[i-1].baselower;
            betweens[i] = make_pair(upper - (upper-prevlower)/2, lower + (nextupper - lower)/2);
        }
    }

    for (int i=1;i<=betweens.size(); i++) {

        int upper = betweens[i].first;
        int lower = betweens[i].second;
        ld[i].upper = upper;
        ld[i].lower = lower;

        cv::Range rows(upper, lower);
        cv::Range cols(0, width);

        cv::Mat lineRect = mat(rows, cols);


        cv::cvtColor(lineRect, lineRect, cv::COLOR_BGR2GRAY);
        lineRect.convertTo(lineRect, CV_8UC1);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        cv::erode(lineRect, lineRect, kernel);
        cv::dilate(lineRect, lineRect, kernel);
        cv::erode(lineRect, lineRect, kernel);
        cv::dilate(lineRect, lineRect, kernel);

        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8,3));
        cv::erode(lineRect, lineRect, kernel);


        cv::bitwise_not(lineRect, lineRect);
        cv::threshold( lineRect, lineRect, 200, 1, 3 );


        cv::Mat hist;

        cv::reduce(lineRect, hist, 1, cv::REDUCE_SUM, CV_32SC1);
        cv::transpose(hist,hist);
        std::vector<std::pair<int,int>> margins = getWordPositions(hist);

        int max = 0;
        for (int k=0;k<margins.size(); k++) {
            if (k==0) {
                ld[i].upper = betweens[i].first + margins[k].first;
                betweens[i].first = betweens[i].first + margins[k].first;
            }
            if (k==margins.size()-1) {
                //betweens[i].second = betweens[i].first + margins[k].second;
            }
        }
    }
}

void Page::parseImage() {

    cv::Mat greyMat;
    cv::cvtColor(mat, greyMat, cv::COLOR_BGR2GRAY);

    std::vector<cv::Vec4i> linesMat;

    cv::bitwise_not(greyMat, greyMat);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(4,4));
    cv::morphologyEx(greyMat, greyMat, cv::MORPH_OPEN, kernel);
    cv::bitwise_not(greyMat, greyMat);

    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(40,1));
    cv::erode(greyMat, greyMat, kernel);
    cv::bitwise_not(greyMat, greyMat);
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1,1));

    cv::dilate(greyMat, greyMat, kernel);

    cv::HoughLinesP( greyMat, linesMat, 1.0, CV_PI/180.0, 100, 100);
    vector<int> points;

    for( size_t i = 0; i < linesMat.size(); i++ ) {
        points.push_back(linesMat[i][1]);
    }

    sort(points.begin(), points.end(), [](const int& a, const int& b){ return a < b; });
    Dbscan dbscan(5, 8, points);
    dbscan.run();
    map<int,pair<int,int>> limits = dbscan.getLimits();

    map<int,LineDescription> ld;

    for (int i=1;i<=limits.size(); i++) {
        if (limits.find(i) != limits.end()) {
            pair<int,int> limit = limits[i];
            ld[i] = LineDescription(limit.first, limit.second);
        }
    }

    calcBetweens(ld);

    lineHeight = (ld[ld.size()].baseupper - ld[1].baseupper)/ld.size() ;

    // iterate lines
    for (int i = 1; limits.find(i) != limits.end();i++) {
        int upper = ld[i].upper;
        int lower = ld[i].lower;
        int baseupper = ld[i].baseupper;
        //int baselower = ld[i].baselower;

        cv::line(mat, cv::Point(0,upper),  cv::Point(width,upper), cv::Scalar(255,0,255) );
        cv::line(mat, cv::Point(0,lower),  cv::Point(width,lower), cv::Scalar(255,0,255) );

        cv::Range rows(upper, lower);
        cv::Range cols(0, width);

        cv::Mat lineRect = mat(rows, cols);
        
        cv::cvtColor(lineRect, lineRect, cv::COLOR_BGR2GRAY);
        lineRect.convertTo(lineRect, CV_8UC1);

        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        cv::erode(lineRect, lineRect, kernel);
        cv::dilate(lineRect, lineRect, kernel);
        cv::erode(lineRect, lineRect, kernel);
        cv::dilate(lineRect, lineRect, kernel);

        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8,3));
        cv::erode(lineRect, lineRect, kernel);


        cv::bitwise_not(lineRect, lineRect);
        cv::threshold( lineRect, lineRect, 200, 1, 3 );


        cv::Mat hist;

        cv::reduce(lineRect, hist, 0, cv::REDUCE_SUM, CV_32SC1);


        std::vector<std::pair<int,int>> wordPositions = getWordPositions(hist);

        Line line(lineHeight);

        for (int k=0; k<wordPositions.size(); k++) {
            int begin = wordPositions[k].first;
            int end = wordPositions[k].second;
            //cv::line(mat, cv::Point(begin,upper),  cv::Point(begin,lower), cv::Scalar(255,0,255) );
            //cv::line(mat, cv::Point(end,upper),  cv::Point(end,lower), cv::Scalar(255,0,255) );

            cv::Range rows(upper, lower);
            cv::Range cols(begin, end);

            cv::Mat wordRect = mat(rows, cols);

            cv::cvtColor(wordRect, wordRect, cv::COLOR_BGR2GRAY);
            wordRect.convertTo(wordRect, CV_8UC1);

            cv::bitwise_not(wordRect, wordRect);


            kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));


            cv::erode(wordRect, wordRect, kernel);
            cv::dilate(wordRect, wordRect, kernel);
            cv::erode(wordRect, wordRect, kernel);
            cv::dilate(wordRect, wordRect, kernel);

            //cv::bitwise_not(wordRect, wordRect);

            cv::Mat hist;

            cv::threshold( wordRect, wordRect, 200, 1, 3 );
            cv::reduce(wordRect, hist, 1, cv::REDUCE_SUM, CV_32SC1);
            cv::transpose(hist,hist);

            std::vector<std::pair<int,int>> zrH = getWordPositions(hist);

            for (int l=0;l<zrH.size(); l++) {
                Word word;
                int beginH = zrH[l].first;
                int endH = zrH[l].second;

                cv::rectangle(mat, cv::Rect(begin,upper+beginH,end-begin,endH-beginH), cv::Scalar(255,0,255));
                word.height = endH-beginH;
                word.width = end-begin;
                word.vindent = upper + beginH - baseupper;
                word.leftUpper = cv::Point(begin,upper+beginH);
                line.words.push_back(word);

                //cv::line(mat, cv::Point(begin,upper+beginH),  cv::Point(end,upper+beginH), cv::Scalar(0,0,255) );
                //cv::line(mat, cv::Point(begin,upper+endH),  cv::Point(end,upper+endH), cv::Scalar(0,0,255) );
            }


        }
        lines.push_back(line);


        //cv::line(mat, cv::Point(0,upper),  cv::Point(width,upper), cv::Scalar(255,0,255) );
        //cv::line(mat, cv::Point(0,lower),  cv::Point(width,lower), cv::Scalar(255,0,255) );
    }

    

    cv::imwrite("rects.png", mat);

}

 std::vector<std::pair<int,int>> Page::getWordPositions(cv::Mat hist) {
    int startpos = 0;
    std::vector<std::pair<int,int>> pairs;
    for (int j=0;j<hist.cols;j++) {
        int val = hist.at<int>(0,j);

        if ((j==0 && val > 0) || (j>0 && val >0 && hist.at<int>(0,j-1) == 0)) {
            startpos = j-1;
        }

        if ((j == hist.cols-1 && val > 0) ||  ( j> 0 &&  val==0 && hist.at<int>(0,j-1) > 0)) {
            pairs.push_back(make_pair(startpos, j));
        }
    }
    return pairs;
 }


Page::Page(char *filename) {
    mat = imread(filename, cv::IMREAD_COLOR); // Read the file
    width = mat.cols;
    height = mat.rows;
    parseImage();
}