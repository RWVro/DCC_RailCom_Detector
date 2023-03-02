
//============================================= Send Read Packets ====================


void AddReadPacket()
{
  SendDccPreamble();
  DccZeroPulse();               // start bit
  SendDccAddress();
  DccZeroPulse();
  SendDccInstruction();
  DccZeroPulse();               // start bit
  SendDccCVNumber();
  DccZeroPulse();               // start bit
  SendDccCVValue();
  DccZeroPulse();
  SendDccErrorDetection();
  DccOnePulse();                // end pulse
  DccCutOutPulse();
}

//=================================================  Read Loc Address =================================================

void ReadBasisLocAddress()      // AddReadSendByte = 0 InstrReadByte = 228 CVNmbReadByte = 0 CVValueByte = 0 AddReadErrorByte = 228
{
  basicPacketFlg = true;
  advancePacketFlg = false;
  AddReadPacket();                              // Read Loc Address firtst with address = 0
}

void ReadToDetectLocAddress()                   // Read Loc Address to detect address
{
  advancePacketFlg = true;
  basicPacketFlg = false;
  AddReadPacket();                              // Read Loc Address firtst with address = 0

  if (!addresFoundOk)                           // If the loc does not respond to address 0 , find the address
  {
    locAddressByte++;
    CalculateEOR();
  }  
  
  if (addresFoundOk)                            // Do until loc address is found
  {
    Serial.println("Add Found");
    locAddressByte =  detectorSaveAddr;        // Read Loc address with detected address (detector.h)
    CalculateEOR();
  }
}
