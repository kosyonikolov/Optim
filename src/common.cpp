#include "common.h"

void printVector(const Vector & x, std::ostream & stream, const std::string & separator)
{
    stream << x[0];
    for (uint32_t i = 1; i < x.size(); i++)
    {
        stream << separator << x[i];
    }
}

std::ostream & operator<<(std::ostream & stream, const Vector & x)
{
    printVector(x, stream, ", ");
    return stream;
}
