/* demonstration of the MQTT interace introduced with MQTT-Hyperdash.
 * (c) Markus Hoffmann 2020
 */

#include <DHT.h>
#define DHTPIN            7         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)

#define HAVE_OLED 1   /* If an OLED Display is connected */
#include "Arduino.h"
#ifdef HAVE_OLED
#include "OLED.h"
#endif
#include "util.h"

#define FIRMWARE_VERSION "1.13"  /* Version of firmware */
#define MAGIC_VALUE 4711
#define ledPin 13     // Status LED on the Arduino board

signed char verbose;
unsigned int mem_valid __attribute__ ((section (".noinit")));
long int lastmeasurement;
long int lastactivity;
void push_mqtt_topic(const char *topic,char *payload);
void push_mqtt_topic_number(const char *topic,int n);

DHT dht(DHTPIN, DHTTYPE);

#define CMD_BUF_LEN 256
char cmd[CMD_BUF_LEN];
char cmd_i=0;

void reset() {
  if(mem_valid!=MAGIC_VALUE ) {  /* Initialisiere nur nach Trennung der Stromversorgung.*/
    push_mqtt_topic("BOOT",(char *)"cold");
  } else {
    push_mqtt_topic("BOOT",(char *)"warm");
  }
  verbose=0;
 // eeprom_read_values();
  digitalWrite(ledPin, 0);
  mem_valid=MAGIC_VALUE;  /* Magig value, to check. if ram was cleared or not between reboots*/
}


void intro() {
#ifdef HAVE_OLED
  oled.clear();
  show_logo();
  oled.setCursor(48,1);oled.set2X();
  oled.println(F("INIT"));oled.set1X();
  oled.print(F(FIRMWARE_VERSION));
#endif
  push_mqtt_topic("FIRMWARE",(char *)FIRMWARE_VERSION);
}




void setup(void) {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1);
  Serial.begin(115200);
  dht.begin();

#ifdef HAVE_OLED
  OLED_setup();
#endif

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if(verbose>=0) intro();
  reset();

  if(verbose>=0) delay(1000); /* das noch, damit intro() länger stehen bleibt */

  bat_status(59);
  lastactivity=lastmeasurement=millis();
}

void do_mqtt(char *topic,char *p) {
  if(!strcmp(topic,"VERBOSE_DC")) verbose=atoi(p);
  else if(!strcmp(topic,"ACTIVITY_DM")) ; /* silently ignore the topic, sent back*/
  else if(!strcmp(topic,"BOOT")) ; /* silently ignore the topic, sent back*/
  else if(!strcmp(topic,"FIRMWARE")) ; /* silently ignore the topic, sent back*/
  else if(!strcmp(topic,"TEMP_AM")) {
    oled.set2X();
    oled.setCursor(0,0);
    oled.print(p);
    oled.print(F(" "));
    oled.set1X();
  }
  else if(!strcmp(topic,"HUMI_AM")) {
    oled.set2X();
    oled.setCursor(0,2);
    oled.print(p);
    oled.print(F(" "));
    oled.set1X();
  }
  else if(!strcmp(topic,"TP_AM")) ; /* silently ignore the topic, sent back*/
  else if(!strcmp(topic,"STATUS_DM")) {
    oled.setCursor(80,0);
    oled.print(F("S: "));
    oled.println(p);
  } 
  else if(!strcmp(topic,"STATUS_SM")) {
    oled.setCursor(0,3);
    oled.println(p);
  } 
  else if(!strcmp(topic,"CMD_DC")) {
    int cmd=atoi(p);
    if(cmd==99) reset();  
  }
  else {
    Serial.print  (F("I: unrecognized TOPIC: "));
    Serial.println(topic);
  }
}

/* Commands, which come via serial interface.
 *
 */
void docommand(char *p) {
  while (*p==' ') p++;
  switch(*p++) {
  case '?':
    Serial.println (F("HELP:\n=====\n? -- HELP\n"
    "i -- intro\n"
    "v/q -- be more verbose / quiet\n"
    "R -- reset to default values\n"
    "X get debug values\n"
    ));
    break;
  case 'M':
    if(*p=='Q' && p[1]=='T' && p[2]=='T') {
      p+=3;
      while (*p==' ') p++;
      if(*p==':') {
        char *topic;
        p++;
        while (*p==' ') p++;
        topic=p;
        while (*p!='=') p++;
        *p++=0;
        int l=strlen(p);
        if(*p=='\"' && p[l-1]=='\"') {
          p[l-1]=0;
          p++;
        }
        do_mqtt(topic,p);
      }
    }  else {
      Serial.print(F("I: Unknown cmd: <"));
      Serial.print (p-1);Serial.println(F(">"));
    }
    break;
  case 'R':  /* RESET */
    while (*p==' ') p++;
    Serial.println (F("I: RESET."));
    if(*p>='1') mem_valid=0; 
    if(*p>='2') software_Reset(); 
    /* Hard Reset: Also reset counters etc...*/
    reset();
    break;
  case 'X':  /* Memdump */
    while (*p==' ') p++;
    memdump((byte *)atoi(p),16);
    break;
  case 'i':
    intro();
    break;
  case 'v': verbose++; break;
  case 'q': verbose--; break;
  default:
    Serial.print(F("I: Unknown cmd: <"));
    Serial.println (p-1);
  }
}


void push_mqtt_topic(const char *topic,char *payload) {
  Serial.print(F("MQTT:"));
  Serial.print(topic);
  Serial.print("=");
  Serial.print("\"");
  Serial.print(payload);
  Serial.println("\"");
}
void push_mqtt_topic_number(const char *topic,int n) {
  Serial.print(F("MQTT:"));
  Serial.print(topic);
  Serial.print("=");
  Serial.println(n);
}
void push_mqtt_topic_numberf(const char *topic,float n,const char *unit) {
  Serial.print(F("MQTT:"));
  Serial.print(topic);
  Serial.print(F("="));
  Serial.print(n);
  Serial.print(F(" "));
  Serial.println(unit);
}


int activity_dm=0;


void loop(void) {
  long int a=millis()-lastactivity;
  long int b=millis()-lastmeasurement;
/*  Do the activity beat...
 */
  if(a>1000) { 
    Serial.print(F("MQTT:"));
    Serial.print(F("ACTIVITY_DM="));
    Serial.println(activity_dm);

    oled.setCursor(96,0);
    if(activity_dm==0) oled.print("|");
    else if(activity_dm==1) oled.print("/");
    else if(activity_dm==2) oled.print("-");
    else if(activity_dm==3) oled.print("\\");
    
    activity_dm++;
    if(activity_dm>=4) activity_dm=0;
    lastactivity=millis(); 
  }
  /* Do the measurement */
  if(b>5000) { 
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);  
    push_mqtt_topic_numberf("TEMP_AM",t,"C");
    push_mqtt_topic_numberf("HUMI_AM",h,"%");
    push_mqtt_topic_numberf("TP_AM",hic,"C");
    lastmeasurement=millis(); 
  }
 /*  Collect bytes from the console to perform commands... 
 *  The serial library uses a 128 bytes input buffer....
 */
 while (Serial.available() > 0) {
    cmd[cmd_i]=Serial.read();
    if(cmd[cmd_i]==10 || cmd[cmd_i]==13) {
      cmd[cmd_i]=0;
      if(cmd_i>0) {
        docommand(cmd);
        cmd_i=0;
      }
    } else if(cmd_i<CMD_BUF_LEN) cmd_i++;
  }
  delay(10); /* let serial stuff finish */
}
