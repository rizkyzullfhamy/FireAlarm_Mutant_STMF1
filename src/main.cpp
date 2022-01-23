#include <Arduino.h>
#include <time.h>
#include <TimeLib.h>
#include <stdbool.h>
#include <ModbusMaster.h> 

#define ledPin PC13 //13
#define smpsPin PA0 //ADC PIN 
#define batPin  PA1 //ADC PIN 
#define relayPin PA7 // RELAY PIN  
#define rs485DE PB6
#define rs485RE PB7    

void printDigits(int digits);
void digitalClockDisplay();
void parsingData();
void parsingDataF4();
void parsingSetDataTimeByF4();
void batrai_smps();
void toScada();
float adcAccess(uint32_t pinAdc);

char buffer[200];
String dataIn;
String dt[20];
char tmp[100];
String dataF4[20];
int notifSens[20];
int dateTimee[6];
int i = 0;
bool parsing=false;
bool flagSetTime = false;
float voltSmps, voltBatrai;
uint8_t kondisiSupply;         
HardwareSerial MySerial1(PA10, PA9);    // RX,TX
HardwareSerial MySerial2(PA3, PA2);
ModbusMaster node;   
uint16_t counting = 0;

void preTransmission() //Fungsi untuk menyetel stste Pin DE & RE RS-485
{
  digitalWrite(rs485RE, LOW);             
  digitalWrite(rs485DE, HIGH);
}

void postTransmission()
{
  digitalWrite(rs485RE, LOW);
  digitalWrite(rs485DE, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(rs485DE, OUTPUT);
  pinMode(rs485RE, OUTPUT);

  // SET RS485 TO RECEIVE =>  made LOW to receive data and made HIGH to write data to RS-485 bus
  digitalWrite(rs485RE, LOW);    // ACTIVE LOW (ACTIVE Receiver)
  digitalWrite(rs485DE, HIGH);     // ACTIVE HIGH (ACTIVE Transciever)

  MySerial2.begin(115200);

  //SERIAL MODBUSS INIT
  MySerial1.begin(19200);
  node.begin(1, MySerial1);
  // node.preTransmission(preTransmission); 
  // node.postTransmission(postTransmission);
  //digitalWrite(rs485DE, HIGH);
  //digitalWrite(rs485RE, LOW);
  
  pinMode(ledPin,OUTPUT);
  pinMode(relayPin,OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(relayPin, LOW);
  MySerial2.begin(115200);
  while(1){
    if(MySerial2.available() > 0){
      delay(10);
      char inChar = (char) MySerial2.read();
      dataIn += inChar;
        if(inChar == '\n'){
        parsing = true;
      }
    }

    if(parsing){
    parsingSetDataTimeByF4();
    // FORMAT PARSING WAKTU KE DARI F4 KE F1 =>  JAM,MENIT,DETIK,HARI,BULAN,TAHUN, (CONTOH: 21,5,10,8,1,2022,)
    setTime(dateTimee[0], dateTimee[1], dateTimee[2], dateTimee[3], dateTimee[4], dateTimee[5]);
    parsing=false;
    dataIn="";
    for(int i = 0; i < 20; i++){
      dt[i] = "";
    }
    flagSetTime = true;
    digitalWrite(ledPin, LOW);
    break;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  /*counting += 9;
  i += 10;
      node.writeSingleRegister(((uint16_t)0x40000),counting);        //Writes value to 0x40000 holding register
      node.writeSingleRegister(((uint16_t)0x40001),i);        //Writes value to 0x40000 holding register
  delay(1000);
  */

  if(flagSetTime == true){
  if(MySerial2.available() > 0){
    delay(10);
    char inChar = (char) MySerial2.read();
    dataIn += inChar;
    if(inChar == '\n'){
      parsing = true;
    }
  }
  if(parsing){
    // parsingData();
    parsingDataF4();
    parsing=false;
    dataIn="";
  }
  voltSmps    =  adcAccess(smpsPin);      // SMPS PIN PA0
  voltBatrai  =  adcAccess(batPin);       // BATRAI PIN PA1
  batrai_smps();
  MySerial2.print("data 1 : ");
  MySerial2.print(dataF4[0]);
  notifSens[0] = dataF4[0].toInt();
  MySerial2.print(" data 2 : ");
  MySerial2.print(dataF4[1]);
  notifSens[1] = dataF4[1].toInt();
  MySerial2.print(" data 3 : ");
  MySerial2.print(dataF4[2]);
  notifSens[2] = dataF4[2].toInt();
  MySerial2.print(" data 4 : ");
  MySerial2.print(dataF4[3]);
  notifSens[3] = dataF4[3].toInt();
  MySerial2.print(" data 5 : ");
  MySerial2.print(dataF4[4]);
  notifSens[4] = dataF4[4].toInt();
  MySerial2.print(" data 6 : ");
  MySerial2.print(dataF4[5]);
  notifSens[5] = dataF4[5].toInt();
  MySerial2.print(" data 7 : ");
  MySerial2.print(dataF4[6]);
  notifSens[6] = dataF4[6].toInt();
  MySerial2.print(" data 8 : ");
  MySerial2.print(dataF4[7]);
  notifSens[7] = dataF4[7].toInt();
  MySerial2.print(" data 9 : ");
  MySerial2.print(dataF4[8]);
  notifSens[8] = dataF4[8].toInt();
  MySerial2.print(" data 10 : ");
  MySerial2.print(dataF4[9]);
  notifSens[9] = dataF4[9].toInt();
  MySerial2.print(" data 11 : ");
  MySerial2.print(dataF4[10]);
  notifSens[10] = dataF4[10].toInt();
  MySerial2.print(" data 12 : ");
  MySerial2.print(dataF4[11]);
  notifSens[11] = dataF4[11].toInt();
  MySerial2.print(" data 13 : ");
  MySerial2.print(dataF4[12]);
  notifSens[12] = dataF4[12].toInt();
  MySerial2.print(" data 14 : ");
  MySerial2.print(dataF4[13]);
  notifSens[13] = dataF4[13].toInt();
  MySerial2.print(" data 15 : ");
  MySerial2.print(dataF4[14]);
  notifSens[14] = dataF4[14].toInt();
  MySerial2.print(" data 16 : ");
  MySerial2.print(dataF4[15]);
  notifSens[15] = dataF4[15].toInt();
  
  /*TEST DATA*/
  MySerial2.print(" V SMPS : ");
  MySerial2.print(voltSmps);
  MySerial2.print(" V BATR : ");
  MySerial2.print(voltBatrai);
  MySerial2.print(" Kondisi Supply: ");
  MySerial2.print(kondisiSupply);
  MySerial2.print("\r\n");
  digitalClockDisplay();
  delay(1000);
  }

}

void parsingDataF4(){
    int count = 0;
    MySerial2.print("data masuk : ");
    MySerial2.print(dataIn);
    for(int i = 0; i < dataIn.length(); i++){
      if(dataIn[i] == ','){
        dataF4[count] = dt[count];
         dt[count] = "";
        count++;
      }else{
         dt[count] = dt[count]  + dataIn[i];
      }
    }
}

void parsingSetDataTimeByF4(){
    int count = 0;
    MySerial2.print("data masuk : ");
    MySerial2.print(dataIn);
    for(int i = 0; i < dataIn.length(); i++){
      if(dataIn[i] == ','){
        dateTimee[count] = dt[count].toInt();
         dt[count] = "";
        count++;
      }else{
         dt[count] = dt[count]  + dataIn[i];
      }
    }
    sprintf(buffer, "Hour : %d | Minute : %d | Sec : %d | day : %d | Month : %d | Year : %d", dateTimee[0], dateTimee[1], dateTimee[2], dateTimee[3], dateTimee[4], dateTimee[5]);
    MySerial2.println(buffer);
}

void digitalClockDisplay()
{
  MySerial2.print(day());
  MySerial2.print("-");
  MySerial2.print(month());
  MySerial2.print("-");
  MySerial2.print(year());
  MySerial2.print(",");
  MySerial2.print(hour());
  printDigits(minute());
  printDigits(second());
  MySerial2.print(" WIB");
  MySerial2.print("|001|");
  MySerial2.println();  
}

void printDigits(int digits)
{
  MySerial2.print(":");
  if(digits < 10)
  MySerial2.print('0');
  MySerial2.print(digits);  
}

float adcAccess(uint32_t pinAdc ){
  float analogVal = analogRead(pinAdc);
  float voltage = (0.0336*analogVal)+0.2337;
  return voltage;
}

void batrai_smps(){
  if(voltSmps > 20 && voltBatrai >= 22){
    // RELAY AKTIF
    digitalWrite(relayPin, LOW);
    kondisiSupply = 3;      // SUPPY DAYA PLN & KONDISI BATRAI AMAN
  }else if(voltSmps < 20 && voltBatrai >= 22){
    // RELAY NONAKTIF
    digitalWrite(relayPin, LOW);
    kondisiSupply = 4;      // SUPPLY DAYA BATRAI & KONDISI BATRAI AMAN
  }else if((voltBatrai >= 18 && voltBatrai < 22) && (voltSmps > 20)){
    // relay aktif
    digitalWrite(relayPin, HIGH);
    kondisiSupply = 5;   // SUPPLY PLN & BATRAI LOW
  }else if((voltBatrai >= 18 && voltBatrai < 22) && (voltSmps < 20)){
    // RELAY NONAKTIF
    digitalWrite(relayPin, LOW);
    kondisiSupply = 6;   // SUPPLY BATRAI & BATRAI LOW
  }else if(voltBatrai < 18 && voltSmps > 20){
      // RELAY NONAKTIF
      digitalWrite(relayPin, LOW);
     kondisiSupply = 7;   // SUPPLY PLN & BATRAI RUSAK
  }else if(voltBatrai < 18 && voltSmps < 20){
      // RELAY NONAKTIF
      digitalWrite(relayPin, LOW);
     kondisiSupply = 8;   // SUPPLY BATRAI & BATRAI RUSAK
  }

// SMPS TRIGGER RELAY
// DIBAWAH 20 VOLT => RELAY ON  => SUPPLY DAYA BATRAI => KODENYA 4
// DIATAS 20 VOLT => RELAY OFF  => SUPPY DAYA PLN   => KODENYA 3 

// BATRAI
// DIBAWAH 22 VOLT = > FAULT    => KODE 5 
// DIATAS 22 VOLT => AMAN

// 18 - 22 BATRAI LOW
// 0- 18 BATRAI RUSAK 
}

void toScada(){
  //Address dan data yang dikirimkan
  node.writeSingleRegister(0x1000, notifSens[0]);
  node.writeSingleRegister(0x1001, notifSens[1]);
  node.writeSingleRegister(0x1002, notifSens[2]);
  node.writeSingleRegister(0x1003, notifSens[3]);
  node.writeSingleRegister(0x1004, notifSens[4]);
  node.writeSingleRegister(0x1005, notifSens[5]);
  node.writeSingleRegister(0x1006, notifSens[6]);
  node.writeSingleRegister(0x1007, notifSens[7]);
  node.writeSingleRegister(0x1008, notifSens[8]);
  node.writeSingleRegister(0x1009, notifSens[9]);
  node.writeSingleRegister(0x1010, notifSens[10]);
  node.writeSingleRegister(0x1011, notifSens[11]);
  node.writeSingleRegister(0x1012, notifSens[12]);
  node.writeSingleRegister(0x1013, notifSens[13]);
  node.writeSingleRegister(0x1014, notifSens[14]);
  node.writeSingleRegister(0x1015, notifSens[15]);
  node.writeSingleRegister(0x1016, kondisiSupply);
}