#ifndef MAGDATATOQUATERNION_H
#define MAGDATATOQUATERNION_H

#include "Mag.h"
#include "QuaternionEarthMatrix.h"
#include "VectorToNormalize.h"
#include <math.h>

class MagDataToQuaternion {
    private:
        QuaternionEarthMatrix qem;
        
        QuaternionEarthMatrix qemFinal;
        VectorToNormalize EarthMagneticVector;
        VectorToNormalize MagnetometerDataVector;
        
    public:
        MagDataToQuaternion();
        void normalize(float& x, float& y, float& z);
        QuaternionEarthMatrix calculateQuaternion(float magDataX, float magDataY, float magDataZ, float earthMagRefX, float earthMagRefY, float earthMagRefZ);
        QuaternionEarthMatrix adjustOrientationToReference(QuaternionEarthMatrix qemRef);
};   
#endif