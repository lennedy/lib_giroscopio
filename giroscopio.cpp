#include "giroscopio.h"

giroscopio * giroscopio::instance;

giroscopio::giroscopio(){
  instance = this;
}

int giroscopio::init(){
    
    Wire.begin();
    //TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

 
    // initialize device
    
    mpu.initialize();
    if(!mpu.testConnection())
      return ERR_MPU6050_CONNECTION_FAILED; 
 //     Serial.println( ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
//    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
//    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == ERR_NO) {
        // turn on the DMP, now that it's ready
        //Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
//        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        //Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        //Serial.print(F("DMP Initialization failed (code "));
        //Serial.print(devStatus);
        //Serial.println(F(")"));
        return devStatus;
    }
    
    attachInterrupt(0, isr, RISING);
    return ERR_NO;
}

void giroscopio::isr(){
  instance->interrupcao();
}

void giroscopio::interrupcao(){ 
  // reset interrupt flag and get INT_STATUS byte
 
  mpuInterrupt = true;
}

void  giroscopio::ler(YPR &leitura){
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  mpuIntStatus = mpu.getIntStatus();    

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      Serial.println(F("FIFO overflow!"));
      
  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;

      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      leitura.yaw  = ypr[0];
      leitura.pitch= ypr[1];
      leitura.roll = ypr[2];
      
      /*
      Serial.print("ypr\t");
      Serial.print(leitura.yaw * 180/M_PI);
      Serial.print("\t");
      Serial.print(leitura.pitch * 180/M_PI);
      Serial.print("\t");
      Serial.println(leitura.roll * 180/M_PI);
      Serial.print("\t");
      */
        
 //     Serial.println(fifoCount);
    //  mpu.resetFIFO();
  }

}

void giroscopio::ler(float &x, float &y, float &z){
	YPR a;

	ler(a);
	x=a.roll;
	y=a.pitch;
	z=a.yaw;
}
