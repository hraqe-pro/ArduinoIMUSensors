#ifndef GYRO_H
#define GYRO_H

#include <Arduino_LSM6DSOX.h>
#include "BiasMatrix.h"
#include "Math/QuaternionEarthMatrix.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Math/KalmanFilter.h"

class Gyro {
private:
    BiasMatrix localBiasMatrix;

    QuaternionEarthMatrix localQuaternion;

    Rotator rawData;

    KalmanFilter3D kalmanFilter  = KalmanFilter3D(0.01, 0.1, 0.1);
public:

    Gyro();

    void Read();

    Rotator BiasCutter(const BiasMatrix& biasData);

    Vector GetCalibratedData();

    Rotator GetRawData();
};

#endif