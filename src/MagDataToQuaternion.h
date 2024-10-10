#ifndef MAGDATATOQUATERNION_H
#define MAGDATATOQUATERNION_H

#include "Mag.h"
#include "QuaternionEarthMatrix.h"
#include "VectorToNormalize.h"
#include <math.h>
#include "SensorData.h"
#include "MagStructure.h"

class MagDataToQuaternion {
    private:
        QuaternionEarthMatrix qem;
        
        QuaternionEarthMatrix qemFinal;
        VectorToNormalize EarthMagneticVector;
        VectorToNormalize MagnetometerDataVector;
        
        
    public:
        MagDataToQuaternion();
        void normalize(double& x, double& y, double& z);
        QuaternionEarthMatrix calculateQuaternion(double earthMagRefX, double earthMagRefY, double earthMagRefZ, MagStructure& magPrimeStructure);
        QuaternionEarthMatrix adjustOrientationToReference(QuaternionEarthMatrix qemRef);
};   
#endif