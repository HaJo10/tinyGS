/**
 *  Automatische Ermittlung des 'nächsten' Satelliten der gehört werden soll
 * 
 *  Linked List um dynamisch Satelliten Zu-/Abgänge verwalten zu können
 *  1. Anlegen Eintrag in Liste für Satelitten
 *  2. TLE-Daten laden falls nicht vorhanden oder zu alt (noch zu definieren)
 *  2. (nach-)laden der nächsten Durchgänge
 */

#ifndef HAJO_SAT
#define HAJO_SAT
#include "../ConfigManager/ConfigManager.h"
#include "../Status.h"
#include <HTTPClient.h>
#include <Sgp4.h>
#include <sgp4pred.h>
#include "../Mqtt/MQTT_Client.h"

extern Status status;

struct passinfoH
{
    time_t  jdstartUTC;
    time_t  jdstopUTC;
    time_t  jdmaxUTC;
    time_t  jdtransitUTC;
  double jdstart;
  double jdstop;
  double jdmax;
  double jdtransit;

  double maxelevation;
  double minelevation;  //elevation at start and end
  double transitelevation;

  double azstart;
  double azmax;
  double azstop;
  double aztransit;
};

struct NextTurn {
    char        satNum[6];                      // sat number as char
    passinfo    overpass;                       // structure to store overpass info
};

struct TLE {
    char        satNam[25];                     // sat name
    char        line1[81];                      // TLE line 1
    char        line2[81];                      // TLE line 2
};

struct Passes {
    passinfo    overpass;
    struct      Passes* next;
};

struct  Satellite {
    char        satNum[6];                      // sat number as character
    char        params[200];
    TLE         tle;
    uint        anzPasses = 0;
    Satellite*  next;
    Passes      nextPasses;
    Passes*     frontPass = NULL;
    Passes*     rearPass = NULL;
};

// LL scedule for sats
struct Scedule {
    passinfoH   overpass;
    Satellite*  satAdr;
    struct      Scedule* next;  
};

// scedule times for checks
double      nextScedTime = 0;
passinfoH   ongoingpass;

time_t      startMaint;                     // Start / Ende Maintanance Fenster (nachladen TLE, next passes...)
time_t      endMaint;                       // Start: Ende akt pass + 2 min, Ende next pass begin - 5 min
int         delayMaint          = 2 * 60;
int         stopMaintbefore     = 5 * 60;
Satellite*  satAdrNextLoadTLE   = NULL;
Satellite*  satAdrNextLoadPass  = NULL;

time_t      timeLastTLEload = 0;
int         delayRefreshTLE = 8 * 60 * 60;  // 8 h 

int         threshholdLoadpasses = 3;       // load next passes when less than this remaining


class HajoSat
{
    private:

    public:
        static void autoSat();
        static void sceduleNextSat();
        static void sceduleSat();

        //void callRemoteSatCmd() = MQTT_Client.callRemoteSatCmd();

        static void getTleData(char * satNumber, Satellite* satAct);
        static void splitTleData(String, Satellite* satAct);
        static void getNextCrossings(char * satNoChar, Satellite* satAct);
        static void predict(int many, char * satNoChar, Satellite* satAct);
        static void loadSats();
        static void printAllSatInfo();
        static void printPassInfo(Passes* frontPass);
        static void printSceduleTable();
        static void addSat(int index, char * satNo);
        static void enqueuePass(passinfo* overpass, Satellite* satAct);
        static void setSceduleTable();
        static void enqueueSced(Satellite* sat);
        static void buildScedOverpass(Satellite* sat, passinfoH &newpassinfo);
        static time_t convertToUTC(double jdTime);

        static void refreshTLE();
        static void loadNextPasses();
        static void checkForSleep();

        static void shutOffGPS();
};

#endif