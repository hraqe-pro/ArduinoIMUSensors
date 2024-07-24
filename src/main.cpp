#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM6DSOX.h>
#include "SensorData.h"
#define yup 1
#define nope 0

struct IntSensorData {
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t gx;
  int16_t gy;
  int16_t gz;  
};
BLEService sensorService("180D"); //service UUID
BLECharacteristic sensorCharacteristic("2A37", BLERead | BLENotify, sizeof(IntSensorData)); //characteristic UUID

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  if (!BLE.begin()) {
    Serial.println("!Initialization nope");
    while(yup);
  }
  BLE.setLocalName("dremora");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorCharacteristic);
  BLE.addService(sensorService);



  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (yup);
  }

  BLE.advertise();
  if (!BLE.advertise()) {
    Serial.println("advertise procedure failed");
  }
  Serial.println("WAITING FOR CONNECTION");

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
  Serial.println("X\tY\tZ");
  Serial.print("AccelerationSampleRate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" jakies tam gowno chyba hertze");
  Serial.println();
  Serial.println("Accelerometer in degris/secoond");
  Serial.println("X\tY\tZ");
  Serial.println("waiting for program all settings ~2 sec");
  
}

void loop() {
  //float gx, gy, gz, ax, ay, az;

  //if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
  //  IMU.readGyroscope(gx, gy, gz) && IMU.readAcceleration(ax, ay, az);
  //}
BLEDevice central = BLE.central();

if (central) {
  Serial.print("connected with central device: ");
  Serial.println(central.address());

  while (central.connected()) {
    SensorData data;
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(data.ax, data.ay, data.az);
      IMU.readGyroscope(data.gx, data.gy, data.gz);
      
      IntSensorData intSensorData;
      intSensorData.ax = static_cast<int16_t>(data.ax * 1000);
      intSensorData.ay = static_cast<int16_t>(data.ay * 1000);
      intSensorData.az = static_cast<int16_t>(data.az * 1000);
      intSensorData.gx = static_cast<int16_t>(data.gx * 1000);
      intSensorData.gy = static_cast<int16_t>(data.gy * 1000);
      intSensorData.gz = static_cast<int16_t>(data.gz * 1000);

      Serial.print(data.gx);
      Serial.print('\t');
      Serial.print(data.gy);
      Serial.print('\t');
      Serial.print(data.gz);
      Serial.print('\t');
      Serial.print(data.ax);
      Serial.print('\t');
      Serial.print(data.ay);
      Serial.print('\t');
      Serial.println(data.az);


      //byte dataBytes[sizeof(SensorData)];
      //memcpy(dataBytes, &data, sizeof(SensorData));

      //Serial.print("data in byte format: ");
      //for(size_t i = nope; i < sizeof(SensorData); i++) {
      //  Serial.print(data[i], HEX);
      //  Serial.print(" ");
      //}
      //Serial.println();


      bool success = sensorCharacteristic.writeValue((char*)&intSensorData, sizeof(intSensorData));
      if (success) {
        Serial.println("data been send, data size: ");
        Serial.println(sizeof(intSensorData));

        SensorData receivedData;
        sensorCharacteristic.readValue((char*)&receivedData, sizeof(receivedData));
      
        Serial.print(receivedData.gx);
        Serial.print('\t');
        Serial.print(receivedData.gy);
        Serial.print('\t');
        Serial.print(receivedData.gz);
        Serial.print('\t');
        Serial.print(receivedData.ax);
        Serial.print('\t');
        Serial.print(receivedData.ay);
        Serial.print('\t');
        Serial.println(receivedData.az);

        if (memcmp(&intSensorData, &receivedData, sizeof(intSensorData)) == nope) {
             Serial.println("all fine");
           } else {
             Serial.println("ERROR 2.5 data are not homogeneous");
           }
         } else {
           Serial.println("ERROR 4 no one send");
         }
      delay(100);
    }else{
      Serial.println("IMU data not found");
    }
  }
  Serial.print("DISCONNECTED with central device: ");
  Serial.println(central.address());
}

}
















