/*
    Messung Batteriespannung
*/

#include "Hajo.h"
#include "Hajo_Bat.h"
#include "../Logger/Logger.h"
#include "driver/gpio.h"
#include "driver/adc.h"

const       byte vbatPin      = 35;
const       byte ADC_BITS     = 12; // 10-12 möglich
byte        mode              = 'u';
bool        isInitialized     = false;


void HajoSat::vBat(int boardNr){

    Log::console(PSTR ("vBat boardNr: %u"), boardNr );

    if ( boardNr == 14 ) {      // 14 = T-Beam
        readAXP192();
    } else {
        readAnaloginput();
    };
    
}

void HajoSat::readAXP192() {

}

void initADC () {
    pinMode(vbatPin, INPUT);
    analogReadResolution(ADC_BITS);
    analogSetPinAttenuation(vbatPin, ADC_11db);
}

float PolyVoltage(uint32_t reading) {
  //return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  //return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
    //  mV   ADC
    // 1250  1362
    // 1500  1676
    // 1750  1982
    // 2000  2284
    // 2250  2594
    
    double fAdcM = (float)reading / 1000.;
    double Vpoly = (-0.033 * pow(fAdcM, 4.)) 
                    + (0.222 * pow(fAdcM, 3.)) 
                    - (0.527 * pow(fAdcM, 2.)) 
                    + (1.321 * fAdcM) 
                    - 0.017;

    //Log::console(PSTR ("reading: %i fAdcM: %.3f Vpoly: %.2f vPoly: %.2f"), reading, fAdcM, Vpoly );
    return Vpoly;
} // Added an improved polynomial, use either, comment out as required

int readADCaverage() {
    
    int         anzMessungen = 21, tries = 25;
    uint32_t    sum = 0;
    int         min = 9999, max = 0, raw = 0, anz = 0;

    if (!isInitialized) initADC();

    for (int i = 0; i < tries; i++) {
        raw = analogRead(vbatPin);
        if ( raw > 0 || raw < 4096 ) {
            sum += raw;
            anz++;
            if ( raw < min ) min = raw;
            if ( raw > max ) max = raw;
        }

        if ( anz >= anzMessungen) break;

        delay(10);
    }
    // tries Messversuche oder anzMessungen erreicht
    if ( anz > 5 ) {    // kleinsten + grössten Wert verwerfen
        sum -= min;
        sum -= max;
        anz -= 2;
    }
    //Log::console(PSTR ("anz: %i av: %i min %i max %i"), anz, sum/anz, min, max );
    return sum / anz;
}


void HajoSat::readAnaloginput() {
    float   Vpoly, pV;
    int     aver = 0;

    aver = readADCaverage();

    Vpoly = PolyVoltage(aver);
    pV    = Vpoly * 2.09;
    Log::console(PSTR ("adc: %i pV: %.2f"), aver, pV );
    status.vBat = pV;
    return;
}
 