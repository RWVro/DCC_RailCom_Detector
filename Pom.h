//

byte LocAddressByte = 0;              // Loc address
byte LocInstructionByte = 0;          // Instruction
byte LocCVAddressByte = 0;            // CV Address = CV Number from NMRA list minus 1
byte LocCVValueByte = 0;              // When reading instruction this remains 0
byte LocErrorByte = 0;                // Error byte

//===================================== functions to create pulses =========================================

void dccOnePulse()
{
  // 58 Microseconds High
  digitalWrite(LPWM, LOW);          // LPWM LOW
  digitalWrite(RPWM, HIGH);         // RPWM HIGH
  delayMicroseconds(58);

  // 58 Microseconds Low
  digitalWrite(RPWM, LOW);          // RPWM LOW
  digitalWrite(LPWM, HIGH);         // LPWM HIGH
  delayMicroseconds(58);
}

void dccZeroPulse()                 // Zero pulse on CH A (Main Track)
{
  // 100 Microseconds High
  digitalWrite(LPWM, LOW);          // LPWM LOW
  digitalWrite(RPWM, HIGH);         // RPWM HIGH
  delayMicroseconds(100);

  // 100 Microseconds Low
  digitalWrite(RPWM, LOW);          // RPWM LOW
  digitalWrite(LPWM, HIGH);         // LPWM HIGH
  delayMicroseconds(100);
}

void basicNormalPulses(int value)
{
  for (byte x = 0; x < value; x++)
  {
    dccZeroPulse();
    dccOnePulse();
    dccZeroPulse();
    dccOnePulse();
    dccZeroPulse();
    dccOnePulse();
    dccZeroPulse();
    dccOnePulse();
    dccZeroPulse();
    dccOnePulse();
  }
}

//===================================== Convert Binary Digits to Pulses ================

void dccBitsToPulses(boolean digit)
{
  if (digit == 1) {
    dccOnePulse();
  }
  else dccZeroPulse();
}

//========================================== Packet Instruction =======================================

void sendDCCPreamble()
{
  for (byte k = 0; k < 14; k++)       // Preamble = 14 x dccOnePulse
  {
    dccOnePulse();
  }
}

//=============================================== Idle Packets ==============================

void dccEightOnes()
{
  for (byte k = 0; k < 8; k++)
  {
    dccOnePulse();
  }
}

void dccEightZeros()
{
  for (byte k = 0; k < 8; k++)
  {
    dccZeroPulse();
  }
}

void dccIdlePackets(int value)
{
  for (int x = 0; x < value; x++)
  {
    sendDCCPreamble();
    dccZeroPulse();     // start bit
    dccEightOnes();
    dccZeroPulse();     // start bit
    dccEightZeros();
    dccZeroPulse();     // start bit
    dccEightOnes();
    dccOnePulse();      // end pulse
  }
}

void sendLocAddress()
{
  for (int k = 7; k >= 0; k--)
  {
    dccBitsToPulses(bitRead(LocAddressByte, k));
  }
}
void sendLocInstruction()
{
  for (int k = 7; k >= 0; k--)
  {
    dccBitsToPulses(bitRead(LocInstructionByte, k));
  }
}

void sendLocCVNumber()
{
  for (int k = 7; k >= 0; k--)
  {
    dccBitsToPulses(bitRead(LocCVAddressByte, k));
  }
}

void sendLocCVValue()
{
  for (int k = 7; k >= 0; k--)
  {
    dccBitsToPulses(bitRead(LocCVValueByte, k));
  }
}

void sendLocErrorDetection()
{
  for (int k = 7; k >= 0; k--)
  {
    dccBitsToPulses( bitRead(LocErrorByte , k));
  }
}

void LocCutOutPulse()
{
  attachInterrupt((railComInt), GPIO15ToLow, FALLING);

  digitalWrite(LPWM, LOW);          // LPWM LOW
  digitalWrite(RPWM, HIGH);         // RPWM HIGH
  delayMicroseconds(26.5);          // Cutout Start Tcs   (NRMA s-9.3.2_2012_12_10(Basic Decoder))
  digitalWrite(RPWM, LOW);

  delayMicroseconds(444);           // Cutout time  Tce - Tcs

  digitalWrite(RPWM, LOW);          // RPWM LOW
  digitalWrite(LPWM, HIGH);         // LPWM HIGH
  delayMicroseconds(27);

  detachInterrupt(digitalPinToInterrupt(railComInt));
}

//================================== Send Loc Read Packet ====================

void readLocAddress()
{
  sendDCCPreamble();
  dccZeroPulse();           // start bit
  sendLocAddress();
  dccZeroPulse();
  sendLocInstruction();
  dccZeroPulse();           // start bit
  sendLocCVNumber();
  dccZeroPulse();           // start bit
  sendLocCVValue();
  dccZeroPulse();
  sendLocErrorDetection();
  dccOnePulse();            // Stop bit
  LocCutOutPulse();
}
