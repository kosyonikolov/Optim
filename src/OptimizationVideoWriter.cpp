#include "OptimizationVideoWriter.h"

void OptimizationVideoWriter::drawBackground(ObjectiveFunction objFun, const double gamma)
{
    cv::Mat functionValues(HEIGHT, WIDTH, CV_64FC1);

    double minVal = 1e99;
    double maxVal = -1e99;

    for (uint32_t y = 0; y < HEIGHT; y++)
    {
        const double vectorY = y / Y_SCALE + Y_OFFSET;
        double * line = functionValues.ptr<double>(y);

        for (uint32_t x = 0; x < WIDTH; x++)
        {
            const double vectorX = x / X_SCALE + X_OFFSET;
            Vector vec = {vectorX, vectorY};

            const double val = objFun(vec);
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);

            line[x] = val;
        }
    }

    // normalize to 0..1
    functionValues -= minVal;
    functionValues *= 1.0 / (maxVal - minVal);

    // apply gamma
    cv::pow(functionValues, gamma, functionValues);

    cv::Mat temp;
    functionValues.convertTo(temp, CV_8UC1, 255.0);
    cv::cvtColor(temp, this->backgroundImg, cv::COLOR_GRAY2BGR);
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
    this->videoWriter.open(filename, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 2, cv::Size(WIDTH, HEIGHT));
}

void OptimizationVideoWriter::drawGrid()
{
    const cv::Scalar gridColor(255, 0, 128);

    // draw horizontal lines
    float xEnd = WIDTH / X_SCALE - X_OFFSET;
    for (float x = std::floor(X_OFFSET); x <= xEnd; x += 1.0f)
    {
        const float imageX = (x - X_OFFSET) * X_SCALE;
        cv::line(backgroundImg, cv::Point2f(imageX, 0), cv::Point2f(imageX, HEIGHT - 1), gridColor);
        cv::putText(backgroundImg, std::to_string(int(x)), cv::Point2f(imageX + 10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, gridColor);
    }

    // draw vertical lines
    float yEnd = HEIGHT / Y_SCALE - Y_OFFSET;
    for (float y = std::floor(Y_OFFSET); y <= yEnd; y += 1.0f)
    {
        float imageY = (y - Y_OFFSET) * Y_SCALE;
        cv::line(backgroundImg, cv::Point2f(0, imageY), cv::Point2f(WIDTH - 1, imageY), gridColor);
        cv::putText(backgroundImg, std::to_string(int(y)), cv::Point2f(5, imageY - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6, gridColor);
    }
}