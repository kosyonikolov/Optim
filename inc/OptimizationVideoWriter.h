#ifndef OPTIMIZATION_VIDEO_WRITER_H
#define OPTIMIZATION_VIDEO_WRITER_H

#include <string>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "Genocop.h"

class OptimizationVideoWriter
{
public:

    OptimizationVideoWriter(const uint32_t w, const uint32_t h,
                            const double xs, const double xe,
                            const double ys, const double ye) :
                            WIDTH(w), HEIGHT(h), X_OFFSET(xs), X_SCALE((w - 1) / (xe - xs)),
                            Y_OFFSET(ys), Y_SCALE((h - 1) / (ye - ys)) 
    {
        backgroundImg = cv::Mat::zeros(h, w, CV_8UC3);
    }

    void drawBackground(ObjectiveFunction objFun, const double gamma = 1.0);

    void drawFrame(const std::vector<Genocop::Score> & population);

    void begin(const std::string & filename);

private:
    const uint32_t WIDTH;
    const uint32_t HEIGHT;

    // vector to image coordinates
    const double X_OFFSET;
    const double X_SCALE;
    const double Y_OFFSET;
    const double Y_SCALE;

    cv::Mat backgroundImg;
    cv::VideoWriter videoWriter;

    void drawGrid();    
};

#endif