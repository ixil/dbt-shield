#pragma once

#include <Arduino.h>
#include "Pins.h"
//#include <SparkFunMAX31855k.h>
#include <SPI.h>

static double tempc = 0;

void updateTemp();
bool isHot();