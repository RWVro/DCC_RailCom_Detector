
//================================= CV Read & Interupt Pin  =================================

HardwareSerial Poort2(2);       // Define a Serial port instance called 'Poort2' using serial port 2

int railComRX = 16;             // GPIO16 Connected to RailCom Detector RX

bool startIntDown = false;
bool startIntUp = false;

int receiveCnt = 0;
bool arrayComplete = false;

int inByte = 0;

const int rxArrayMax = 11;
int rxArray[rxArrayMax];    // [10] = char's
int rxArrayCnt = 0;

int addrValue;

bool ok_4_8Code = true;
bool no_4_8Code = false;

bool test_4_8Code = true;
bool test_4_8Decimal = true;

int detectorSaveAddr = 0;

String outputDetectorAddressString;
String SaveInfoString;

int convByte;

bool addresFoundOk = false;       // Used in AddressRead.h

bool basicPacketFlg = false;
bool advancePacketFlg = false;

int interuptCnt = 0;              // Used for testing Loc on track or not

//============================== 4-8 Table conversion ======================

int decodeArray[68] = {172, 170, 169, 165, 163, 166, 156, 154, 153, 149, 147, 150, 142, 141, 139, 177, 178, 180, 184, 116,
                       114, 108, 106, 105, 101, 99, 102, 92, 90, 89, 85, 83, 86, 78, 77, 75, 71, 113, 232, 228, 226, 209, 201,
                       197, 216, 212, 210, 202, 198, 204, 120, 23, 27, 29, 30, 46, 54, 58, 39, 43, 45, 53, 57, 51, 15, 240, 225, 31
                      };       // 31 is end of table (0001 1111)


int convertArray[67] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
                        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66
                       };

//====================================== Convert 4_8 Table to dec. ====================

void check_4_8Code()
{
  test_4_8Code = true;

  int compareValue = inByte;
  int i = 0;
  while (compareValue != decodeArray[i])
  {
    if (decodeArray[i] == 31)                  // End of table reached
    {
      test_4_8Code = false;
      goto out;
    }
    i++;
  }
out:;
}

void convert4_8ToDec()
{
  test_4_8Decimal = true;

  int compareValue = inByte;
  int i = 0;
  while (compareValue != decodeArray[i])
  {
    if (decodeArray[i] == 31)                  // End of table reached
    {
      test_4_8Decimal = false;
      goto out;
    }
    i++;
  }
  inByte = convertArray[i];
out:;
}

//===================================== Print array ===================================

void printRxArray()
{
  int i = 0;
  while (i <= rxArrayCnt - 1)
  {
    Serial.print(rxArray[i]);
    Serial.print(" ");
    i ++;
  }
  Serial.println("");
}

void setIntToBinString()
{
  for (int k = 7; k >= 0; k--)
  {
    Serial.print(bitRead(convByte, k));
  }
  Serial.print(" ");
}

void printRxArrayToBin()
{
  int i = 0;
  while (i <= rxArrayCnt - 1)
  {
    convByte = (rxArray[i]);
    setIntToBinString();
    i ++;
  }
  Serial.println("");
}

//======================================= Clear array ===================================

void ClearRxArray()
{
  int i = 0;
  while (i < rxArrayMax - 1)
  {
    rxArray[i] = 0;
    i ++;
  }
}

//==================================== Monitor Address  ===================================

void monitorAll()
{
  rxArrayCnt = 0;

  while (Poort2.available())
  {
    for (int i  = 0; i < 8; i++)                          // Read 8 bytes
    {
      inByte = Poort2.read();                             // Read byte

      if (inByte == 0 || inByte == 255 || inByte < 0)
      {
        goto next;
      }

      check_4_8Code();
      convert4_8ToDec();

      if (test_4_8Decimal)                                     // If ok_4_8Code is valid byte into  array
      {
        rxArray[rxArrayCnt] = inByte;                       // Byte into  array
        rxArrayCnt ++;                                      // Increment char counter
      }
next :;
    }
  }
  arrayComplete = true;
  startIntUp = true;

  if (basicPacketFlg)
  {
    if (rxArrayCnt < 2)                                   // Print only arrays with 2 or more bytes
    {
      return;
    }
  }
  if (advancePacketFlg)
  {
    if (rxArrayCnt < 4)                                   // Print only arrays with 4 or more bytes
    {
      return;
    }
  }
}

//===================================== Find Address ===================================

void FindAddressInData()
{
  if (startIntUp)
  {
    if (arrayComplete)                                    // Print array's
    {
      addresFoundOk = false;

      if (rxArray[0] == 8)                                // Test is array starts with 8 (address line)
      {
        addrValue = rxArray[1];                           // Save the loc address
        if (addrValue != detectorSaveAddr)                // Print 1 time address and when it changed
        {
          detectorSaveAddr = addrValue;
          addresFoundOk = true;                           // Set true for detect in AddressRead.h
        }
      }

      if (addrValue > 2)                                  // If Address < 3 don't print
      {
        outputDetectorAddressString = "Loc " + String ("Address = ") +  String (addrValue);
        interuptCnt = 0;                                                                    // Reset for Loc on Track (ISR)
        SaveInfoString = outputDetectorAddressString;
      }
    }
  }
  arrayComplete = false;
  startIntUp = false;
  startIntDown = true;
  addrValue = 0;
  ClearRxArray();
}

//===================================== ISR GPIO15 ================

void IRAM_ATTR GPIO15ToLow()
{
    interuptCnt++;
    if (interuptCnt >= 400)
    {
      SaveInfoString = "No Loc on track!";
      interuptCnt = 0;
    }
    monitorAll();
  return;
}
