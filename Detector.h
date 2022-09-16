//

HardwareSerial Poort2(2);     // Define a ESP32 Serial port instance called 'Poort2' using serial port 2

int railComRX = 16;           // GPIO16 Connected to RailCom Detector RX
int railComInt = 15;;         // GPIO15 input pin  int Raicom Detector

int receiveCnt = 0;
int displCnt = 0;
bool arrayComplete = false;

int inByte = 0;

const int rxArrayMax = 11;
int rxArray[rxArrayMax];      
int rxArrayCnt = 0;

bool ok_4_8Code = true;
bool no_4_8Code = false;

bool test_4_8Code = true;
bool test_4_8Decimal = true;

int convByte;

//==================================== Conversion Arrays =============================

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
    if (decodeArray[i] == 31)             // End of table reached
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
    if (decodeArray[i] == 31)             // End of table reached
    {
      test_4_8Decimal = false;
      goto out;
    }
    i++;
  }
  inByte = convertArray[i];
out:;
}

//====================================== Print Array ============================

void printRxArray()
{
  int i = 0;
  while (i <= rxArrayCnt)
  {
    Serial.print(rxArray[i]);
    Serial.print(" ");
    i ++;
  }
  Serial.println("");
}

//====================================== Integer to Binair ============================

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
  while (i <= rxArrayCnt)
  {
    convByte = (rxArray[i]);
    setIntToBinString();
    i ++;
  }
  Serial.println("");  
}

void ClearRxArray()
{
  int i = 0;
  while (i < rxArrayMax - 1)
  {
    rxArray[i] = 0;
    i ++;
  }
}

//===================================== ISR GPIO15 ==============================

void IRAM_ATTR GPIO15ToHigh()
{
  rxArrayCnt = 0;

  while (Poort2.available())
  {
    for (int i  = 0; i < 8; i++)                  // Read 8 bytes
    {
      inByte = Poort2.read();                     // Read byte

      if (inByte == 0 || inByte == 255 || inByte < 0)
      {
        goto next;
      }

      check_4_8Code();

      if (test_4_8Code)                             // Check if 4-8 code is ok, if ok , print
      {
        //Serial.print(inByte);                     // Print 4-8 code
        //Serial.print(" ");
      }

      convert4_8ToDec();

      if (test_4_8Decimal)                          // If ok_4_8Code is valid byte into  array
      {
        rxArray[rxArrayCnt] = inByte;               // Byte into  array
        rxArrayCnt ++;                              // Increment char counter
      }
next :;
    }
  }
  if (rxArrayCnt > 1)                               // Print only only more than 1 char's received
  {
    printRxArray();
    printRxArrayToBin();
  }
  ClearRxArray();
}
