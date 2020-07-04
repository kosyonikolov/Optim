#include <iostream>
#include <valarray>
#include <cmath>
#include <opencv2/opencv.hpp>

#include "Genocop.h"
#include "OptimizationVideoWriter.h"

cv::VideoWriter videoWriter;

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
    options.eliteChildrenCount = 1;

    options.tournament.p = 0.9;
    options.tournament.size = 6;

    options.maxIters = 100;
    options.mutatation.fineMutationMin = 1e-5;
    options.mutatation.fineMutationMax = 0.2;
    options.mutatation.pFull = 0.02;
    options.mutatation.pFine = 0.2;

    options.crossover.totalProbability = 0.8;

    // video output
    OptimizationVideoWriter video(1024, 1024, -3, 3, -3, 3);
    video.begin("banana.mp4");
    video.drawBackground(banana, 0.3);

    optim.callback = [&video](const std::vector<Genocop::Score> & population)
    {
        video.drawFrame(population);
    };

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution  << "\n"; 
}

int main() 
{
    run2d();
    return 0;
}
