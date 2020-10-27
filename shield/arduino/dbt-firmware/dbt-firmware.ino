#include "Serial.h"
#include "Heater.h"
#include "Tsens.h"

#include <avr/pgmspace.h>


void setup()
{
    Serial.begin(115200);
}

void loop()
{
    processCom();
    updateTemp();
    if(isHot()){
        Serial.println("ITS HOT");
    }
}