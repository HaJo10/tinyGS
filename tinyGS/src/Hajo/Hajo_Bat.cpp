/*
    Messung Batteriespannung
*/

#include "Hajo.h"
#include "Hajo_Bat.h"
#include "../Logger/Logger.h"
#include "driver/gpio.h"
#include "driver/adc.h"

const       byte vbatPin      = 35;
const       byte ADC_BITS     = 10; // 10-12 möglich
byte        mode              = 'u';



void HajoSat::vBat(){

        if ( boardNr == 14 ) {      // 14 = T-Beam
            readAXP192();
        } else {
            readAnaloginput();
        };
    
}

void HajoSat::readAXP192() {

}

void HajoSat::readAnaloginput() {
    float   adc_reading = 0;
    float   vBat;

    Serial.println("Init ADC");

    pinMode(vbatPin, INPUT);
    analogReadResolution(ADC_BITS);

    adc_reading = (float)(analogRead( vbatPin ));

    //vBat = adc_reading / ( 1024 * 2 * 3.3 * 1.1 );

    vBat = ( adc_reading / (( 1<< ADC_BITS ) - 1 )) * 3.3;

    //status.neu.vBat = vBat; 

    uint32_t raw = analogRead(vbatPin);
    float scaled = 1000.0 * 2 * (3.3 / 1024.0) * raw;

    Serial.printf (PSTR ("adc %04u vBat %04f scaled %04f \n"), adc_reading, vBat, scaled);
    Log::console(PSTR ("adc %04u vBat %04f scaled %04f %04f \n"), adc_reading, vBat, scaled, raw);

    //status.neu.vBat = 3.01;
}