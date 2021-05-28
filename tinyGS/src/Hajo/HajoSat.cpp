/**
 *  Automatische Ermittlung des 'nächsten' Satelliten der gehört werden soll
 */

#include "autoSat.h"
#include "../Logger/Logger.h"
#include "StringSplitter.h"

#include <Sgp4.h>
#include <sgp4pred.h>
#include <Ticker.h>

#include "ArduinoJson.h"
#include "satInfo.h"

using namespace std;

Sgp4 sat;
unsigned long unixtime = 1558950400;
int  timezone = 1 ;  //utc + 1 oder 2 Winter - Sommer >> Sommer-/Winterzeit intern desw 1
int  year; int mon; int day; int hr; int minute; double sec;

//                                Norbi  SDS    FEES   FEES    ShriShakti
//                                                48082        UnitySat-3
//const int   satsToLookFor433[] = {46494, 47721, 48041, 47947, 47716 };
const int   satsToLookFor433[] = {46494};

//                             Lacuna-3  VR3X-A   -B     -C    Lacuna-2B
const int   satsToLookFor866[] = {46492, 47463,  47467, 47524, 47948};

//const char  *urlC      = "https://celestrak.com/NORAD/elements/gp.php?CATNR=25544&FORMAT=TLE";
const char  *urlCeles    = "https://celestrak.com/NORAD/elements/gp.php?CATNR=";
const char  *formatTLE   = "&FORMAT=TLE";

String      allTLE[50][3];

uint32_t    last_tle_read_time_ms = 0;

byte        passes;
const char  selBand = '4';

NextTurn    nextPasses[40] PROGMEM;

NextTurn    satQueue[20] PROGMEM;

Satellite   satellites;

struct      Satellite*  satFront = NULL;
struct      Satellite*  satRear  = NULL;

struct      Scedule*    scedFront = NULL;
struct      Scedule*    scedRear  = NULL;

#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime> 

double      nextScedTime = 0;
time_t      jetzt;

void HajoSat::autoSat() {
    // TLE's laden
    return;
    jetzt = time(NULL);
    
    if (last_tle_read_time_ms == 0) {

        if (satFront == NULL) {
            // initialize Sat structure
            loadSats();
            Log::console(PSTR("Total heap: %d"), ESP.getHeapSize());
            Log::console(PSTR("Free heap: %d"), ESP.getFreeHeap());
            Log::console(PSTR("Total PSRAM: %d"), ESP.getPsramSize());
            Log::console(PSTR("Free PSRAM: %d"), ESP.getFreePsram());
        }

        Serial.println("AutoSat");
        last_tle_read_time_ms = millis();

        time_t  rawtime;
        struct  tm timeinfo;
        timeinfo = *localtime(&rawtime);
        //timeinfo.tm_year = timeinfo.tm_year - 1970; -> ist schon Jahr - 1900 !

        time_t date = mktime(&timeinfo);

        Log::console(PSTR("date: %d"), date );
        Serial.println(String(timeinfo.tm_mday) + '.' + String(timeinfo.tm_mon) + '.' + String(timeinfo.tm_year) + ' ' + String(timeinfo.tm_hour) + ':' + String(timeinfo.tm_min) + ':' + String(timeinfo.tm_sec));
        
        timeinfo = *localtime(&jetzt);
        Log::console(PSTR("jetzt: %s"), asctime(&timeinfo) );

        timeinfo = *gmtime(&jetzt);
        Log::console(PSTR("jetzt UTC: %s"), asctime(&timeinfo) );
        Log::console(PSTR("timeinfo jahr: %d"), timeinfo.tm_year );
        Log::console(PSTR("jetzt: %u"), jetzt );

        

        struct timeval time_now{};
        
        gettimeofday(&time_now, nullptr);
        time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

        Log::console(PSTR("now: %d \n"), msecs_time);

        int anzSats = sizeof(sats4xx) / sizeof(sats4xx[0]);
        Log::console(PSTR("anzSats: %d \n"), anzSats);
        for (int i = 0; i < 10; i++) {
            setSceduleTable();
        }
        printSceduleTable();
        return;     
    } else {
        // all prepared let's scedule
        if ( scedFront == NULL ) return;     // no scedules

        sceduleNextSat();
    }
}

void HajoSat::sceduleNextSat() {
    // we want to scedule first if there was no one sceduled
    if ( nextScedTime == 0 ) {   //scedule first Sat
        sceduleSat();
    }


    if ( scedFront->overpass.jdstartUTC < jetzt ) sceduleSat();
}

void HajoSat::sceduleSat() {

    // void MQTT_Client::manageMQTTData(char *topic, uint8_t *payload, unsigned int length)
    // topic: tinygs/1705665548/DE_72116_433_2/cmnd/begin
    char    topic[44] = "tinygs/1705665548/DE_72116_433_2/cmnd/begin";
    unsigned int  laenge;
    struct Satellite* tempSat = scedFront->satAdr;

    char para[200]   ;
    strcpy(para, tempSat->params);
    uint8_t paraAdr = para[0];

    laenge = strlen(tempSat->params);
    Log::console(PSTR("Länge: %u"), laenge);
    Log::console(PSTR("Länge: %s"), tempSat->params);

    //MQTT_Client::callRemoteSatCmd( topic, *tempSat->params, laenge );
    //MQTT_Client::getInstance().callRemoteSatCmd(topic, paraAdr, laenge);

    nextScedTime = scedFront->overpass.jdstartUTC;

    // scedule löschen
    Scedule* delNode    = scedFront;
    scedFront           = scedFront->next;
    delete delNode;
}

void HajoSat::setSceduleTable() {
    struct Satellite* temp = satFront;
    struct Satellite* nextSat = NULL;
    double lowestDate = 0;

    while(temp != NULL) {
        if (nextSat == NULL) {
            nextSat = temp;
            lowestDate = temp->frontPass->overpass.jdstart;
        } else {
            if (temp->frontPass->overpass.jdstart < lowestDate) {
                nextSat = temp;
                lowestDate = temp->frontPass->overpass.jdstart;
            }
        }
        temp = temp->next;
    }  
    // gefundenen pass in die scedule-queue und dann beim sat entfernen
    if (nextSat != NULL) {

        enqueueSced(nextSat);
        // pass aus LL raus
        // Move the head pointer to the next node

        Passes* nextNode    = nextSat->frontPass;
        nextSat->frontPass  = nextSat->frontPass->next;
        delete nextNode;
    }  
}

void HajoSat::enqueueSced(Satellite* sat) {    

  	struct Scedule* tempSced = 
		(struct Scedule*)malloc(sizeof(struct Scedule));

    tempSced->satAdr    = sat;
    buildScedOverpass(sat, tempSced->overpass);
    //tempSced->overpass  = utcpass;
    tempSced->next      = NULL;

    Log::console(PSTR("enquSced: %f %u"), tempSced->overpass.jdstart, tempSced->overpass.jdstartUTC);

    if (scedFront == NULL) {
        scedFront = scedRear = tempSced;
        return;
    }
    scedRear->next = tempSced;
    scedRear       = tempSced;
}

void HajoSat::buildScedOverpass(Satellite* sat, passinfoH &utcpass) {

    Passes            tempPass;

    //inOverpass  = new passinfo();

    tempPass    = *sat->frontPass;

    utcpass.jdstartUTC         = convertToUTC(tempPass.overpass.jdstart);
    utcpass.jdstopUTC          = convertToUTC(tempPass.overpass.jdstop);
    utcpass.jdmaxUTC           = convertToUTC(tempPass.overpass.jdmax);
    utcpass.jdtransitUTC       = convertToUTC(tempPass.overpass.jdtransit);
    Log::console(PSTR("bldScedOP: %f %u"), tempPass.overpass.jdstart, utcpass.jdstartUTC);

    utcpass.jdstart            = tempPass.overpass.jdstart;
    utcpass.jdstop             = tempPass.overpass.jdstop;
    utcpass.jdmax              = tempPass.overpass.jdmax;
    utcpass.jdtransit          = tempPass.overpass.jdtransit;

    utcpass.maxelevation       = tempPass.overpass.maxelevation;
    utcpass.minelevation       = tempPass.overpass.minelevation;
    utcpass.transitelevation   = tempPass.overpass.transitelevation;

    utcpass.azstart            = tempPass.overpass.azstart;
    utcpass.azmax              = tempPass.overpass.azmax;
    utcpass.azstop             = tempPass.overpass.azstop;
    utcpass.aztransit          = tempPass.overpass.aztransit;

}

time_t HajoSat::convertToUTC(double jdTime) {
    int     year; int mon; int day; int hr; int min; double sec;
    
    struct  tm  timeinfo = {0};
    time_t  utcDate;

    invjday(jdTime, 0, false, year, mon, day, hr, min, sec);

    timeinfo.tm_year    = year - 1900;
    timeinfo.tm_mon     = mon - 1;
    timeinfo.tm_mday    = day;
    timeinfo.tm_hour    = hr;
    timeinfo.tm_min     = min;
    timeinfo.tm_sec     = sec;  

    utcDate = mktime(&timeinfo);

    //Log::console(PSTR("conUTC:jdstart: %f %f %d"), &jdTime, jdTime, &utcDate);
    //Log::console(PSTR("conUTC:jdstart: %f year: %i tm_year: %i mon: %i tm_mon: %i day: %i tm_day: %i "), jdTime, year, timeinfo.tm_year, mon, timeinfo.tm_mon, day, timeinfo.tm_mday );

    return utcDate;
}

void HajoSat::loadSats() {
    char sucheNORAD[] = "NORAD";
    char * pointer;
    char satNoChar[6];

    int anzSats = sizeof(sats4xx) / sizeof(sats4xx[0]);
    for (int i = 0; i < anzSats; i++) {
        // Sat-Nr extrahieren
        pointer = strstr(&sats4xx[i][0], sucheNORAD);
        Log::console(PSTR("i: %d norad: %d \n"), i, pointer);
        if ( !pointer ) continue;
        pointer += 7;   //pointer auf Norad-Nummer stellen
        strncpy(satNoChar, pointer, 5);
        satNoChar[5] = 0;
        Log::console(PSTR("i: %d norad: %d no: %s \n"), i, pointer, satNoChar);

        // linked list für Satelliten anlegen/erweitern
        addSat(i, satNoChar);
        // TLE daten besorgen
        getTleData( satNoChar, satRear );
        // next passes laden
        getNextCrossings(satNoChar, satRear);
        printAllSatInfo();
    };
}

void HajoSat::printSceduleTable() {

    Log::console(PSTR("Sceduler \n"));

    struct Satellite*   tempSat;
    struct Scedule*     temp = scedFront;
    struct tm           timeinfo;
    time_t              datum;
    

    while(temp != NULL) {
        tempSat = temp->satAdr;
        datum = time_t(temp->overpass.jdstartUTC);
        timeinfo = *localtime(&datum);
        Log::console(PSTR("overpass: %5s %u "), tempSat->satNum, temp->overpass.jdstartUTC );
        Log::console(PSTR("  Start: az= %6s ° %s "), String(temp->overpass.azstart), asctime(&timeinfo) );
        datum = time_t(temp->overpass.jdmaxUTC);
        timeinfo = *localtime(&datum);
        Log::console(PSTR("  Max: elev= %6s ° %s "), String(temp->overpass.maxelevation), asctime(&timeinfo) );
        //Log::console(PSTR("  Min: elev= %6s °  "), String(temp->overpass.minelevation) );
        //datum = time_t(temp->overpass.jdtransitUTC);
        //timeinfo = *localtime(&datum);
        //Log::console(PSTR("  Transelev= %6s ° %s "), String(temp->overpass.transitelevation), asctime(&timeinfo) );
        datum = time_t(temp->overpass.jdstopUTC);
        timeinfo = *localtime(&datum);
        Log::console(PSTR("   Stop: az= %6s ° %s "), String(temp->overpass.azstop), asctime(&timeinfo) );

        temp = temp->next;
    }
}

void HajoSat::printAllSatInfo() {

    Log::console(PSTR("Satinfos \n"));

    struct Satellite* temp = satFront;

	while(temp != NULL) {
        Log::console(PSTR("sat: %s \n"), temp->satNum);
        Log::console(PSTR("par: %s \n"), temp->params);
        Log::console(PSTR("tle0: %s \n"), temp->tle.satNam);
        Log::console(PSTR("tle1: %s \n"), temp->tle.line1);
        Log::console(PSTR("tle2: %s \n"), temp->tle.line2);

        Log::console(PSTR("pass: %d \n"), temp->frontPass);
        if ( temp->frontPass ) {
            printPassInfo( temp->frontPass );
        }
      

		temp = temp->next;
	}
	printf("\n");
}

void HajoSat::printPassInfo(Passes* frontPass) {
    int  year; int mon; int day; int hr; int min; double sec;

    struct Passes* temp = frontPass;

    while(temp != NULL) {
        invjday(temp->overpass.jdstart ,timezone ,true , year, mon, day, hr, min, sec);
        Serial.println("Overpass " + String(day) + ' ' + String(mon) + ' ' + String(year));
        Serial.println("  Start: az=" + String(temp->overpass.azstart) + "° " + String(hr) + ':' + String(min) + ':' + String(sec));
        
        invjday(temp->overpass.jdmax ,timezone ,true , year, mon, day, hr, min, sec);
        Serial.println("  Max: elev=" + String(temp->overpass.maxelevation) + "° " + String(hr) + ':' + String(min) + ':' + String(sec));
        
        invjday(temp->overpass.jdstop ,timezone ,true , year, mon, day, hr, min, sec);
        Serial.println("  Stop: az=" + String(temp->overpass.azstop) + "° " + String(hr) + ':' + String(min) + ':' + String(sec));

        temp = temp->next;
    }
}

// To Enqueue next Satellite
void HajoSat::addSat(int index, char * satNo) {
    Satellite* temp = NULL;
	temp = new Satellite();

	strcpy(temp->satNum, satNo); 
    temp->satNum[5] = 0;
    strcpy(temp->params, &sats4xx[index][0]);
	temp->next = NULL;
	if(satFront == NULL){
		satFront = satRear = temp;
		return;
	}
	satRear->next = temp;
	satRear = temp;
}
// aktuelle TLE-Daten zu einem Satelliten lesen
void HajoSat::getTleData(char * satNumber, Satellite* satAct) {
    char        url[256];

    HTTPClient http;

    Log::console(PSTR("satNumber: %s \n"), satNumber);
    sprintf(url, "%s%s%s", urlCeles, satNumber, formatTLE );
 
    Log::console(PSTR("url: %s \n"), url);
    
    
    // Your Domain name with URL path or IP address with path
    http.begin(url);
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        // Trennen in Name, Zeile 1, Zeile 2
        splitTleData(payload, satAct);
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    };
    // Free resources
    http.end();

}

// TLE-Daten splitten in Array mit name, tle1, tle2
void HajoSat::splitTleData(String tleString, Satellite* satAct) {

    String  singleTLE[3];

    StringSplitter *splitter = new StringSplitter(tleString, '\n', 3);  // new StringSplitter(string_to_split, delimiter, limit)
    int itemCount = splitter->getItemCount();
    Serial.println("Item count: " + String(itemCount));

    for(int i = 0; i < itemCount; i++){
        String item = splitter->getItemAtIndex(i);
        singleTLE[i] = item;
        Serial.println("Item @ index " + String(i) + ": " + String(item));
    }
    // TLE-daten bei satellite eintragen/ersetzen
    strncpy(satAct->tle.satNam, singleTLE[0].c_str(), 24);
    satAct->tle.satNam[24] = 0;
    strcpy(satAct->tle.line1,  singleTLE[1].c_str());
    strcpy(satAct->tle.line2,  singleTLE[2].c_str());
}

void HajoSat::getNextCrossings(char * satNoChar, Satellite* satAct) {
    unixtime = time(nullptr);
    Log::console(PSTR("getNextCrossings, unixtime: %d"), unixtime);
    //Log::console(PSTR("getNextCrossings, unixtime: %s"), asctime(gmtime(&unixtime)) );
    sat.initpredpoint( unixtime , 0.0 );     //finds the startpoint
    sat.site(48.4071646, 9.0563024, 475); //set site latitude[°], longitude[°] and altitude[m]

    char satname[25] = {""};
    char tle_line1[129];  //Line one from the TLE data
    char tle_line2[129];  //Line two from the TLE data
    strcpy(satname, satAct->tle.satNam);
    strcpy(tle_line1, satAct->tle.line1);
    strcpy(tle_line2, satAct->tle.line2);

    sat.init(satname,tle_line1,tle_line2);     //initialize satellite parameters 

    predict(9, satNoChar, satAct);   //Calculates the next n overpasses

    //Display TLE epoch time
    double jdC = sat.satrec.jdsatepoch;
    invjday(jdC , timezone, true, year, mon, day, hr, minute, sec);
    Serial.println("Epoch: " + String(day) + '/' + String(mon) + '/' + String(year) + ' ' + String(hr) + ':' + String(minute) + ':' + String(sec));
    Serial.println();
}

void HajoSat::predict(int many, char * satNoChar, Satellite* satAct){
    
    passinfo overpass;                       //structure to store overpass info
    sat.initpredpoint( unixtime , 0.0 );     //finds the startpoint
    double datum;
    bool anzeigen = false;
    
    bool error;
    unsigned long start = millis();
    for (int i = 0; i<many ; i++){
        error = sat.nextpass(&overpass,20);     //search for the next overpass, if there are more than 20 maximums below the horizon it returns false
        delay(0);
        
        if ( error == 1){ //no error, prints overpass information
         if ( anzeigen ) {
          datum = overpass.jdstart;
          Log::console(PSTR("predict jdstart: %f %f"), overpass.jdstart, datum );
          invjday(overpass.jdstart ,timezone ,true , year, mon, day, hr, minute, sec);
          Serial.println("Overpass " + String(day) + ' ' + String(mon) + ' ' + String(year));
          Serial.println("  Start: az=" + String(overpass.azstart) + "° " + String(hr) + ':' + String(minute) + ':' + String(sec));
          
          invjday(overpass.jdmax ,timezone ,true , year, mon, day, hr, minute, sec);
          Serial.println("  Max: elev=" + String(overpass.maxelevation) + "° " + String(hr) + ':' + String(minute) + ':' + String(sec));
          
          invjday(overpass.jdstop ,timezone ,true , year, mon, day, hr, minute, sec);
          Serial.println("  Stop: az=" + String(overpass.azstop) + "° " + String(hr) + ':' + String(minute) + ':' + String(sec));
         }
          switch(overpass.transit){
              case none:
                  break;
              case enter:
                  invjday(overpass.jdtransit ,timezone ,true , year, mon, day, hr, minute, sec);
                  Serial.println("  Enter earth shadow: " + String(hr) + ':' + String(minute) + ':' + String(sec)); 
                  break;
              case leave:
                  invjday(overpass.jdtransit ,timezone ,true , year, mon, day, hr, minute, sec);
                  Serial.println("  Leave earth shadow: " + String(hr) + ':' + String(minute) + ':' + String(sec)); 
                  break;
          }
          switch(overpass.sight){
              case lighted:
                  Serial.println("  Visible");
                  break;
              case eclipsed:
                  Serial.println("  Not visible");
                  break;
              case daylight:
                  Serial.println("  Daylight");
                  break;
          }
  
            strcpy(nextPasses[passes].satNum, satNoChar);
            nextPasses[passes].overpass = overpass;
            passes++;
            // append overpass to passes of sat
            enqueuePass(&overpass, satAct);
        }else{
            Serial.println("Prediction error");
            Log::console(PSTR("Prediction error \n"));
        }
        delay(0);
    }
    unsigned long einde = millis();
    Serial.println("Time: " + String(einde-start) + " milliseconds");
    
}
// To Enqueue overpass
void HajoSat::enqueuePass(passinfo* overpass, Satellite* satAct) {
	struct Passes* temp = 
		(struct Passes*)malloc(sizeof(struct Passes));

	temp->overpass.jdstart          =  overpass->jdstart; 
    temp->overpass.jdstop           =  overpass->jdstop; 
    temp->overpass.jdmax            =  overpass->jdmax; 
    temp->overpass.jdtransit        =  overpass->jdtransit; 
    temp->overpass.maxelevation     =  overpass->maxelevation; 
    temp->overpass.minelevation     =  overpass->minelevation; 
    temp->overpass.transitelevation =  overpass->transitelevation; 
    temp->overpass.azstart          =  overpass->azstart; 
    temp->overpass.azstop           =  overpass->azstop; 
    temp->overpass.aztransit        =  overpass->aztransit; 

	temp->next = NULL;
	if(satAct->frontPass == NULL && satAct->rearPass == NULL){
		satAct->frontPass = satAct->rearPass = temp;
		return;
	}
	satAct->rearPass->next = temp;
	satAct->rearPass = temp;
}