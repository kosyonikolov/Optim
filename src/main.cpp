#include <iostream>
#include <valarray>
#include <cmath>
#include <opencv2/opencv.hpp>

#include "Genocop.h"

cv::VideoWriter videoWriter;

void drawPopulation(const std::vector<Genocop::Score> & population)
{
    const uint32_t IMAGE_WIDTH = 1024;
    const uint32_t IMAGE_HEIGHT = 1024;
    const double X_MIN = -3;
    const double X_MAX = 3;
    const double Y_MIN = -3;
    const double Y_MAX = 3;

    const double SCALE_X = (IMAGE_WIDTH - 1) / (X_MAX - X_MIN);
    const double SCALE_Y = (IMAGE_HEIGHT - 1) / (Y_MAX - Y_MIN);

    // create image
    cv::Mat image = cv::Mat::zeros(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);

    // TODO draw coordinate axis

    // draw points
    for (auto & score : population)
    {
        const double x = score.x[0];
        const double y = score.x[1];

        cv::Point2f pt(float((x - X_MIN) * SCALE_X ), float((y - Y_MIN) * SCALE_Y));
        cv::circle(image, pt, 2, cv::Scalar(0, 0, 255), cv::FILLED);
    }

    static uint32_t id = 0;
    //cv::imwrite("iter_" + std::to_string(id) + ".jpg", image);
    videoWriter << image;
    id++;
}

double objFunc1(const Vector & x)
{
    double val = x[0] + x[0] * std::sin(20 * x[0]);
    return val;
}

double banana(const Vector & vec)
{
    const double a = 1;
    const double b = 100;

    const double x = vec[0];
    const double y = vec[1];

    double val = std::pow(a - x, 2) + b * std::pow(y - x * x, 2);
    return val;
}

void run1d()
{
    // ranges
    Vector xMin = {-2};
    Vector xMax = {8};

    Genocop optim(1, objFunc1, xMin, xMax);

    Genocop::Options options;

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution << "\n"; 
}

void run2d()
{
    // ranges
    Vector xMin = {-3, -3};
    Vector xMax = {3, 3};

    Genocop optim(2, banana, xMin, xMax);

    Genocop::Options options;
    options.eliteChildrenCount = 0;

    options.tournament.p = 0.9;
    options.tournament.size = 3;

    options.maxIters = 500;
    options.mutatation.fineMutationMin = 1e-5;
    options.mutatation.fineMutationMax = 0.3;
    options.mutatation.pFull = 0.001;
    options.mutatation.pFine = 0.2;

    options.crossover.totalProbability = 0.8;

    optim.callback = drawPopulation;

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution  << "\n"; 
}

int main() 
{
    // open output video
    videoWriter.open("optim.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(1024, 1024));

    run2d();
    return 0;
}
