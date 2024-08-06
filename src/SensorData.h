#ifndef SENSORDATA_H
#define SENSORDATA_H

//char structure for sending data by BLE


struct SensorData {
    double ax;
    double ay;
    double az;
    double gx;
    double gy;
    double gz;
    double magx;
    double magy;
    double magz;
    double magN;
    double th;
    double tm;
    double ts;
    double lati;
    double loni;
};
#endif //SENSORDATA_H