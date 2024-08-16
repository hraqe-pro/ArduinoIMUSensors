#ifndef MAG_H
#define MAG_H

#include <DFRobot_BMM150.h>
#include "ScaleMatrix.h"
#include "CombinedMatrix.h"
#include "BiasMatrix.h"
#include "ScaleMatrix.h"
#include "CombinedMatrix.h"
#include "SensorData.h"

class Mag {
private:
    BiasMatrix localBiasMatrix;
    ScaleMatrix localScaleMatrix;
    CombinedMatrix localCombinedMatrix;
    //DFRobot_BMM150_I2C bmm150;
public: 
    Mag(const BiasMatrix& biasMatrixInstance, const ScaleMatrix& scaleMatrixInstance, const CombinedMatrix& combinedMatrixInstance);
    Mag();
    void begin();
    void DataReader(SensorData& data);
    void Calibration(SensorData & data);
    //tu trzeba zrobić operacje liczbowe do liczenia kalibracji za pomoca naszych struktur

    //nasza nowa klasa nie ma dodanych żadnych zależnośći w main.cpp , należy nic tam nie dodawać ani nie usuwać zanim nie zostanie ukończona ta klasa.
};
#endif