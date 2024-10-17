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


/*Mag::Mag(const BiasMatrix &biasMatrixInstance, const ScaleMatrix &scaleMatrixInstance, const CombinedMatrix &combinedMatrixInstance)
: localBiasMatrix(biasMatrixInstance),localScaleMatrix(scaleMatrixInstance), localCombinedMatrix(combinedMatrixInstance) {
}*/
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
void Mag::Normalize(double &x, double &y, double &z) {
  double length = sqrt (x * x + y * y + z * z);
    if (length != 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}
MagStructure Mag::Calibration() { //calibrated data
    sBmm150MagData_t magData = bmm150.getGeomagneticData();
    double raw[3] = {magData.x, magData.y, magData.z};
    double biasedData[3];
    double corrected[3];
    //double calibrated[3];
    MagStructure calibrated;
    //tu poniżej trzeba sprawdzić która wersja jest bardziej wydajna (optymalizacja w google):
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
    
    //Mag::Normalize(calibrated[0], calibrated[1], calibrated[2]);
   
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
  return calibrated;
}
/*
QuaternionEarthMatrix Mag::MagQuaternizer()
{
   MagDataToQuaternion MDTQ;
  VectorStructure earthMagneticVectorRefStructure; 
  //data from reference earth magnetic field vector 
  earthMagneticVectorRefStructure.x = 19720.3 / 1000;
  earthMagneticVectorRefStructure.y = 2191.0 / 1000;
  earthMagneticVectorRefStructure.z = 45805.4 / 1000;
  
  Mag magPrime;
  MagStructure magPrimeStructure = magPrime.Calibration();
  QuaternionEarthMatrix qem = MDTQ.calculateQuaternion(earthMagneticVectorRefStructure.x, earthMagneticVectorRefStructure.y, earthMagneticVectorRefStructure.z, magPrimeStructure);
  QuaternionEarthMatrix finalQuaternion = MDTQ.adjustOrientationToReference(qem);

  
  Serial.print("Final Quaternion: ");
  Serial.print(finalQuaternion.w);
  Serial.print(", ");
  Serial.print(finalQuaternion.x);
  Serial.print(", ");
  Serial.print(finalQuaternion.y);
  Serial.print(", ");
  Serial.print(finalQuaternion.z);
  
  Serial.print(", ");
  Serial.print(magPrimeStructure.x);
  Serial.print(", ");
  Serial.print(magPrimeStructure.y);
  Serial.print(", ");
  Serial.println(magPrimeStructure.z);
  
  
  return finalQuaternion;
}
*/

