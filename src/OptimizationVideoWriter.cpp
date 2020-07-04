#include "OptimizationVideoWriter.h"

void OptimizationVideoWriter::drawBackground(ObjectiveFunction objFun, const double gamma)
{

}

void OptimizationVideoWriter::drawFrame(const std::vector<Genocop::Score> & population)
{
    cv::Mat image = this->backgroundImg.clone();
    for (auto & score : population)
    {
        const double x = score.x[0];
        const double y = score.x[1];

        cv::Point2f pt(float((x - X_OFFSET) * X_SCALE), float((y - Y_OFFSET) * Y_SCALE));
        cv::circle(image, pt, 2, cv::Scalar(0, 0, 255), cv::FILLED);
    }

    this->videoWriter << image;
}

void OptimizationVideoWriter::begin(const std::string & filename)
{
    this->videoWriter.open(filename, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(WIDTH, HEIGHT));
}

void OptimizationVideoWriter::drawGrid()
{

}