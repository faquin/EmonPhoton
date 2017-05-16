// This #include statement was automatically added by the Particle IDE.
#include <blynk.h>
#include <ThingSpeak.h>
#include <EmonLib.h>



const int INTERVAL_MEASURE = 30; // seconds
unsigned long TimeLastMeasure=0;

EnergyMonitor emon1;
double avg_30sec_Irms=0;
int nb_avg=0;
char str[64];

/* Thingspeak */
TCPClient client;
int myChannelNumber = 260886;
const char * myWriteAPIKey = "I50CXZXMZ4IU7B6Z";

// Blynk
char auth[] = "61b76d31d9924683853968782a6eac53";
const int INTERVAL_blynk = 1; // seconds
unsigned long TimeLastBlynk=0;

BLYNK_WRITE(V2) //Button Widget is writing to pin V2
{
  int pinData = param.asInt(); 
  emon1.current(A1, pinData);
}


void setup()
{
    // turn off core LED
	RGB.control(true);
	RGB.color(0, 0, 0);
	
	ThingSpeak.begin(client);
	
	Blynk.begin(auth);
	
    Serial.begin(9600);

    emon1.current(A1, 101);             // Current: input pin, calibration.

}

void loop()
{
        // faire la moyenne sur 30sec, ne marche pas encore
    //double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    //avg_30sec_Irms = (avg_30sec_Irms + Irms)/(++nb_avg);
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    avg_30sec_Irms += Irms;
    nb_avg++;

   
    if ((Time.now() - TimeLastMeasure) >= INTERVAL_MEASURE) { // interval=300sec=5min
		TimeLastMeasure = Time.now();
		double tmp_30sec_Irms = avg_30sec_Irms/nb_avg;
		
        Serial.println(tmp_30sec_Irms);             // Irms
        ThingSpeak.setField(1, (float)tmp_30sec_Irms);
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        sprintf(str,"{\Irms_30s\: %.3f}", tmp_30sec_Irms);
        Particle.publish("emoncms", str);
        //Particle.publish("nb_avg", String::format("%d",nb_avg));
        
        avg_30sec_Irms=0;
        nb_avg=0;
        
    }
    if((Time.now() - TimeLastBlynk) >= INTERVAL_blynk){
        Blynk.virtualWrite(1, Irms);
        TimeLastBlynk = Time.now();
        Blynk.run();
    }
}
