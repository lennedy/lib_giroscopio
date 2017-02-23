#include "Wire.h"
#include "giroscopio.h"

#define LED 13
giroscopio giro;
YPR ypr;

bool blinkState = false;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    Serial.begin(115000);
    Serial.println(F("Initializing giroscopio..."));
    giro.init();
    
    pinMode(LED, OUTPUT);   
    
}
// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

  static int count=0;
  if(giro.mpuInterruptActive()){
    giro.ler(ypr);
    blinkState = !blinkState;
    giro.clear_mpuInterrupt();
    digitalWrite(LED, blinkState);
    
    Serial.print("ypr\t");
    Serial.print(ypr.yaw * 180/M_PI);
    Serial.print("\t");
    Serial.print(ypr.pitch * 180/M_PI);
    Serial.print("\t");
    Serial.println(ypr.roll * 180/M_PI);
    Serial.print("\t");
    
  }
}  
