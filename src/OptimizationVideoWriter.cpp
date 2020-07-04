#include "OptimizationVideoWriter.h"

void OptimizationVideoWriter::drawBackground(ObjectiveFunction objFun, const double gamma)
{
    drawGrid();
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
    // draw horizontal lines
    float xEnd = WIDTH / X_SCALE - X_OFFSET;
    for (float x = std::floor(X_OFFSET); x <= xEnd; x += 1.0f)
    {
        const float imageX = (x - X_OFFSET) * X_SCALE;
        cv::line(backgroundImg, cv::Point2f(imageX, 0), cv::Point2f(imageX, HEIGHT - 1), cv::Scalar(0, 128, 128));
        cv::putText(backgroundImg, std::to_string(int(x)), cv::Point2f(imageX + 10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 128, 128));
    }

    // draw vertical lines
    float yEnd = HEIGHT / Y_SCALE - Y_OFFSET;
    for (float y = std::floor(Y_OFFSET); y <= yEnd; y += 1.0f)
    {
        float imageY = (y - Y_OFFSET) * Y_SCALE;
        cv::line(backgroundImg, cv::Point2f(0, imageY), cv::Point2f(WIDTH - 1, imageY), cv::Scalar(0, 128, 128));
        cv::putText(backgroundImg, std::to_string(int(y)), cv::Point2f(5, imageY - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 128, 128));
    }
}