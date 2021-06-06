
// erste Dimension anpassen wenn sich Anzahl Einträge ändert! Unschön aber geht
const char sats4xx[3][200] = {
 //   {"{\"mode\":\"LoRa\",\"freq\":436.703,\"bw\":250,\"sf\":10,\"cr\":5,\"sw\":18,\"pwr\":5,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":true,\"fldro\":1,\"sat\":\"Norbi\",\"NORAD\":46494}"}, 
 //   {"{\"mode\":\"LoRa\",\"freq\":437.200,\"bw\":125,\"sf\":9,\"cr\":5,\"sw\":18,\"pwr\":5,\"cl\":120,\"pl\":6,\"gain\":0,\"crc\":true,\"fldro\":0,\"sat\":\"FEES\",\"NORAD\":48082}"},
 //   {"{\"mode\":\"LoRa\",\"freq\":435.500,\"bw\":125,\"sf\":9,\"cr\":7,\"sw\":18,\"pwr\":5,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":true,\"fldro\":0,\"sat\":\"SDSat\",\"NORAD\":47721}"},

 //   {"{\"mode\":\"LoRa\",\"freq\":915.600,\"bw\":62.5,\"sf\":7,\"cr\":8,\"sw\":18,\"pwr\":20,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":false,\"fldro\":0,\"sat\":\"VR3X-A\",\"NORAD\":47463}"},
 //   {"{\"mode\":\"LoRa\",\"freq\":915.600,\"bw\":62.5,\"sf\":7,\"cr\":8,\"sw\":18,\"pwr\":20,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":false,\"fldro\":0,\"sat\":\"VR3X-B\",\"NORAD\":47467}"},
 //   {"{\"mode\":\"LoRa\",\"freq\":915.600,\"bw\":62.5,\"sf\":7,\"cr\":8,\"sw\":18,\"pwr\":20,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":false,\"fldro\":0,\"sat\":\"VR3X-C\",\"NORAD\":47524}"},

    {"{\"mode\":\"LoRa\",\"freq\":863.000,\"bw\":62.5,\"sf\":7,\"cr\":8,\"sw\":18,\"pwr\":20,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":false,\"fldro\":0,\"sat\":\"VR3X-A\",\"NORAD\":47463}"},
    {"{\"mode\":\"LoRa\",\"freq\":867.600,\"bw\":62.5,\"sf\":7,\"cr\":8,\"sw\":18,\"pwr\":20,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":false,\"fldro\":0,\"sat\":\"VR3X-B\",\"NORAD\":47467}"},
    {"{\"mode\":\"LoRa\",\"freq\":870.600,\"bw\":62.5,\"sf\":7,\"cr\":8,\"sw\":18,\"pwr\":20,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":false,\"fldro\":0,\"sat\":\"VR3X-C\",\"NORAD\":47524}"},
    };

const char sats8xx[20][200] = {
    };

const char* satsJson = "{\"mode\":\"LoRa\",\"freq\":436.703,\"bw\":250,\"sf\":10,\"cr\":5,\"sw\":18,\"pwr\":5,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":true,\"fldro\":1,\"sat\":\"Norbi\",\"NORAD\":46494}";
//             , {\"satNum\": \"48041\", \"band\": \"2\", \"mode\":\"LoRa\",\"freq\":437.200,\"bw\":125,\"sf\":9,\"cr\":5,\"sw\":18,\"pwr\":5,\"cl\":120,\"pl\":8,\"gain\":0,\"crc\":true,\"fldro\":1,\"sat\":\"FEES\",\"NORAD\":48041}]";


// Original
// {"mode":"LoRa","freq":437.2,"bw":125,"sf":9,"cr":5,"sw":18,"pwr":5,"cl":120,"pl":6,"gain":0,"crc":true,"fldro":0,"sat":"FEES","NORAD":99738}
// {"mode":"LoRa","freq":436,"bw":125,"sf":10,"cr":8,"sw":52,"pwr":5,"cl":120,"pl":8,"gain":0,"crc":true,"fldro":0,"sat":"Sri Shakthi Sat","NORAD":99899}
// {"mode":"LoRa","freq":435.5,"bw":125,"sf":9,"cr":7,"sw":18,"pwr":5,"cl":120,"pl":8,"gain":0,"crc":true,"fldro":0,"sat":"SDSat","NORAD":47721}
// {"mode":"LoRa","freq":436.703,"bw":250,"sf":10,"cr":5,"sw":18,"pwr":5,"cl":120,"pl":8,"gain":0,"crc":true,"fldro":1,"sat":"Norbi","NORAD":46494}
// 
// {"mode":"LoRa","freq":915.6,"bw":62.5,"sf":7,"cr":8,"sw":18,"pwr":20,"cl":120,"pl":8,"gain":0,"crc":false,"fldro":0,"sat":"VR3X-A","NORAD":0,"filter":[1,0,51]}
// {"mode":"LoRa","freq":915.6,"bw":62.5,"sf":7,"cr":8,"sw":18,"pwr":20,"cl":120,"pl":8,"gain":0,"crc":false,"fldro":0,"sat":"VR3X-B","NORAD":0,"filter":[1,0,51]}
// {"mode":"LoRa","freq":915.6,"bw":62.5,"sf":7,"cr":8,"sw":18,"pwr":20,"cl":120,"pl":8,"gain":0,"crc":false,"fldro":0,"sat":"VR3X-C","NORAD":0,"filter":[1,0,51]}
//
// {"mode":"LoRa","freq":433.3,"bw":125,"sf":7,"cr":7,"sw":18,"pwr":5,"cl":120,"pl":8,"gain":0,"crc":true,"fldro":0,"sat":"ISM_433","NORAD":99999}
// {"mode":"LoRa","freq":903.9,"bw":125,"sf":7,"cr":5,"sw":52,"pwr":5,"cl":120,"pl":8,"gain":0,"crc":true,"fldro":0,"sat":"VUCSCHAB-1","NORAD":99999,"filter":[3,2,22,2,38]}