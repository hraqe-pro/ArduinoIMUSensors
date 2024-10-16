#ifndef VECTOR_H
#define VECTOR_H

#include <array>
//float default structure to all use

struct Vector
{
    Vector() {};
    Vector(const double x, const double y, const double z) : x(x), y(y), z(z) {};

    std::array<double, 3> ToArray();

    double x;  
    double y;
    double z;
};
#endif //VECTOR_H
