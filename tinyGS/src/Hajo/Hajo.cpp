/*
    Steuerung A
*/

#include "./Hajo.h"
#include "Hajo_Defs.h"

uint32_t    batInterval       = 1000 * 20;
uint32_t    last_read_time_ms = 0;


void HajoSat::control() {

    if (millis() - last_read_time_ms > batInterval) {
        if ( boardNr == 99 ) {
            boardNr    = ConfigManager::getInstance().getBoard();
        }

        last_read_time_ms = millis();
        //vBat(boardNr);


        autoSat(boardNr);
    };

    //autoSat();
}