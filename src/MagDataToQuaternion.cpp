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
//SensorData data;
//Mag magPrime;
//MagStructure magPrimeStructure = magPrime.Calibration(data);
QuaternionEarthMatrix MagDataToQuaternion::calculateQuaternion(double earthMagRefX, double earthMagRefY, double earthMagRefZ, MagStructure& magPrimeStructure) {
    MagDataToQuaternion::normalize(magPrimeStructure.x, magPrimeStructure.y, magPrimeStructure.z); //magnetometer data vector normalization
    MagDataToQuaternion::normalize(earthMagRefX, earthMagRefY, earthMagRefZ); //earth magnetic field vector normalization
    //vector product - rotation axis 
    float vx = earthMagRefY * magPrimeStructure.z - earthMagRefZ * magPrimeStructure.y;
    float vy = earthMagRefZ * magPrimeStructure.x - earthMagRefX * magPrimeStructure.z;
    float vz = earthMagRefX * magPrimeStructure.y - earthMagRefY * magPrimeStructure.x;
    
    //scalar product - angle cosinus 
    //float dot_product = earthMagRefX * magPrimeStructure.x + earthMagRefY * magPrimeStructure.y + earthMagRefZ * magPrimeStructure.z;
    float dot_product = fmax(-1.0, fmin(1.0, earthMagRefX * magPrimeStructure.x + earthMagRefY * magPrimeStructure.y + earthMagRefZ * magPrimeStructure.z));


    //rotation angle
    float theta = acos(dot_product);

    //quaternion rotation calculation
    QuaternionEarthMatrix qem;
    qem.w = cos(theta / 2);
    qem.x = vx * sin(theta / 2);    
    qem.y = vy * sin(theta / 2);
    qem.z = vz * sin(theta / 2);   
    Serial.print(qem.w);
    Serial.print("|");
    Serial.print(qem.x);
    Serial.print("|");
    Serial.print(qem.y);
    Serial.print("|");
    Serial.print(qem.z);
    Serial.print("|"); 
    Serial.println("ENDLTEST");
    return qem;
}

QuaternionEarthMatrix MagDataToQuaternion::adjustOrientationToReference(QuaternionEarthMatrix qem) {
    QuaternionEarthMatrix reference = {0.98, -0.04, 0.08, 0.01}; /// tu trzeba sie upewnić w kwestii tego wektora odniesienia na podstawie układu współrzędnych czy takie założenia są prawidłowe
    //quaternion's product
    qemFinal.w = qem.w * reference.w - qem.x * reference.x - qem.y * reference.y - qem.z * reference.z;
    qemFinal.x = qem.w * reference.x + qem.x * reference.w + qem.y * reference.z - qem.z * reference.y;
    qemFinal.y = qem.w * reference.y - qem.x * reference.z + qem.y * reference.w + qem.z * reference.x;
    qemFinal.z = qem.w * reference.z + qem.x * reference.y - qem.y * reference.x + qem.z * reference.w;
    return qemFinal;
}
