
//====================================== Program Station + Detector Program ================================
//
//
//=================================  Board = ESP32 Dev Module (NodeMCU ESP32) =========================
//
//
//=================================== ESP32 with  L298N motorcontrollermodule =========================

#define idStr "ESP32 Program Station Connected!"
#define ONBOARD_LED 2

#define RPWM 19                 // to RPWM MotorShield pin 1 & 4
#define LPWM 18                 // to LPWM MotorShield pin 2 & 3

#include "Detector.h"
#include "DCC.h"
#include "AddressRead.h"

//=================================  VARIABLES =================================


//================================= Setup =======================================

void PrintFileName()
{
  Serial.println("");
  Serial.println(__FILE__" ("__DATE__") "__TIME__);
  Serial.println("");
}

//-----------------------------------------------

void setup()
{
  Serial.begin(500000);
  delay(1000);
  PrintFileName();

  // sets the digital pins as output or input
  //=====================================

  pinMode(RPWM, OUTPUT);          // to RPWM MotorShield pin 1 & 4
  pinMode(LPWM, OUTPUT);          // to LPWM MotorShield pin 2 & 3

  digitalWrite(RPWM, LOW);
  digitalWrite(LPWM, LOW);

  pinMode(ONBOARD_LED, OUTPUT);       // For test Blue Led
  digitalWrite(ONBOARD_LED, HIGH);

  pinMode(railComInt, INPUT);         // GPIO15 input pin  int Raicom Detector
  pinMode(railComRX, INPUT);          // GPIO16 Connected to RailCom Detector RX

  delay(1000);

  Serial.println("PROGRAM_STATION started!");

  pinMode(ONBOARD_LED, OUTPUT);           // For test   Blue Led
  digitalWrite(ONBOARD_LED, LOW);

  Poort2.begin(250000, SERIAL_8N1, railComRX, 17);                    // Define and start serial port
  delay(1000);
}

//======================================= Main Program ==============================

void loop()
{
  BasicNormalDccPulses(20);

  ReadToDetectLocAddress();               // Read to detect loc address
  FindAddressInData();

  if (SaveInfoString == "")             // Do not print empty lines
  {
    return;
  }
  Serial.println(SaveInfoString);       // If Xamarin is of print Loc Address =.. or No Loc on track!
}
