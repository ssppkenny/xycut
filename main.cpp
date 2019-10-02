#include <iostream>
#include "flow.h"
#include <opencv2/opencv.hpp>
#include "Xycut.h"
#include "PageSegmenter.h"

int main() {

    //Page p = Page("littlewood.png");
    //std::cout << p.height << " " << p.width << std::endl;
    cv::Mat image = cv::imread("image1.png");
    
    Xycut xycut(image);
    std::vector<ImageNode> images = xycut.xycut();
    
    for (int i=0;i<images.size(); i++) {
        ImageNode node = images.at(i);
        cv::rectangle(image, cv::Rect(node.get_x(), node.get_y(), node.get_width(), node.get_height()), cv::Scalar(255,0,0), 2, 8,0);

        //std::string f = std::string("out") + std::to_string(i) + std::string(".png");
        //cv::Mat out = node.get_mat();
        //cv::bitwise_not(out, out);
        //cv::imwrite(f.c_str(), out);

    }

    cv::imwrite("segms.png", image);

    
/*
    PageSegmenter ps(image);
    std::vector<glyph> glyphs = ps.get_glyphs();

    for (int i=0; i<glyphs.size(); i++) {
        glyph g = glyphs.at(i);
        cv::rectangle(image, cv::Rect(g.x, g.y, g.width, g.height), cv::Scalar(255,0,0), 1, 8,0);

        
    }
*/
    cv::imwrite("segms.png", image);

    
    
    //Page np = p.reflow(p.width/2, p.height*2);

    //cv::imwrite("reflow.png", np.mat);


    return 0;
}
