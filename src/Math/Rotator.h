#ifndef ROTATOR_H
#define ROTATOR_H

#include "BiasMatrix.h"
#include <array>

struct Rotator
{
    Rotator();

    Rotator(double pitch, double yaw, double roll) : pitch(pitch), yaw(yaw), roll(roll) {};

    double pitch = 0;
    double yaw = 0;
    double roll = 0;

    std::array<double, 3> ToArray() const;

    QuaternionEarthMatrix ToQuaternion() const;

    double YawToQuaternion() const;
};

Rotator operator-(const Rotator& thisRotator, const BiasMatrix& biasMatrix) {
        return Rotator(thisRotator.pitch - biasMatrix.biasMatrix[0],
            thisRotator.yaw - biasMatrix.biasMatrix[1],
            thisRotator.roll - biasMatrix.biasMatrix[2]);
}



#endif