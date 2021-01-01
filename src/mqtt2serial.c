/* MQTT2SERIAL.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/*
   This is a generic application (similar to the rule engine framework) to 
   asynchronoulsy pass mqtt content to a serial line, and also accept 
   asynchronously content from that line.

   This way an arduino could be easily hooked to the serial line and pass its 
   values and accept commands from it. (See the example sketch coming with this 
   repository).


There are two options: eiter use a binary package format or 
use an ASCII representation (which would be much more easy to implement in 
the arduino firmware). However there need to be made several design choices.

The format could be:

MQTT:=, where the payload should be enclosed with "

The TOPIC would be a local topics name, and the framework can add a prefix to 
it to make it available on the mqtt broker. However, this way no binary data 
could be transfered. Here possibly a base64 encoding could be used.

The framework (a simple excecutable running on the (linux/Raspberry), made out 
of the rule engines) could take as commandline parameters:

    serial device
    baud rate
    device settings (like 8:N:1)
    the MQTT prefix for local topics
    behaviour on connection loss (serial device)

Somewhere the list of MQTT topics to listen for need to be defined, maybe as a 
wildcard pattern.

TODO: implement a list of different prefixes (to be added with wildcards...) or 
a list with individual topics.


Run this e.g. with: 

./mqtt2serial --broker tcp://localhost:1883 --prefix "ARDUINO" --device /dev/ttyUSB0 --baudrate 9600 

and watch the topics with 

mosquitto_sub -h localhost -t "ARDUINO/#" -v

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined WINDOWS
  #define EX_OK		  0	/* successful termination */
  #define EX_UNAVAILABLE 69	/* service unavailable */
#else
  #include <sysexits.h>
#endif
#include <sys/time.h>
#include <time.h>

#include <MQTTClient.h>
#include "file.h"

#include "subscribe.h"
#include "mqtt.h"
#include "util.h"
#include "serial.h"

#define CLIENT "mqtt2serial"

#define RECONNECTION_PAUSE 2  /*  2 Seconds */
#define RECONNECTION_TIME 60  /* 60 Seconds */


/* Configuration variables */

char *broker_url=DEFAULT_BROKER;
char *broker_user=NULL;
char *broker_passwd=NULL;
char *topic_pattern="#";
int baudrate=9600;

char device[128]="/dev/ttyUSB0";

extern char clientID[];
int verbose=0;    /* Verbosity level */
int do_persist=0; /* Shall the engine persist to connect ad infinitum? */
int do_json=1;    /* Expand JSON properties by default */

char *topic_prefix="SERIAL"; /* Can be configured */

/* Callback from the mqtt hyperdash rule engine framework. sub=-1 means
  that the topic was not in the subscription list (can happen when wildcards are used) */

void update_topic_message(int sub, const char *topicname, STRING message) {
  char buf[128];
  
  /* Remove the prefix from the topic name and pass the rest to 
     the serial line. 
     TODO: encode binary data to base64.
     */
  char *p;
  if(*topic_prefix) {
    int l=strlen(topic_prefix);
    if(!strncmp(topicname,topic_prefix,l)) {
      p=(char *)topicname+l+1;
    } else {
      if(verbose>=0) printf("WARNING: received unmatched topic <%s>\n",topicname);
      return;
    }
  } else p=(char *)topicname;
     
  sprintf(buf,"MQTT:%s=\"%s\"\n",p,message.pointer);
  if(device_fd!=-1) {
    write(device_fd,buf,strlen(buf));
    fsync(device_fd);
  }
  if(verbose>0) printf("%s",buf);
}



static void usage() {
  printf(
    "\nUsage: %s [-hvq] ---\tbridge to serial devices.\n\n"
    "  -h --help\t\t---\tusage\n"
    "  --broker  <url>\t---\tdefine the broker url used [%s]\n"
    "  --persist\t\t---\tbe persistent even when connection is impossible\t\n"
    "  --prefix <prefix>\t---\tset topic prefix\t\n"
    "  --pattern <pat>\t---\tset topic pattern [%s]\n"
    "  --json\t\t---\texpand JSON properties. (default)\n"
    "  --nojson\t\t---\tdo not expand JSON properties.\n"
    "  --device <devicename>\t---\tSet serial devicename [%s]\n"
    "  --baudrate <rate>\t---\tSet serial baud rate [%d]\n"
    "  -v\t\t\t---\tbe more verbose\n"
    "  -q\t\t\t---\tbe more quiet\n"
    ,CLIENT,broker_url,topic_pattern,device,baudrate);
}

  /* process command line parameters */

static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"--broker"))   broker_url=argumente[++count];
    else if(!strcmp(argumente[count],"--user"))     broker_user=argumente[++count];
    else if(!strcmp(argumente[count],"--passwd"))   broker_passwd=argumente[++count];
    else if(!strcmp(argumente[count],"--pattern"))  topic_pattern=argumente[++count];
    else if(!strcmp(argumente[count],"--persist"))  do_persist=1; 
    else if(!strcmp(argumente[count],"--prefix"))   topic_prefix=argumente[++count];
    else if(!strcmp(argumente[count],"--nojson"))   do_json=0;
    else if(!strcmp(argumente[count],"--json"))     do_json=1;
    else if(!strcmp(argumente[count],"--device"))   strncpy(device,argumente[++count],sizeof(device));
    else if(!strcmp(argumente[count],"--baudrate")) baudrate=atoi(argumente[++count]);
    else if(!strcmp(argumente[count],"-v"))	    verbose++;
    else if(!strcmp(argumente[count],"-q"))	    verbose--;
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the rule itself */
    else {
      /* do nothing, these could be options for rule itself */
    }
  }
  if(quitflag) exit(EX_OK);
}



static char *make_topic(char *name) {
  static char topic[256]; 
  snprintf(topic,sizeof(topic),"%s/%s",topic_prefix,name);
  return(topic);
}

#define PUBLISH_ANY(a,b,c)  snprintf(buf,sizeof(buf),c,a); \
  message_string.pointer=buf; \
  message_string.len=strlen(buf); \
  mqtt_publish(make_topic(b),message_string,0,1)

static int mqtt_publish_status(int status, char *status_s) {
  char buf[256];
  STRING message_string;

  PUBLISH_ANY(status,     "STATUS_DM","%d");
  PUBLISH_ANY(status_s,   "STATUS_SM","%s");
  return(0);
}

/* return a 64 bit UNIX timestamp (in double)*/
static double v_timer() {
  struct timeval t;
  struct timezone tz;
  gettimeofday(&t,&tz);
  return((double)t.tv_sec+(double)t.tv_usec/1000000);
}


/* This procedure is called whenever a complete line has been received from 
   the device.
 */

int processinput(char *line) {
  char buf[256];
  STRING message_string;
  double timestamp=v_timer();
  int l=strlen(line);
  if(l<1) return(0);
  if(line[l-1]=='\r') {line[l-1]=0;l--;}
  if(!strncmp(line,"MQTT:",5)) {
    char *p=line+5;
    while (*p==' ') p++;
    char *topic=p;
    while (*p!='=') p++;
    *p++=0;
    l=strlen(p);
    if(*p=='\"' && p[l-1]=='\"') {
      p[l-1]=0;
      p++;
    }
    if(verbose>0) printf("--> %.13g %s=<%s>\n",timestamp,topic,p); 
    PUBLISH_ANY(p,topic,"%s");
  } else {
    if(verbose>=0) printf("unrecognized output <%s>\n",line);
  }
  return(0);
}


int main(int argc, char* argv[]) {
  int has_been_connected=0;
  int err=0;

  kommandozeile(argc, argv);    /* process command line */
  if(do_persist) has_been_connected++;
   if(verbose>0) {
    printf("Persistence mode=%d\n",do_persist);
    printf("Topic Prefix:    <%s>\n",topic_prefix);
    printf("Device : <%s>\n",device);
  }
  char buf[256];
  if(*topic_prefix) sprintf(buf,"%s/%s",topic_prefix,topic_pattern);
  else sprintf(buf,"%s",topic_pattern); 
  add_subscription(buf,0);
  int rc;

  while(1) {
    again:
    rc=mqtt_broker(broker_url,broker_user,broker_passwd,CLIENT);  /* connect to mqtt broker */
    if(rc==-1) {
      printf("MQTT Error: Could not connect to the MQTT broker %s.\n",broker_url);
      if(!has_been_connected) {
        printf("Quit.\n");
        exit(EX_UNAVAILABLE);
      } else {
        printf("Try to reconnect in %d minute(s).\n",RECONNECTION_TIME/60);
        sleep(RECONNECTION_TIME);
	goto again;
      }
    }
    has_been_connected++;
    mqtt_subscribe_all();
    if(verbose>0) printf("INFO: MQTT engine up and running. Client Id=<%s>\n",clientID);
    /* This is the main loop. */
    while(mqtt_isconnected) {
      if(device_fd==-1) {
        if(device_setup(device,baudrate)) {
          device_init_success=0;
	  perror("Device Setup failed.");
	  mqtt_publish_status(EX_UNAVAILABLE,"Serial Device Setup failed.");
        } else {
          device_init_success=1;
	  if(verbose>=0) printf("Device Setup complete.\n");
  	  mqtt_publish_status(0,"Serial Device up and running.");
        }
      }
      if(device_init_success && device_fd!=-1) {
	while(mqtt_isconnected && (err=device_loop())==0) {  /* Endless loop until error occurs ....*/
          if(verbose>2) printf("++++++ LOOP\n");
        }
	if(err) {  /* Erial device error */
          mqtt_publish_status(1,"Device stopped.");
          if(verbose>=0) printf("Device abort. Closing...\n");
          device_close();
	  device_init_success=0;
          if(!do_persist) break;
          if(verbose>=0) printf("Device waiting 5min to reconnect...\n");
          sleep(300);  /* wait 5 min before try again to reconnect to the device. */
	}
      }
    }
    mqtt_unsubscribe_all();
    printf("INFO: try to reconnect in %d secs...\n",RECONNECTION_PAUSE);
    sleep(RECONNECTION_PAUSE);
  }
  mqtt_exit();  /* Verbindung zum Broker trennen. */ 
  if(!device_init_success) return(EX_UNAVAILABLE);
  if(err) return(EX_IOERR);
  return(EX_OK);
}
