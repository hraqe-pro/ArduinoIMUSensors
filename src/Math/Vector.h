#ifndef VECTOR_H
#define VECTOR_H
//float default structure to all use

struct Vector
{
    Vector() {};
    Vector(const double x, const double y, const double z) : x(x), y(y), z(z) {};

    double x;  
    double y;
    double z;
};
#endif //VECTOR_H
