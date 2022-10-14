/**
 * Author Rizky Zull Fhamy PC
 */

/* VARIABEL GLOBAL */

#include "Arduino.h"
#include <time.h>
#include <TimeLib.h>
#include <ModbusMaster.h>

#define ledPin PC13  // 13
#define smpsPin PA1  // ADC PIN
#define batPin PA0   // ADC PIN
#define relayPin PB0 // RELAY PIN
#define rs485DE_RE PB1

extern HardwareSerial MySerial1;
extern HardwareSerial MyRS485;
extern HardwareSerial MySerial2;

extern ModbusMaster node;
extern char buffer[200];
extern String dataIn;
extern String dt[20];
extern String dataF4[30];
extern String dataSendModbus[2];

extern int dataSupply;
extern int dateTimee[6];
extern float voltSmps, voltBatrai;
extern uint8_t kondisiSupply;

/* _____GLOBAL FUNCTION_____________________________________________________ */

/*--COMMUNICATION--*/
extern void preTransamissio();
extern void postTransamission();
extern void digitalClockDisplay();
extern void printDigits(int digits);
extern void sendToF4();
extern void printDataSegment();
extern void sendToRS485Scada();
extern void parsingDataF4();
extern void parsingSetDataTimeByF4();

/*--SENSOR--*/
extern float adcAccess(uint32_t pinAdc);
extern void batrai_smps();