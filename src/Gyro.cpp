#include <Gyro.h>



Gyro::Gyro()
{
    localBiasMatrix.biasMatrix[0] = -0.13015616316239317; 
    localBiasMatrix.biasMatrix[1] = -0.25248730854700857; 
    localBiasMatrix.biasMatrix[2] = 0.11972280376068375;
}
void Gyro::Read()
{
    if (IMU.gyroscopeAvailable())
    {
        float x;
        float y;
        float z;
        IMU.readGyroscope(x, y, z);
        rawData.pitch = x;
        rawData.yaw = y;
        rawData.roll = z;
    }
}
Rotator Gyro::BiasCutter(const BiasMatrix &biasData)
{
    return rawData - biasData;
}

Vector Gyro::GetCalibratedData()
{
    Rotator biasedData = BiasCutter(localBiasMatrix);

    kalmanFilter.predict();
    kalmanFilter.update(biasedData.ToArray());

    std::array<double, 3> estimatedAngles = kalmanFilter.getAngles();
    //kalmanFilter.update();
}

inline Rotator Gyro::GetRawData()
{
    return rawData;
}
