#include <iostream>
#include <valarray>
#include <cmath>

#include "Genocop.h"

double objFunc1(Vector x)
{
    double val = x[0] + x[0] * std::sin(20 * x[0]);
    return val;
}

double banana(Vector vec)
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

    std::cout << "Min value: " << minVal << " at x = " << solution[0] << "\n"; 
}

void run2d()
{
    // ranges
    Vector xMin = {-3, -3};
    Vector xMax = {3, 3};

    Genocop optim(2, banana, xMin, xMax);

    Genocop::Options options;
    options.maxIters = 1000;
    options.mutatation.fineMutationMin = 1e-7;
    options.mutatation.pFull = 0.0001;
    options.mutatation.pFine = 0.2;

    Vector solution(1);
    double minVal = optim.run(solution, options);

    std::cout << "Min value: " << minVal << " at x = " << solution[0] << ", " << solution[1] << "\n"; 
}

int main() 
{
    run2d();
    return 0;
}
