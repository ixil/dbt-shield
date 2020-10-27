#include "Serial.h"
#include "Heater.h"
#include <avr/pgmspace.h>


void setup()
{
    Serial.begin(9600);
}

void loop()
{
    processCom();
}