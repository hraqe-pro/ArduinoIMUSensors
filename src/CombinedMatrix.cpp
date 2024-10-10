#include "CombinedMatrix.h"
//matrix with scale combined data to second calibrate (optional calibration method, in this case we use it native)

//its CALIBRATION MATRIX
const CombinedMatrix globalCombinedMatrix = {
    {
        {0.746300, 0.031002, 0.010444},
        {0.031002, 0.874248, -0.002796},
        {0.010444, -0.002796, 0.852353}
        
    }
};