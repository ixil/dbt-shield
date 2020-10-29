#include "Tsens.h"


void updateTemp()
{
    //tempc++;
    //Serial.println(tempc);
}

bool isHot()
{
    if(tempc > 1000){
        tempc = 0;
        return true;
    }
    return false;
}
