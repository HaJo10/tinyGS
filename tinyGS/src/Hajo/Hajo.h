/*
    Steuerung
*/

#ifndef HAJO_SAT
#define HAJO_SAT

#include "../ConfigManager/ConfigManager.h"
#include "../Status.h"
#include <HTTPClient.h>
#include <Sgp4.h>
#include <sgp4pred.h>

extern Status status;



extern Status status;

class HajoSat 
{
    public:
        
        static void control();      // Steuerung der Aufrufe Bat-Control, Sat-Auwahl, Sat-Daten nachladen
        static void vBat();
};

#endif
