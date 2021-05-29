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

uint32_t    last_read_time_ms = 0;

void HajoSat::vBat(){
    uint32_t    adc_reading = 0;
    float       vBat;
    if (last_read_time_ms == 0) {
        Serial.println("Init ADC");

        pinMode(vbatPin, INPUT);
        analogReadResolution(ADC_BITS);
    };

    const uint32_t min_read_interval = 5000;
    if (millis() - last_read_time_ms > min_read_interval) {
        last_read_time_ms = millis();

        adc_reading = (float)(analogRead( vbatPin ));
        //adc_reading = (float)(analogRead( vbatPin ));

        //vBat = adc_reading / ( 1024 * 2 * 3.3 * 1.1 );

        vBat = ( adc_reading / (( 1<< ADC_BITS ) - 1 )) * 3.3;

        //status.neu.vBat = vBat; 

        uint32_t raw = analogRead(vbatPin);
        float scaled = 1000.0 * 2 * (3.3 / 1024.0) * raw;

        Serial.printf (PSTR ("adc %04u vBat %04f scaled %04f \n"), adc_reading, vBat, scaled);
        Log::console(PSTR ("adc %04u vBat %04f scaled %04f %04f \n"), adc_reading, vBat, scaled, raw);
    };

    //status.neu.vBat = 3.01;
}