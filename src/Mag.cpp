#include "Mag.h"
#include <Wire.h>
#include <Arduino.h>

DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);

Mag::Mag() {

}