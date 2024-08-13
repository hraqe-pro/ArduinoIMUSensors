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

void Mag::begin() {
    //tutaj jakis return od bmm ale trzeba pomyslec, narazie wrzucam kopie tego wywołania z maina
    while(bmm150.begin()){
        Serial.println("bmm150 init failed, Please try again!");
        delay(1000);
    }   Serial.println("bmm150 init success!");
    bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);
    bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY);
    bmm150.setRate(BMM150_DATA_RATE_30HZ);
    bmm150.setMeasurementXYZ();
}

void Mag::DataReader(SensorData& data, double x, double y, double z) {
    sBmm150MagData_t magData = bmm150.getGeomagneticData();
    
    data.magx = magData.x;
    data.magy = magData.y;      
    data.magz = magData.z;
}