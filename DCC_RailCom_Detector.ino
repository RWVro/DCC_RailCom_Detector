
//====================================== DCC RailCom Detector Program ================================
//
//
//=================================  Board = ESP32 Dev Module (NodeMCU ESP32) =========================
//
//
//=================================== ESP32 with L298N motorcontrollermodule =========================

#define RPWM 19                 // to RPWM MotorShield pin 1 & 4
#define LPWM 18                 // to LPWM MotorShield pin 2 & 3

#include "Detector.h"
#include "Pom.h"

int loopCount;

//================================= Setup =======================================

void setup()
{
  Serial.begin(500000);
  delay(3000);

  pinMode(RPWM, OUTPUT);              // to RPWM MotorShield pin 1 & 4
  pinMode(LPWM, OUTPUT);              // to LPWM MotorShield pin 2 & 3

  digitalWrite(RPWM, LOW);
  digitalWrite(LPWM, LOW);

  pinMode(railComInt, INPUT);         // GPIO15 input pin  int Raicom Detector
  pinMode(railComRX, INPUT);          // GPIO16 Connected to RailCom Detector RX

  delay(3000);
  Serial.println("Program Started!");
  Serial.println("");
  
  Poort2.begin(250000, SERIAL_8N1, railComRX, 17);    // Define and start ESP32 serial port
  delay(3000);
}

//======================================= Main Program ==============================

void loop()
{
  basicNormalPulses(10);
  
  if (loopCount < 50)
  {
    dccIdlePackets(5);
    readLocAddress();
    loopCount++;
  }
}
