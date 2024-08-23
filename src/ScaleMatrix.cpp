#include "ScaleMatrix.h"
//matrix with scale calibration data, native matrix to calibrate magnetic vector's data

//its CORRECTION MATRIX
const ScaleMatrix globalScaleMatrix = {
    {
        {1.194463, 0.000799, 0.002650},
        {0.000799, 1.052105, 0.001479},
        {0.002650, 0.001479, 1.081255}
    }
};