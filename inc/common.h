#ifndef COMMON_H
#define COMMON_H

#include <valarray>
#include <string>
#include <functional>
#include <stdint.h>
#include <ostream>

typedef std::valarray<double> Vector;

// Objective function to be minimized
typedef std::function<double(const Vector &)> ObjectiveFunction;

void printVector(const Vector & x, std::ostream & stream, const std::string & separator);

std::ostream & operator<<(std::ostream & stream, const Vector & x);

#endif