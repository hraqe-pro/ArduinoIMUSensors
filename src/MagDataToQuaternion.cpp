#include "MagDataToQuaternion.h"
#include <Arduino.h>

MagDataToQuaternion::MagDataToQuaternion()
{
}

void MagDataToQuaternion::normalize(float &x, float &y, float &z)
{
    float length = sqrt (x * x + y * y + z * z);
    if (length != 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}

QuaternionEarthMatrix MagDataToQuaternion::calculateQuaternion(float magDataX, float magDataY, float magDataZ, float earthMagRefX, float earthMagRefY, float earthMagRefZ) {
    MagDataToQuaternion::normalize(magDataX, magDataY, magDataZ); //magnetometer data vector normalization
    MagDataToQuaternion::normalize(earthMagRefX, earthMagRefY, earthMagRefZ); //earth magnetic field vector normalization
    //vector product - rotation axis 
    float vx = earthMagRefY * magDataZ - earthMagRefZ * magDataY;
    float vy = earthMagRefZ * magDataX - earthMagRefX * magDataZ;
    float vz = earthMagRefX * magDataY - earthMagRefY * magDataX;
    
    //scalar product - angle cosinus 
    float dot_product = earthMagRefX * magDataX + earthMagRefY * magDataY + earthMagRefZ * magDataZ;

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
