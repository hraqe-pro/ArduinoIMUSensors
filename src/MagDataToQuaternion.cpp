#include "MagDataToQuaternion.h"
#include <math.h>




MagDataToQuaternion::MagDataToQuaternion(){
}

void MagDataToQuaternion::normalize(&float x, &float y, &float z) {
    float length = sqrt (x * x + y * y + z * z);
    if (length != 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}

VectorToNormalize MagDataToQuaternion::calculateQuaternion(float magDataX, float magDataY, float magDataZ, float earthMagRefX, float earthMagRefY, float earthMagRefZ) {
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
    qem.w = cos(theta / 2);
    qem.x = vx * sin(theta / 2);    
    qem.y = vy * sin(theta / 2);
    qem.z = vz * sin(theta / 2);    
    return qem;
}

VectorToNormalize MagDataToQuaternion::adjustOrientationToReference(qemRef) {
    
    return VectorToNormalize();
}
