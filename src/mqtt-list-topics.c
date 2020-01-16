/* MQTT_LIST_TOPICS.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
/* This little tool lists the topics available on one broker.

   It subscribes to one broker for all topics and listens for about 5 
   seconds. All collected information is printed to stdout. 
   Also it tries to guess the type of topic analyzing the payload.
   This works only, if the topics have a retained value.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#if defined WINDOWS
  #define EX_OK		  0	/* successful termination */
  #define EX_UNAVAILABLE 69	/* service unavailable */
#else
  #include <sysexits.h>
#endif

#include <MQTTClient.h>
#include "basics.h"
#include "subscribe.h"
#include "mqtt.h"
#include "util.h"


#define USER NULL
#define PASSWD NULL

#define CLIENT "mqtt-list-topics"

/* Configuration variables */

char *broker_url=DEFAULT_BROKER;
char *topic_pattern="#";
int listen_time=1;  /* in secs */

extern char clientID[];
int verbose=0;    /* Verbosity level */


#define TIMEOUT     10000L
typedef struct {
  char *topic;
  int anz;
  STRING last_value;
} TOPIC;
TOPIC topics[512];
int anztopics;

static int find_topic(const char *topic) {
  int ret=-1;
  int i;
  if(anztopics>0) {
    for(i=0;i<anztopics;i++) {
      if(!strcmp(topic,topics[i].topic)) return(i); 
    }
  }
  return(ret);
}
#if 0
static void clear_topic(int idx) {
  if(idx>=0 && idx<anztopics) {
    topics[idx].anz=0;
    free(topics[idx].topic);
    topics[idx].topic=NULL;
    free(topics[idx].last_value.pointer);
    topics[idx].last_value.len=0;
    topics[idx].last_value.pointer=NULL;
    if(idx==anztopics-1) anztopics--;
  }
  while(anztopics>0 && topics[anztopics-1].anz==0) {
     anztopics--;
  }
}
#endif
static void clear_all_topics() {
  int i;
  if(anztopics>0) {
    for(i=0;i<anztopics;i++) {
      topics[i].anz=0;
      free(topics[i].topic);
      topics[i].topic=NULL;
      free(topics[i].last_value.pointer);
      topics[i].last_value.len=0;
      topics[i].last_value.pointer=NULL;  
    }
  }
  anztopics=0;
}
int add_topic(const char *topic, STRING value) {
  int i=find_topic(topic);
  if(i>=0) topics[i].anz++;
  else {
    i=anztopics;
    anztopics++;
    topics[i].anz=1;
    topics[i].last_value.pointer=NULL; 
    topics[i].topic=strdup(topic);
  }
  topics[i].last_value.pointer=realloc(topics[i].last_value.pointer,value.len);
  topics[i].last_value.len=value.len;
  memcpy(topics[i].last_value.pointer,value.pointer,value.len);
  return(i);
}

void clear_all_topics();
int add_topic(const char *topic, STRING value);

/* Collect all topics arriving...
 */
void update_topic_message(int sub,const char *topic_name,  STRING message) {
  if(verbose>0) printf("update_topic_message: %s <%s>\n",topic_name,message.pointer);
  add_topic(topic_name,message);
}

static void intro() {
  puts("mqtt-list-topics V.1.02 (c) 2020 by Markus Hoffmann\n"
         "This tool is part of MQTT-Hyperdash, the universal MQTT Dashboard for linux.");
}

static void usage() {
  printf(
    "\nUsage: %s [-hvq] ---\tlist mqtt topics.\n\n"
    "  -h --help\t\t---\tusage\n"
    "  --broker  <url>\t---\tdefine the broker url used [%s]\n"
    "  --pattern <pat>\t---\tset topic pattern [%s]\n"
    "  --wait <seconds>\t---\tlisten for [%d] seconds.\n"
    "  -v\t\t\t---\tbe more verbose\n"
    "  -q\t\t\t---\tbe more quiet\n"
    ,CLIENT,broker_url,topic_pattern,listen_time);
}
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"--version"))  {
      intro();
      quitflag=1;
    } else if(!strcmp(argumente[count],"--broker"))   broker_url=argumente[++count];
    else if(!strcmp(argumente[count],"--pattern"))  topic_pattern=argumente[++count];
    else if(!strcmp(argumente[count],"--wait"))     listen_time=atoi(argumente[++count]);
    else if(!strcmp(argumente[count],"-v"))	     verbose++;
    else if(!strcmp(argumente[count],"-q"))	     verbose--;
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the rule itself */
    else {
      /* do nothing, these could be options for rule itself */
    }
  }
  if(quitflag) exit(EX_OK);
}



int main(int argc, char* argv[]) {
  int rc,i,j;
  double v;
  char *typ="unknown";
  kommandozeile(argc, argv);    /* process command line */
  add_subscription(topic_pattern,0);
  
  
  rc=mqtt_broker(broker_url,USER,PASSWD,CLIENT);  /* connect to mqtt broker */
  if(rc==-1) {
    printf("MQTT Error: Could not connect to the MQTT broker %s.\n",broker_url);
    printf("Quit.\n");
    exit(EX_UNAVAILABLE);
  }
  mqtt_subscribe_all();
  if(verbose>0) printf("INFO: list-topic up and listening. Client Id=<%s>\n",clientID);
    /* This is the main loop. */
    while(mqtt_isconnected && listen_time>0) {
      sleep(1);
      listen_time--;
    }
  mqtt_unsubscribe_all();

  if(verbose>-1) printf("# Collected Information about %d topics in %d seconds on %s.\n",anztopics,listen_time,broker_url);
  if(anztopics>0) {
    for(i=0;i<anztopics;i++) {
      v=myatof(topics[i].last_value.pointer);
      if(v!=0) {
        if(v==(double)((int)v)) typ="integer";
        else typ="number";
      } else {
        int nonprint=0;
	int nonnumber=0;
	char a;
        for(j=0;j<topics[i].last_value.len;j++) {
	  a=topics[i].last_value.pointer[j];
	  if(!isprint(a)) nonprint++;
	  if(!isdigit(a)) nonnumber++;
	
	}
	// printf("nonprint=%d----",nonprint);
        if(nonprint==0) {
	  if(nonnumber==0) typ="number";
	  else typ="string";
        } else typ="binary";
      }
      printf("%s \t%d \t%s \t\"",topics[i].topic,topics[i].anz,typ);
      rc=topics[i].last_value.len;
      if(rc>16) rc=16;
      char a;
      for(j=0;j<rc;j++) {
	  a=topics[i].last_value.pointer[j];
	  if(isprint(a)) printf("%c",a);
	  else printf(".");
      }
      printf("\"\n");
    }
  }
  clear_all_topics();
  mqtt_exit();  /* Verbindung zum Broker trennen. */ 
  return(EX_OK);
}
