#include <Gyro.h>



Gyro::Gyro()
{
    
}
void Gyro::BiasCutter(const BiasMatrix& biasData) {
    rawData = rawData - biasData;
}