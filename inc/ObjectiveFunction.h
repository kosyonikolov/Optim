#ifndef OBJECTIVE_FUNCTION_H
#define OBJECTIVE_FUNCTION_H

#include <functional>
#include <valarray>

// Objective function to be minimized
typedef std::function<double(const std::valarray<double> &)> ObjectiveFunction;

#endif