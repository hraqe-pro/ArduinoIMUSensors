#include "MagDataToQuaternion.h"
#include <Arduino.h>

MagDataToQuaternion::MagDataToQuaternion() {
}

void MagDataToQuaternion::normalize(double &x, double &y, double &z) {
    double length = sqrt (x * x + y * y + z * z);
    if (length != 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}
SensorData data;
Mag magPrime;
MagStructure magPrimeStructure = magPrime.Calibration(data);
QuaternionEarthMatrix MagDataToQuaternion::calculateQuaternion(MagStructure& magPrimeStructure, double earthMagRefX, double earthMagRefY, double earthMagRefZ) {
    MagDataToQuaternion::normalize(magPrimeStructure.x, magPrimeStructure.y, magPrimeStructure.z); //magnetometer data vector normalization
    MagDataToQuaternion::normalize(earthMagRefX, earthMagRefY, earthMagRefZ); //earth magnetic field vector normalization
    //vector product - rotation axis 
    float vx = earthMagRefY * magPrimeStructure.z - earthMagRefZ * magPrimeStructure.y;
    float vy = earthMagRefZ * magPrimeStructure.x - earthMagRefX * magPrimeStructure.z;
    float vz = earthMagRefX * magPrimeStructure.y - earthMagRefY * magPrimeStructure.x;
    
    //scalar product - angle cosinus 
    float dot_product = earthMagRefX * magPrimeStructure.x + earthMagRefY * magPrimeStructure.y + earthMagRefZ * magPrimeStructure.z;

    //rotation angle
    float theta = acos(dot_product);

    //quaternion rotation calculation
    QuaternionEarthMatrix qem;
    qem.w = cos(theta / 2);
    qem.x = vx * sin(theta / 2);    
    qem.y = vy * sin(theta / 2);
    qem.z = vz * sin(theta / 2);    
    return qem;
}

QuaternionEarthMatrix MagDataToQuaternion::adjustOrientationToReference(QuaternionEarthMatrix qemRef) {
    QuaternionEarthMatrix reference = {1.0, 0.0, 0.0, 0.0}; /// tu trzeba sie upewnić w kwestii tego wektora odniesienia na podstawie układu współrzędnych czy takie założenia są prawidłowe
    //quaternion's product
    qemFinal.w = qemRef.w * reference.w - qemRef.x * reference.x - qemRef.y * reference.y - qemRef.z * reference.z;
    qemFinal.w = qemRef.w * reference.x - qemRef.x * reference.w - qemRef.y * reference.z - qemRef.z * reference.y;
    qemFinal.w = qemRef.w * reference.y - qemRef.x * reference.z - qemRef.y * reference.w - qemRef.z * reference.x;
    qemFinal.w = qemRef.w * reference.z - qemRef.x * reference.y - qemRef.y * reference.x - qemRef.z * reference.w;
    return qemFinal;
}
