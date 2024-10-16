#include "Rotator.h"
#include "QuaternionEarthMatrix.h"

std::array<double, 3> Rotator::ToArray()
{
    return std::array<double, 3> {pitch, yaw, roll};
}

QuaternionEarthMatrix Rotator::ToQuaternion() const
{
    QuaternionEarthMatrix quaternion;

    double p = pitch * M_PI / 180.0;
    double y = yaw * M_PI / 180.0;
    double r = roll * M_PI / 180.0;

    // Obliczanie połówek kątów
    double cy = cos(y * 0.5);
    double sy = sin(y * 0.5);
    double cp = cos(p * 0.5);
    double sp = sin(p * 0.5);
    double cr = cos(r * 0.5);
    double sr = sin(r * 0.5);

    quaternion.w = cr * cp * cy + sr * sp * sy;
    quaternion.x = sr * cp * cy - cr * sp * sy;
    quaternion.y = cr * sp * cy + sr * cp * sy;
    quaternion.z = cr * cp * sy - sr * sp * cy;

    return ;
}

double Rotator::YawToQuaternion() const
{
    QuaternionEarthMatrix quaternion;
    quaternion.w = cos(yaw / 2);
    quaternion.z = sin(yaw / 2);

    return quaternion;
}
