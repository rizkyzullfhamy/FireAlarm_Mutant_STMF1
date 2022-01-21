#include <Arduino.h>
#include <time.h>
#include <TimeLib.h>
#include <stdbool.h>
#include <ModbusMaster.h> 

#define ledPin PC13 //13
#define smpsPin PA0 //ADC PIN 
#define batPin  PA1 //ADC PIN 
#define relayPin PA7 // RELAY PIN     

void printDigits(int digits);
void digitalClockDisplay();
void parsingData();
void parsingDataF4();
void parsingSetDataTimeByF4();
void batrai_smps();
float adcAccess(uint32_t pinAdc);

char buffer[200];
String dataIn;
String dt[20];
char tmp[100];
String dataF4[20];
int dateTimee[6];
int i;
bool parsing=false;
bool flagSetTime = false;
float voltSmps, voltBatrai;
uint8_t kondisiSupply;         
HardwareSerial MySerial1(PA10, PA9);    // RX,TX

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin,OUTPUT);
  pinMode(relayPin,OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(relayPin, LOW);
  MySerial1.begin(115200);
  while(1){
    if(MySerial1.available() > 0){
      delay(10);
      char inChar = (char) MySerial1.read();
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
  if(flagSetTime == true){
  if(MySerial1.available() > 0){
    delay(10);
    char inChar = (char) MySerial1.read();
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
  MySerial1.print("data 1 : ");
  MySerial1.print(dataF4[0]);
  MySerial1.print(" data 2 : ");
  MySerial1.print(dataF4[1]);
  MySerial1.print(" data 3 : ");
  MySerial1.print(dataF4[2]);
  MySerial1.print(" data 4 : ");
  MySerial1.print(dataF4[3]);
  MySerial1.print(" V SMPS : ");
  MySerial1.print(voltSmps);
  MySerial1.print(" V BATR : ");
  MySerial1.print(voltBatrai);
  MySerial1.print(" Kondisi Supply: ");
  MySerial1.print(kondisiSupply);
  MySerial1.print("\r\n");
  digitalClockDisplay();
  delay(1000);
  }

}

void parsingDataF4(){
    int count = 0;
    MySerial1.print("data masuk : ");
    MySerial1.print(dataIn);
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
    MySerial1.print("data masuk : ");
    MySerial1.print(dataIn);
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
    MySerial1.println(buffer);
}

void digitalClockDisplay()
{
  MySerial1.print(day());
  MySerial1.print("-");
  MySerial1.print(month());
  MySerial1.print("-");
  MySerial1.print(year());
  MySerial1.print(",");
  MySerial1.print(hour());
  printDigits(minute());
  printDigits(second());
  MySerial1.print(" WIB");
  MySerial1.print("|001|");
  MySerial1.println();  
}

void printDigits(int digits)
{
  MySerial1.print(":");
  if(digits < 10)
  MySerial1.print('0');
  MySerial1.print(digits);  
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




