#include <Arduino.h>
#include "Serial.h"

void exec(char *cmdline)
{
    char *command = strsep(&cmdline, " ");

    if (strcmp_P(command, PSTR("help")) == 0) {
        Serial.println(F(
            "mode <pin> <mode>: pinMode()\r\n"
            "read <pin>: digitalRead()\r\n"
            "aread <pin>: analogRead()\r\n"
            "write <pin> <value>: digitalWrite()\r\n"
            "awrite <pin> <value>: analogWrite()\r\n"
            "echo <value>: set echo off (0) or on (1)"));
    } else if (strcmp_P(command, PSTR("mode")) == 0) {
        int pin = atoi(strsep(&cmdline, " "));
        int mode = atoi(cmdline);
        pinMode(pin, mode);
    } else if (strcmp_P(command, PSTR("read")) == 0) {
        int pin = atoi(cmdline);
        Serial.println(digitalRead(pin));
    } else if (strcmp_P(command, PSTR("aread")) == 0) {
        int pin = atoi(cmdline);
        Serial.println(analogRead(pin));
    } else if (strcmp_P(command, PSTR("write")) == 0) {
        int pin = atoi(strsep(&cmdline, " "));
        int value = atoi(cmdline);
        digitalWrite(pin, value);
    } else if (strcmp_P(command, PSTR("awrite")) == 0) {
        int pin = atoi(strsep(&cmdline, " "));
        int value = atoi(cmdline);
        analogWrite(pin, value);
    } else if (strcmp_P(command, PSTR("echo")) == 0) {
        do_echo = atoi(cmdline);
    } else {
        Serial.print(F("Error: Unknown command: "));
        Serial.println(command);
    }
}

void processCom()
{
    /* Process incoming commands. */
    while (Serial.available()) {
        static char buffer[BUF_LENGTH];
        static int length = 0;

        int data = Serial.read();
        if (data == '\b' || data == '\177') {  // BS and DEL
            if (length) {
                length--;
                if (do_echo) Serial.write("\b \b");
            }
        }
        else if (data == '\r') {
            if (do_echo) Serial.write("\r\n");    // output CRLF
            buffer[length] = '\0';
            if (length) exec(buffer);
            length = 0;
        }
        else if (length < BUF_LENGTH - 1) {
            buffer[length++] = data;
            if (do_echo) Serial.write(data);
        }
    }
}

