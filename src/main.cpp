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

double rastrigin(const Vector & vec)
{
    const double a = 10;
    const double PI = 3.14159265359;

    const double x = vec[0];
    const double y = vec[1];

    double val = 2 * a + x * x + y * y - a * std::cos(2 * PI * x) - a * std::cos(2 * PI * y);
    return val;
}

double himmelblau(const Vector & vec)
{
    const double a = 10;

    const double x = vec[0];
    const double y = vec[1];

    double val = std::pow(x * x + y - 11, 2) + std::pow(x + y * y - 7, 2);
    return val;
}

double levi13(const Vector & vec)
{
    const double a = 10;
    const double PI = 3.14159265359;

    const double x = vec[0];
    const double y = vec[1];

    double val = std::pow(std::sin(3 * PI * x), 2) + std::pow(x - 1, 2) * (1 + std::pow(std::sin(3 * PI * y), 2)) + std::pow(y - 1, 2) * (1 + std::pow(std::sin(3 * PI * x), 2));
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

void run2d_rastrigin()
{
    // ranges
    Vector xMin = {-5.12, -5.12};
    Vector xMax = {5.12, 5.12};

    Genocop optim(2, rastrigin, xMin, xMax);

    Genocop::Options options;
    options.eliteChildrenCount = 1;

    options.tournament.p = 0.9;
    options.tournament.size = 6;

    options.maxIters = 100;
    options.mutatation.fineMutationMin = 1e-5;
    options.mutatation.fineMutationMax = 0.2;
    options.mutatation.pFull = 0.05;
    options.mutatation.pFine = 0.2;

    options.crossover.totalProbability = 0.8;

    // video output
    OptimizationVideoWriter video(1024, 1024, -5.12, 5.12, -5.12, 5.12);
    video.begin("rastrigin.mp4");
    video.drawBackground(rastrigin, 1.0);

    optim.callback = [&video](const std::vector<Genocop::Score> & population)
    {
        video.drawFrame(population);
    };

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution  << "\n"; 
}

void run2d_himmelblau()
{
    // ranges
    Vector xMin = {-5.12, -5.12};
    Vector xMax = {5.12, 5.12};

    Genocop optim(2, himmelblau, xMin, xMax);

    Genocop::Options options;
    options.eliteChildrenCount = 1;

    options.tournament.p = 0.9;
    options.tournament.size = 6;

    options.maxIters = 100;
    options.mutatation.fineMutationMin = 1e-5;
    options.mutatation.fineMutationMax = 0.2;
    options.mutatation.pFull = 0.05;
    options.mutatation.pFine = 0.2;

    options.crossover.totalProbability = 0.8;

    // video output
    OptimizationVideoWriter video(1024, 1024, -5.12, 5.12, -5.12, 5.12);
    video.begin("himmelblau.mp4");
    video.drawBackground(himmelblau, 0.5);

    optim.callback = [&video](const std::vector<Genocop::Score> & population)
    {
        video.drawFrame(population);
    };

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution  << "\n"; 
}

void run2d_levi13()
{
    // ranges
    Vector xMin = {-10, -10};
    Vector xMax = {10, 10};

    Genocop optim(2, levi13, xMin, xMax);

    Genocop::Options options;
    options.eliteChildrenCount = 1;

    options.tournament.p = 0.9;
    options.tournament.size = 6;

    options.maxIters = 100;
    options.mutatation.fineMutationMin = 1e-5;
    options.mutatation.fineMutationMax = 0.2;
    options.mutatation.pFull = 0.05;
    options.mutatation.pFine = 0.2;

    options.crossover.totalProbability = 0.8;

    // video output
    OptimizationVideoWriter video(1024, 1024, -10, 10, -10, 10);
    video.begin("levi13.mp4");
    video.drawBackground(levi13, 0.5);

    optim.callback = [&video](const std::vector<Genocop::Score> & population)
    {
        video.drawFrame(population);
    };

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution  << "\n"; 
}

double f2d(const Vector & vec)
{
    double x = vec[0];
    double y = vec[1];
    double sum = x + 10.0 * y;
    double val = sum * sum;
    //double val = std::pow(x + 10.0 * y, 2);
    return val;
}

void run2d_f()
{
    // ranges
    Vector xMin = {-10, -10};
    Vector xMax = {10, 10};

    Genocop optim(2, f2d, xMin, xMax);

    Genocop::Options options;
    options.eliteChildrenCount = 1;

    options.tournament.p = 0.9;
    options.tournament.size = 6;

    options.maxIters = 100;
    options.mutatation.fineMutationMin = 1e-5;
    options.mutatation.fineMutationMax = 0.2;
    options.mutatation.pFull = 0.05;
    options.mutatation.pFine = 0.2;

    options.crossover.totalProbability = 0.8;

    // video output
    OptimizationVideoWriter video(1024, 1024, -10, 10, -10, 10);
    video.begin("levi13.mp4");
    video.drawBackground(f2d, 0.5);

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
    run2d_f();
    return 0;
}
