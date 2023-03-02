
//========================================= Variables ====================================

byte locAddressByte = 0;                  // Loc address 0
byte locInstructionByte = 0;            // Loc Instruction
byte locCVNumberByte = 0;                 // Loc CV number (0 = CV1)
byte locCVValueByte = 0;                  // Loc CV Value
byte LocErrorByte = 0;                  // Loc Error Byte

int railComInt = 15;                      // GPIO15 input pin  int Raicom Detector


//======================================== DCC signal =====================================

void DccOnePulse()
{
  // 58 Microseconds High
  digitalWrite(LPWM, LOW);          // LPWM LOW
  digitalWrite(RPWM, HIGH);         // RPWM HIGH
  delayMicroseconds(57);

  // 58 Microseconds Low
  digitalWrite(RPWM, LOW);          // RPWM LOW
  digitalWrite(LPWM, HIGH);         // LPWM HIGH
  delayMicroseconds(57);
}

void DccZeroPulse()                 // Zero pulse on CH A (Main Track)
{
  // 100 Microseconds High
  digitalWrite(LPWM, LOW);          // LPWM LOW
  digitalWrite(RPWM, HIGH);         // RPWM HIGH
  delayMicroseconds(99);

  // 100 Microseconds Low
  digitalWrite(RPWM, LOW);          // RPWM LOW
  digitalWrite(LPWM, HIGH);         // LPWM HIGH
  delayMicroseconds(99);
}

//===================================== Binaire cijfers omzetten in pulsen ================

void DccBitsToPulses(boolean digit)
{
  if (digit == 1) {
    DccOnePulse();
  }
  else DccZeroPulse();
}

void DccEightOnes()
{
  for (byte k = 0; k < 8; k++)
  {
    DccOnePulse();
  }
}

void DccEightZeros()
{
  for (byte k = 0; k < 8; k++)
  {
    DccZeroPulse();
  }
}

//===================================== Send Preamble ===========================================

void SendDccPreamble()
{
  for (byte k = 0; k < 14; k++)           // Preamble = 14 x dccOnePulse
  {
    DccOnePulse();
  }
}

//===================================== Send Address ===========================================

void SendDccAddress()
{
  for (int k = 7; k >= 0; k--)
  {
    DccBitsToPulses(bitRead(locAddressByte , k));
  }
}

//===================================== Send Instruction ========================================

void SendDccInstruction()
{
  for (int k = 7; k >= 0; k--)
  {
    DccBitsToPulses(bitRead(locInstructionByte, k));
  }
}

//===================================== Send CVNumber ===========================================

void SendDccCVNumber()
{
  for (int k = 7; k >= 0; k--)
  {
    DccBitsToPulses(bitRead(locCVNumberByte, k));
  }
}

//===================================== Send CVValue ===========================================

void SendDccCVValue()
{
  for (int k = 7; k >= 0; k--)
  {
    DccBitsToPulses(bitRead(locCVValueByte, k));
  }
}

//===================================== Send Error Detection ===========================================

void SendDccErrorDetection()
{
  for (int k = 7; k >= 0; k--)
  {
    DccBitsToPulses( bitRead(LocErrorByte , k));
  }
}

//======================================== Calculate Error =======================================

void CalculateEOR()
{
  int ErrorByte;
  ErrorByte = locAddressByte ^ locInstructionByte ^ locCVNumberByte ^ locCVValueByte;
  LocErrorByte = ErrorByte;
}

//======================================== CutoutPulse ===========================================

void DccCutOutPulse()
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

//===================================== BasicNormalDccPulses ===================================

void BasicNormalDccPulses(int value)
{
  for (byte x = 0; x < value; x++)
  {
    DccZeroPulse();
    DccOnePulse();
  }
}

//======================================== Idle Packet ===========================================

void DccIdlePackets(int value)
{
  for (int x = 0; x < value; x++)
  {
    SendDccPreamble();
    DccZeroPulse();     // start bit
    DccEightOnes();
    DccZeroPulse();     // start bit
    DccEightZeros();
    DccZeroPulse();     // start bit
    DccEightOnes();
    DccOnePulse();      // end pulse
  }
}

//============================================= Reset Packets ====================


void DccResetPacket()
{
  SendDccPreamble();
  DccZeroPulse();         // start bit
  DccEightZeros();        // 8 x 0 bit
  DccZeroPulse();         // start bit
  DccEightZeros();        // 8 x 0 bit
  DccZeroPulse();         // start bit
  DccEightZeros();        // 8 x 0 bit        // Error detection
  DccOnePulse();          // end pulse
}
