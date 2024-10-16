#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM6DSOX.h>
#include "SensorData.h"
#include "Mag.h"
#include <Wire.h>
#include <VMA430_GPS.h>
#include "DFRobot_BMM150.h"
#include "pico/multicore.h"
#include "Gyro.h"


//DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4); //mag do wyjebania

#define LSM6DSOX_ADRESS 0x6A
#define CTRL1_A 0x10 //accel
#define CTRL2_G 0x11 //gyro
#define yup 1
#define nope 0

//mag do usunięcia
/*
#if defined(ESP32) || defined(ESP8266)
  #define BMM150_CS D3
#elif defined(__AVR__) || defined(ARDUINO_SAM_ZERO)
  #define BMM150_CS 3
#elif (defined NRF5)
  #define BMM150_CS 2  //The corresponding silkscreen on the development board is the pin P2
#endif*/
Mag mag;

Gyro gyro;


struct IntSensorData {
  int64_t ax;
  int64_t ay;
  int64_t az;
  int64_t gx;
  int64_t gy;
  int64_t gz; 
  int64_t magx;
  int64_t magy;
  int64_t magz;
  int64_t magN; 
  int64_t th;
  int64_t tm;
  int64_t ts;
  int64_t lati;
  int64_t loni;
  
  
};
BLEService sensorService("180D"); //service UUID
BLECharacteristic sensorCharacteristic("2A37", BLERead | BLENotify, sizeof(IntSensorData)); //characteristic UUID

//gps datas
VMA430_GPS gps(&Serial1);
UBX_msg ubxmsg;
static const uint32_t GPSBaud = 9600;

/// pressure sensor datas
const float  OffSet = 0.515 ;
float V, P;


void setup() {
  Serial.begin(115200);

  //mag
  /*
  while(bmm150.begin()){
    Serial.println("bmm150 init failed, Please try again!");
    delay(1000);
  } Serial.println("bmm150 init success!");
  bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);
  bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY);
  bmm150.setRate(BMM150_DATA_RATE_30HZ);
  bmm150.setMeasurementXYZ();*/
  
  mag.begin();
  


  //gps 
  Serial1.begin(GPSBaud);
  gps.begin(GPSBaud);
  gps.setUBXNav();
  gps.DataRefreshRate = F4Hz;


   //while (!Serial);
  analogReadResolution(12);

  if (!BLE.begin()) {
    Serial.println("!Initialization nope");
    while(yup);
  }
  BLE.setLocalName("dremora");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorCharacteristic);
  BLE.addService(sensorService);

  Wire.begin();
  Wire.beginTransmission(LSM6DSOX_ADRESS);
  Wire.write(CTRL1_A);
  Wire.write(0xA0);
  Wire.endTransmission();

  Wire.beginTransmission(LSM6DSOX_ADRESS);
  Wire.write(CTRL2_G);
  Wire.write(0xA0);
  Wire.endTransmission();



  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (yup);
  }
  //if (!IMU.accelerationSampleRate() || !IMU.gyroscopeSampleRate()) {
  //  Serial.println("Failed to set ODR to 6.66 kHz!");
  //  while(yup);
  //  delay(10000);
  //}

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

  /*
  int sensorValue = analogRead(0);     //Sensor output voltage
  float V = sensorValue * (3.284748 / 4095);

  P = (V * 1.369044 - OffSet) * 250;             //Calculate water pressure

  Serial.print("Voltage:");
  Serial.print(V, 3);
  Serial.println("V");

  Serial.print(" Pressure:");
  Serial.print(P, 5);
  Serial.println(" KPa");
  Serial.println();

  delay(500);*/
//UBX_msg ubx_msg;
//ubx_msg

//gps.sendUBX();

BLEDevice central = BLE.central();

if (central) {
  Serial.print("connected with central device: ");
  Serial.println(central.address());

  while (central.connected()) {
    SensorData data;
    
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      //imu
      float xa, ya, za;
      float xg, yg, zg;
      IMU.readAcceleration(xa, ya, za);
      IMU.readGyroscope(xg, yg, zg);

      

      //accel & gyro
      data.ax = xa;
      data.ay = ya;
      data.az = za;
      data.gx = xg;
      data.gy = yg;
      data.gz = zg;

      //mag
      /*sBmm150MagData_t magData = bmm150.getGeomagneticData();
      float compassDegree = bmm150.getCompassDegree();
      data.magx = magData.x;
      data.magy = magData.y;      
      data.magz = magData.z;
      data.magN = compassDegree;*/
      
      //mag.DataReader(data);
      mag.Calibration();
      //mag.MagQuaternizer();
      
      //gps
      if (gps.getUBX_packet()) {
        gps.parse_ubx_data();
        if (gps.utc_time.valid)
        {
          data.th = gps.utc_time.hour;
          data.tm = gps.utc_time.minute;
          data.ts = gps.utc_time.second;
          
        }
      }
      data.lati = gps.location.latitude;
      data.loni = gps.location.longitude;
      if (data.lati <= 0.0 || data.loni <= 0.0) {
        gps.setUBXNav();
        gps.DataRefreshRate = F4Hz;
      }
      //std::string test;
      //test = byte<byte>(ubxmsg.msg);
      
      
      
      IntSensorData intSensorData;
      intSensorData.ax = static_cast<int64_t>(data.ax * 100000000);
      intSensorData.ay = static_cast<int64_t>(data.ay * 100000000);
      intSensorData.az = static_cast<int64_t>(data.az * 100000000);
      intSensorData.gx = static_cast<int64_t>(data.gx * 100000000);
      intSensorData.gy = static_cast<int64_t>(data.gy * 100000000);
      intSensorData.gz = static_cast<int64_t>(data.gz * 100000000);
      intSensorData.magx = static_cast<int64_t>(data.magx * 100000000);
      intSensorData.magy = static_cast<int64_t>(data.magy * 100000000);
      intSensorData.magz = static_cast<int64_t>(data.magz * 100000000);
      intSensorData.magN = static_cast<int64_t>(data.magN * 100000000);      
      intSensorData.th = static_cast<int64_t>(data.th * 100000000);       
      intSensorData.tm = static_cast<int64_t>(data.tm * 100000000);
      intSensorData.ts = static_cast<int64_t>(data.ts * 100000000);
      intSensorData.lati = static_cast<int64_t>(data.lati * 100000000);
      intSensorData.loni = static_cast<int64_t>(data.loni * 100000000);
/*
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
      Serial.print(data.az);
      Serial.print('\t');
      Serial.print(data.th);
      Serial.print('\t');
      Serial.print(data.tm);
      Serial.print('\t');
      Serial.print(data.ts);
      Serial.print('\t');
      Serial.print(data.lati);
      Serial.print('\t');     
      Serial.print(data.loni);
      Serial.print("\t gowno: Latitude");
      Serial.print(gps.location.latitude, 8);
      Serial.print("\t Longitude");
      Serial.print(gps.location.longitude, 8);
      Serial.print("\t test"); */

      Serial.print(intSensorData.gx);
      Serial.print('\t');
      Serial.print(intSensorData.gy);
      Serial.print('\t');
      Serial.print(intSensorData.gz);
      Serial.print('\t');
      Serial.print(intSensorData.ax);
      Serial.print('\t');
      Serial.print(intSensorData.ay);
      Serial.print('\t');
      Serial.print(intSensorData.az);
      Serial.print('\t');
      Serial.print(intSensorData.magx);
      Serial.print('\t');
      Serial.print(intSensorData.magy);
      Serial.print('\t');
      Serial.print(intSensorData.magz);
      Serial.print('\t');
      Serial.print("mag: ");
      Serial.print(intSensorData.magN);
      Serial.print('\t');
      Serial.print(intSensorData.th);
      Serial.print('\t');
      Serial.print(intSensorData.tm);
      Serial.print('\t');
      Serial.print(intSensorData.ts);
      Serial.print('\t');
      Serial.print(intSensorData.lati);
      Serial.print('\t');     
      Serial.println(intSensorData.loni);
      //Serial.print('\t');
      //Serial.println();

      //byte dataBytes[sizeof(SensorData)];
      //memcpy(dataBytes, &data, sizeof(SensorData));

      //Serial.print("data in byte format: ");
      //for(size_t i = nope; i < sizeof(SensorData); i++) {
      //  Serial.print(data[i], HEX);
      //  Serial.print(" ");
      //}
      //Serial.println();

      char *test = reinterpret_cast<char*>(&intSensorData);
      bool success = sensorCharacteristic.writeValue(&intSensorData, 120);
      if (success) {
        
        Serial.println("data been send, data size: ");
        Serial.println(sizeof(intSensorData));

        SensorData receivedData;
        sensorCharacteristic.readValue((char*)&receivedData, sizeof(receivedData));
      /*
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
        Serial.print(receivedData.az);
        Serial.print('\t');
        Serial.print(receivedData.lati);
        Serial.println(receivedData.loni);*/

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
















