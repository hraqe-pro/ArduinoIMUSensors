#include "Vector.h"

std::array<double, 3> Vector::ToArray()
{
    return std::array<double, 3> {x, y, z};
}