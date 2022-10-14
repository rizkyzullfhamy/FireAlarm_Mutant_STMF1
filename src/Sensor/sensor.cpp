/**
 * Author Rizky Zull Fhamy PC
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "Arduino.h"
#include "header.h"

/* _____GLOBAL VARIABLES_____________________________________________________ */
float voltSmps, voltBatrai;
int dataSupply;
uint8_t kondisiSupply;

/* _____DECLARATION FUNCTION_____________________________________________________ */
float adcAccess(uint32_t pinAdc);
void batrai_smps();

float adcAccess(uint32_t pinAdc)
{
    float analogVal = analogRead(pinAdc);
    float voltage = (0.0336 * analogVal) + 0.2337;
    return voltage;
}
void batrai_smps()
{
    /*
      SMPS TRIGGER RELAY
      DIBAWAH 20 VOLT => RELAY ON  => SUPPLY DAYA BATRAI => KODENYA 4
      DIATAS 20 VOLT => RELAY OFF  => SUPPY DAYA PLN   => KODENYA 3

      BATRAI
      DIBAWAH 22 VOLT = > FAULT    => KODE 5
      DIATAS 22 VOLT => AMAN

      18 - 22 BATRAI LOW
      0- 18 BATRAI RUSAK
    */
    if ((voltSmps > 20) && (voltBatrai >= 23 && voltBatrai <= 27))
    {
        digitalWrite(relayPin, LOW); // CHARGER BATRAI NONAKTIF
        kondisiSupply = 4;           // SUPPY DAYA PLN & KONDISI BATRAI AMAN
        dataSupply = 100;            // AMAN
    }
    else if (voltSmps < 20 && voltBatrai >= 23)
    {
        digitalWrite(relayPin, LOW); // CHARGER BATRAI NONAKTIF
        kondisiSupply = 5;           // SUPPLY DAYA BATRAI & KONDISI BATRAI AMAN
        dataSupply = 100;
    }
    else if ((voltSmps > 20) && (voltBatrai >= 18 && voltBatrai < 23))
    {
        digitalWrite(relayPin, HIGH); // CHARGER BATRAI AKTIF
        kondisiSupply = 6;            // SUPPLY PLN & BATRAI LOW (SEDANG CAS)
        dataSupply = 100;             // AMAN
    }
    else if ((voltSmps < 20) && (voltBatrai >= 18 && voltBatrai < 23))
    {
        digitalWrite(relayPin, LOW); // CHARGER BATRAI NONAKTIF
        dataSupply = 100;            // AMAN
        kondisiSupply = 7;           // SUPPLY BATRAI & BATRAI LOW
    }
    else if (voltBatrai < 18 && voltSmps > 20)
    {
        digitalWrite(relayPin, LOW); // CHARGER BATRAI NONAKTIF
        dataSupply = 102;            // LOS BATRAI       // awal data 101
        kondisiSupply = 8;           // SUPPLY PLN & BATRAI RUSAK
    }
    else if (voltBatrai < 18 && voltSmps < 20)
    {
        digitalWrite(relayPin, LOW); // CHARGER BATRAI NONAKTIF
        dataSupply = 101;            // LOS PLN        // awal data 102
        kondisiSupply = 9;           // SUPPLY BATRAI & BATRAI RUSAK
    }
    else
    {
        dataSupply = 100; // KONDISI AMAN
    }
}