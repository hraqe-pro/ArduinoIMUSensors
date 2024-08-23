#ifndef MAGDATATOQUATERNION_H
#define  MAGDATATOQUATERNION_H

#include "Mag.h"
#include "QuaternionEarthMatrix.h"
#include "VectorToNormalize.h"

class MagDataToQuaternion {
    private:
        QuaternionEarthMatrix qem;
        QuaternionEarthMatrix qemRef;
        VectorToNormalize EarthMagneticVector;
        VectorToNormalize MagnetometerDataVector;
        
    public:
        MagDataToQuaternion();
        void normalize(&float x, &float y, &float z);
        VectorToNormalize calculateQuaternion(float magDataX, float magDataY, float magDataZ, float earthMagRefX, float earthMagRefY, float earthMagRefZ);
        VectorToNormalize adjustOrientationToReference(qemRef);
}   
#endif