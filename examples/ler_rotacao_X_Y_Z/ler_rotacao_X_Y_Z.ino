#include "Wire.h"
#include "giroscopio.h"

giroscopio giro;
float x=0;
float y=0;
float z=0;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    Serial.begin(115000);
    Serial.println(F("Initializing giroscopio..."));
    giro.init();
        
}
// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

  static int count=0;
  if(giro.mpuInterruptActive()){
    giro.ler(z,y,x);
    giro.clear_mpuInterrupt();
    
    Serial.print("rot zyx\t");
    Serial.print(z * 180/M_PI);
    Serial.print("\t");
    Serial.print(y * 180/M_PI);
    Serial.print("\t");
    Serial.println(x * 180/M_PI);
    Serial.print("\t");
    
  }
}  
