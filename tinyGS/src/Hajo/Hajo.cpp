/*
    Steuerung A
*/

#include "./Hajo.h"

uint32_t    batInterval       = 1000 * 10;
uint32_t    last_read_time_ms = 0;

void HajoSat::control() {
    if (millis() - last_read_time_ms > batInterval) {
        last_read_time_ms = millis();
        vBat();
    };
}