/**
 * Author Rizky Zull Fhamy PC
 * PROJECT FIRE-ALARM
 * BUILD BY STM32F1
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include <Arduino.h>
#include <stdbool.h>
#include <header.h>

/* _____GLOBAL VARIABLES_____________________________________________________ */
int i = 0;
bool parsing = false;
bool flagSetTime = false;

unsigned long startMillis1;
unsigned long startMillis2;
unsigned long startMillis3;
unsigned long currentMillis;

void setup()
{
  // put your setup code here, to run once:
  pinMode(rs485DE_RE, OUTPUT);

  // SET RS485 TO RECEIVE =>  made LOW to receive data and made HIGH to write data to RS-485 bus
  digitalWrite(rs485DE_RE, HIGH);

  MyRS485.begin(19200);
  node.begin(1, MyRS485);

  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(relayPin, LOW);
  MySerial1.begin(115200);
  MySerial2.begin(115200);

  while (1)
  {
    if (MySerial1.available() > 0)
    {
      delay(10);
      char inChar = (char)MySerial1.read();
      dataIn += inChar;
      if (inChar == '\n')
      {
        parsing = true;
      }
    }

    if (parsing)
    {
      parsingSetDataTimeByF4();
      // FORMAT PARSING WAKTU KE DARI F4 KE F1 =>  JAM,MENIT,DETIK,HARI,BULAN,TAHUN, (CONTOH: 21,5,10,8,1,2022,)  //1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
      setTime(dateTimee[0], dateTimee[1], dateTimee[2], dateTimee[3], dateTimee[4], dateTimee[5]);
      parsing = false;
      dataIn = "";
      for (int i = 0; i < 20; i++)
      {
        dt[i] = "";
      }
      flagSetTime = true;
      digitalWrite(ledPin, LOW);
      sendToF4(); // SEND DATETIME TO F4
      break;
    }
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  if (MySerial1.available() > 0)
  { // KOMUNIKASI F1 <=> F4
    char inChar = (char)MySerial1.read();
    dataIn += inChar;
    if (inChar == '\n')
    {
      parsing = true;
    }
  }

  if (parsing)
  {
    parsingDataF4();
    parsing = false;
    dataIn = "";
  }

  if (currentMillis - startMillis2 >= 15000)
  {
    voltSmps = adcAccess(smpsPin);  // SMPS PIN PA0
    voltBatrai = adcAccess(batPin); // BATRAI PIN PA1
    batrai_smps();
    digitalWrite(ledPin, LOW);
    startMillis2 = currentMillis;
  }

  if (currentMillis - startMillis3 >= 60000)
  {
    digitalWrite(ledPin, HIGH);
    sendToF4(); // SEND DATETIME TO F4
    sendToRS485Scada();
    startMillis3 = currentMillis;
  }
}