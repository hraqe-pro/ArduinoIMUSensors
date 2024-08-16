#include "Mag.h"
#include <Wire.h>
#include <Arduino.h>

DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);

#if defined(ESP32) || defined(ESP8266)
  #define BMM150_CS D3
#elif defined(__AVR__) || defined(ARDUINO_SAM_ZERO)
  #define BMM150_CS 3
#elif (defined NRF5)
  #define BMM150_CS 2  //The corresponding silkscreen on the development board is the pin P2
#endif


Mag::Mag(const BiasMatrix &biasMatrixInstance, const ScaleMatrix &scaleMatrixInstance, const CombinedMatrix &combinedMatrixInstance)
: localBiasMatrix(biasMatrixInstance),localScaleMatrix(scaleMatrixInstance), localCombinedMatrix(combinedMatrixInstance) {
}
Mag::Mag(){
}

void Mag::begin() {
    //działa wszystko teraz
    while(bmm150.begin()){
        Serial.println("bmm150 init failed, Please try again!");
        delay(1000);
    }   Serial.println("bmm150 init success!");
    bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);
    bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY);
    bmm150.setRate(BMM150_DATA_RATE_30HZ);
    bmm150.setMeasurementXYZ();
}

//używaj albo DataReader albo Calibration, nigdy obu naraz
void Mag::DataReader(SensorData& data) { //data without calibration
    sBmm150MagData_t magData = bmm150.getGeomagneticData();
    
    data.magx = magData.x;
    data.magy = magData.y;      
    data.magz = magData.z;
}

void Mag::Calibration(SensorData& data) { //calibrated data
    sBmm150MagData_t magData = bmm150.getGeomagneticData();
    double raw[3] = {magData.x, magData.y, magData.z};
    double biasedData[3];
    double corrected[3];
    double calibrated[3];

    for (size_t i = 0; i < 3; i++)
    {
      biasedData[i] = raw[i] - globalBiasMatrix.biasMatrix[i];
    }
    for (size_t i = 0; i < 3; i++)
    {
      corrected[i] = 0;
      for (size_t j = 0; j < 3; j++)
      {
        corrected[i] += biasedData[j] * globalScaleMatrix.scaleMatrix[i][j];
      }
    }
    for (size_t i = 0; i < 3; i++)
    {
      calibrated[i] = 0;
      for (size_t j = 0; j < 3; j++)
      {
        calibrated[i] += corrected[j] * globalCombinedMatrix.combinedMatrix[i][j];
      }
    }
    /*for (size_t i = 0; i < 3; i++)
    {
      corrected[i] = 0;
      for (size_t j = 0; j < 3; j++)
      {
        corrected[i] += globalScaleMatrix.scaleMatrix[i][j] * (raw[j] - globalBiasMatrix.biasMatrix[j]);
      }
      corrected[i] *= globalCombinedMatrix.combinedMatrix[i][i];
    }*/
    data.magx = magData.x;
    data.magy = magData.y;      
    data.magz = magData.z;
    Serial.print("RAW: ");
    Serial.print(raw[0]);
    Serial.print(",");
    Serial.print(raw[1]);
    Serial.print(",");
    Serial.print(raw[2]);
    Serial.print(" | CORRECTED: ");
    Serial.print(calibrated[0]);
    Serial.print(",");
    Serial.print(calibrated[1]);
    Serial.print(",");
    Serial.println(calibrated[2]);

}
