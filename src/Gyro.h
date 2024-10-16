#ifndef GYRO_H
#define GYRO_H

#include <Arduino_LSM6DSOX.h>
#include "BiasMatrix.h"
#include "Math/QuaternionEarthMatrix.h"
#include "Math/Rotator.h"

class Gyro {
private:
    BiasMatrix localBiasMatrix;

    QuaternionEarthMatrix localQuaternion;

    Rotator rawData;
public:

    Gyro();

    void BiasCutter(const BiasMatrix& biasData);
};

#endif