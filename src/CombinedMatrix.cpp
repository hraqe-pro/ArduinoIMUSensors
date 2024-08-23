#include "CombinedMatrix.h"
//matrix with scale combined data to second calibrate (optional calibration method, in this case we use it native)

//its CALIBRATION MATRIX
const CombinedMatrix globalCombinedMatrix = {
    {
        {0.837202, -0.000633, -0.002051},
        {-0.000633, 0.950478, -0.001298},
        {-0.002051, -0.001298, 0.924858}
        
    }
};