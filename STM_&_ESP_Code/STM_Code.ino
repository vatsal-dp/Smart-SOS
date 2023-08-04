#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include<HardwareSerial.h>
HardwareSerial Serial1(PA10,PA9);
#define REPORTING_PERIOD_MS     100
#define I2C_ADDR  2


PulseOximeter pox;

uint32_t tsLastReport = 0;


/*void onBeatDetected() {
    Serial.println("Beat!");
}*/

void setup() {
  Serial1.begin(9600);
    Serial.begin(9600);
    Wire.setSDA(PB9);
    Wire.setSCL(PB8);
    Wire.begin();
    Serial.print("Initializing sensor..");

    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

   // pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {

    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS && ((pox.getHeartRate()>0 && pox.getSpO2()!=0)||
            (pox.getHeartRate()==0 && pox.getSpO2()==0)) && pox.getHeartRate()<220) {
        String reading= "";
        String hr="",sp="";
        Serial.print("Heart rate:");
        hr=String((int)pox.getHeartRate());
        Serial.println((int)pox.getHeartRate());
        //Serial1.print("\n");
        Serial.print("bpm / SpO2:");
        sp=String(pox.getSpO2());
        Serial.println(pox.getSpO2());
        reading=hr+"\r"+sp+"\n";
        Serial1.print(reading);
       // Serial1.print("\n");
        
        //Serial.println("%");

        tsLastReport = millis();
    }
}
