#ifndef SENSORDATA_H
#define SENSORDATA_H

//char structure for sending data by BLE


struct SensorData {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
};
#endif //SENSORDATA_H