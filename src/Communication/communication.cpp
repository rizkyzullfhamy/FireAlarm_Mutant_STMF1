/**
 * Author Rizky Zull Fhamy PC
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "Arduino.h"
#include "header.h"

/* _____GLOBAL VARIABLES_____________________________________________________ */
ModbusMaster node;
char buffer[200];
String dataIn;
String dt[20];
String dataF4[30];
String dataSendModbus[2];

int dateTimee[6];

HardwareSerial MySerial1(PA10, PA9); // RX,TX
HardwareSerial MyRS485(PB11, PB10);
HardwareSerial MySerial2(PA3, PA2);

/* _____DECLARATION FUNCTION_____________________________________________________ */
void preTransamissio();
void postTransamission();
void digitalClockDisplay();
void printDigits(int digits);
void sendToF4();
void printDataSegment();
void sendToRS485Scada();
void parsingDataF4();
void parsingSetDataTimeByF4();

void preTransamissio()
{
    digitalWrite(rs485DE_RE, LOW);
    digitalWrite(rs485DE_RE, HIGH);
}

void postTransamission()
{
    digitalWrite(rs485DE_RE, LOW);
    digitalWrite(rs485DE_RE, HIGH);
}

void digitalClockDisplay()
{
    MySerial1.print(day());
    MySerial1.print("-");
    MySerial1.print(month());
    MySerial1.print("-");
    MySerial1.print(year());
    MySerial1.print(", ");
    MySerial1.print(hour());
    printDigits(minute());
    MySerial1.print(" WIB");
    MySerial1.print("|001|;");
    MySerial1.println();
}

void printDigits(int digits)
{
    MySerial1.print(":");
    if (digits < 10)
        MySerial1.print('0');
    MySerial1.print(digits);
}

void sendToF4()
{
    int tmpDay, tmpMonth, tmpYear, tmpHour, tmpMinute;
    tmpDay = day();
    tmpMonth = month();
    tmpYear = year();
    tmpHour = hour();
    tmpMinute = minute();
    // sprintf(buffer, "%d-%d-%d, %d:%d WIB|%d|\r\n", tmpDay, tmpMonth, tmpYear, tmpHour, tmpMinute, dataSupply);   // date format if use datetime full
    sprintf(buffer, "%d-%d-%d|%d|\r\n", tmpDay, tmpMonth, tmpYear, dataSupply);
    MySerial1.print(buffer);
}

void printDataSegment()
{
    MySerial2.print("DATA MODBUS 1 : ");
    MySerial2.println(dataSendModbus[0]);
    MySerial2.print("DATA MODBUS 2 : ");
    MySerial2.println(dataSendModbus[1]);
}

void sendToRS485Scada()
{
    // CONVERT DATA DARI F4 SEBELUM DIKIRIM KE SCADA
    for (int i = 0; i < 16; i++)
    {
        if (dataF4[i] == "0")
        {
            dataF4[i] = "1";
        }
        else if (dataF4[i] == "1")
        {
            dataF4[i] = "2";
        }
        else if (dataF4[i] == "2")
        {
            dataF4[i] = "3";
        }
    }
    // CONVERT DATA MENJADI 2 BAGIAN ARRAY
    for (int i = 0; i < 16; i++)
    {
        if (i < 9)
        {
            dataSendModbus[0] += dataF4[i];
        }
        else
        {
            dataSendModbus[1] += dataF4[i];
        }
    }

    dataSendModbus[1] += String(kondisiSupply);

    // KIRIM DATA MODBUS MENJADI 2 SAJA
    node.writeSingleRegister(((uint16_t)0x40000), dataSendModbus[0].toInt());
    node.writeSingleRegister(((uint16_t)0x40001), dataSendModbus[1].toInt());

    //   printDataSegment();           // AKTIFKAN UNTUK DEBUG SAJA

    // reset data
    dataSendModbus[0] = "";
    dataSendModbus[1] = "";
}

void parsingDataF4()
{
    int count = 0;

    dataF4[count] = "";

    for (int i = 0; i < dataIn.length(); i++)
    {
        if (dataIn[i] == ',')
        {
            count++;
            dataF4[count] = "";
        }
        else
        {
            dataF4[count] = dataF4[count] + dataIn[i];
        }
    }
}

void parsingSetDataTimeByF4()
{
    int count = 0;
    for (int i = 0; i < dataIn.length(); i++)
    {
        if (dataIn[i] == ',')
        {
            dateTimee[count] = dt[count].toInt();
            dt[count] = "";
            count++;
        }
        else
        {
            dt[count] = dt[count] + dataIn[i];
        }
    }
}
