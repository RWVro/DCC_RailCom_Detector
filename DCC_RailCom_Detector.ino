//
//          I have noticed that locomotives from some manufacturers
//          do not answer if you request the address with address 0.
//          The solution is as follows:
//
//          Increase the address by 1 and test again, stop when the address is found
//
//          Tested with Fleischmann locomotives and MiniTrix locomotives.
//
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

int loopCount = 0;

//String outputStr;

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
//
// Do not use delay() commands in the loop program.
// Otherwise there is a chance that the locomotive will quickly run off the rails.
// DCC signal then stops temporarily.

void loop()
{
  // Start testing with LocAddressByte = 0 

  basicNormalPulses(50);                                        // Create DCC signal

  while (loopCount < 20)
  {
    basicNormalPulses(10);                                      // Create DCC signal

    readLocValues();
    dccIdlePackets(1);
    readLocValues();
    dccIdlePackets(1);

    loopCount++;
  }

  if (!addressOK)                                             // Increase the address if the locomotive does not respond to address 0
  {
    LocAddressByte++;
    String tempStr;
    tempStr = "New Test Address=" + String(LocAddressByte);
    Serial.println(tempStr);
    loopCount = 0;
    return;
  }
}
