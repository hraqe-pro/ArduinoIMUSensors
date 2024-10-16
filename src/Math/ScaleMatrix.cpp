#include "ScaleMatrix.h"
//matrix with scale calibration data, native matrix to calibrate magnetic vector's data

//its CORRECTION MATRIX
const ScaleMatrix globalScaleMatrix = {
    {
        {1.342155, -0.047647, -0.016601},
        {-0.047647, 1.145543, 0.004342},
        {-0.016601, 0.004342, 1.173441}
    }
};