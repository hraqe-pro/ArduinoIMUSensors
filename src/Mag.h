#ifndef MAG_H
#define MAG_H

#include <DFRobot_BMM150.h>
#include "ScaleMatrix.h"
#include "CombinedMatrix.h"
#include "BiasMatrix.h"
#include "ScaleMatrix.h"
#include "CombinedMatrix.h"
#include "SensorData.h"
#include "MagDataToQuaternion.h"
#include "MagStructure.h"
#include "VectorStructure.h"
#include "QuaternionEarthMatrix.h"

class Mag {
private:
    BiasMatrix localBiasMatrix;
    ScaleMatrix localScaleMatrix;
    CombinedMatrix localCombinedMatrix;
    
    
    //DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);
public: 
    //Mag(const BiasMatrix& biasMatrixInstance, const ScaleMatrix& scaleMatrixInstance, const CombinedMatrix& combinedMatrixInstance);
    Mag();
    void begin();
    void DataReader(SensorData& data);
    MagStructure Calibration();
    //tu trzeba zrobić operacje liczbowe do liczenia kalibracji za pomoca naszych struktur
    QuaternionEarthMatrix MagQuaternizer();
    void Normalize(double &x, double &y, double &z);
};
#endif