## Pinout
---
![pinout](https://github.com/hiiragii/dbt-shield/blob/master/images/MINIPINOUT.png)
The 2x16 pin terminal blocks on the right side are directly connected to the respective arduino pin (eg D0 on the terminal block is digital pin 0 on the arduino).

## Connections
---
+ Connect the 24v power supply wires to the top terminal.
+ Connect the Extruder optional object fan to OUT0 and OUT1.
+ Connect the thermistor leads to TH0+ and TH0-, polarity doesn't matter.
+ Connect the super-volcano heater to OUT3 and GND (first 2 pins on the terminal block).
+ Connect the first stepper coil to M1.1A and M1.1B. Connect the second coil to M1.2A and M1.2B. Polarity doesn't matter, you can reverse the direction of rotation directly in software or by swapping 2 leads of the same coil (eg. M1.1A and M1.1B).  
+ Lastly 

## Set-up
---