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

#define CLIENT "mqtt-list-topics"

/* Configuration variables */

char *broker_url=DEFAULT_BROKER;
char *broker_user=NULL;
char *broker_passwd=NULL;

char *topic_pattern="#";
int listen_time=1;  /* in secs */

extern char clientID[];
int verbose=0;    /* Verbosity level */
int do_json=1;    /* Expand JSON properties by default */

int max_num_topics=512;

#define TIMEOUT     10000L
typedef struct {
  char *topic;
  int anz;
  STRING last_value;
} TOPIC;
TOPIC *topics;
int anztopics;

static int find_topic(const char *topic) {
  int ret=-1;
  if(anztopics>0) {
    int i;
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
static int add_topic(const char *topic, STRING value) {
  static int toldyou=0;
  int i=find_topic(topic);
  if(i>=0) topics[i].anz++;
  else {
    i=anztopics;
    if(anztopics<max_num_topics-1) {
      anztopics++;
      topics[i].anz=1;
      topics[i].last_value.pointer=NULL; 
      topics[i].topic=strdup(topic);
    } else {
      if(!toldyou) {
        printf("# ERROR: too many topics (%d)\n",anztopics);
        printf("# The maximum is set to %d.\n",max_num_topics);
        toldyou=1;
      }
    }
  }
  topics[i].last_value.pointer=realloc(topics[i].last_value.pointer,value.len);
  topics[i].last_value.len=value.len;
  memcpy(topics[i].last_value.pointer,value.pointer,value.len);
  return(i);
}

/* Collect all topics arriving...
 */
void update_topic_message(int sub,const char *topic_name,  STRING message) {
  if(verbose>0) printf("update_topic_message: %s <%s>\n",topic_name,message.pointer);
  add_topic(topic_name,message);
}

static void intro() {
  puts("mqtt-list-topics V.1.03 (c) 2020 by Markus Hoffmann\n"
         "This tool is part of MQTT-Hyperdash, the universal MQTT Dashboard for linux.");
}

static void usage() {
  printf(
    "\nUsage: %s [-hvq] ---\tlist mqtt topics.\n\n"
    "  -h --help\t\t---\tusage\n"
    "  --broker  <url>\t---\tdefine the broker url used [%s]\n"
    "  --user  <user>\t---\tdefine the username for the broker.\n"
    "  --passwd  <passwd>\t---\tdefine the password for the broker.\n"
    "  --pattern <pat>\t---\tset topic pattern [%s]\n"
    "  --wait <seconds>\t---\tlisten for [%d] seconds.\n"
    "  -n <number>\t\t---\tmaximum number of topics to collect [%d]\n"
    "  --json\t\t---\texpand JSON properties. (default)\n"
    "  --nojson\t\t---\tdo not expand JSON properties.\n"
    "  -v\t\t\t---\tbe more verbose\n"
    "  -q\t\t\t---\tbe more quiet\n"
    ,CLIENT,broker_url,topic_pattern,listen_time,max_num_topics);
}
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } else if(!strcmp(argumente[count],"--version"))  {
      intro();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"--broker"))   broker_url=argumente[++count];
    else if(!strcmp(argumente[count],"--user"))     broker_user=argumente[++count];
    else if(!strcmp(argumente[count],"--passwd"))   broker_passwd=argumente[++count];
    else if(!strcmp(argumente[count],"--pattern"))  topic_pattern=argumente[++count];
    else if(!strcmp(argumente[count],"--wait"))     listen_time=atoi(argumente[++count]);
    else if(!strcmp(argumente[count],"-n"))         max_num_topics=atoi(argumente[++count]);
    else if(!strcmp(argumente[count],"--nojson"))   do_json=0;
    else if(!strcmp(argumente[count],"--json"))     do_json=1;
    else if(!strcmp(argumente[count],"-v"))	    verbose++;
    else if(!strcmp(argumente[count],"-q"))	    verbose--;
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the rule itself */
    else {
      /* do nothing, these could be options for rule itself */
    }
  }
  if(quitflag) exit(EX_OK);
}

static void print_value(char *v,int rc) {
  int j;
  for(j=0;j<rc;j++) {
      if(isprint(v[j])) printf("%c",v[j]);
      else printf(".");
  }
}
const char pngheader[]={0x89,'P','N','G',0x0d,0x0a,0x1a,0x0a};
const char jpgheader[]={0xff,0xd8,0xff};
static char *get_type(char *pl, int l) {
  if(!pl || l<=0) return("empty");
  int json_object;
  int json_balance;
  int j;
  double v=myatof(pl);
  if(v!=0) {
    if(v==(double)((int)v)) return("integer");
    else return("number");
  } else {
    int nonprint=0;
    int nonnumber=0;
    int nonspace=0;
    json_object=0;
    json_balance=0;
    char a;
    for(j=0;j<l;j++) {
      a=pl[j];
      if(a=='{' && nonspace==0) json_object=1;
      if(a==':' && json_object==1) json_object=2;
      if(a=='{') json_balance++;
      else if(a=='}') json_balance--;
      if(!isprint(a)) nonprint++;
      if(!isdigit(a)) nonnumber++;
      if(!isspace(a)) nonspace++;
    }
    if(json_object==2 && json_balance==0) {
      return("JSON");
    } else {
      if(nonprint==0) {
        if(nonnumber==0) return("number");
	else return("string");
      } else {
        if(l>40 && (!memcmp(pl,pngheader,sizeof(pngheader)) || !memcmp(pl,jpgheader,sizeof(jpgheader)))) return("IMAGE");
        return("binary");
      }
    }
  }
  return("unknown");
}


int main(int argc, char* argv[]) {
  int rc,i,j;
  char *typ="unknown";
  kommandozeile(argc, argv);    /* process command line */
  add_subscription(topic_pattern,0);
  topics=calloc(max_num_topics,sizeof(TOPIC));
  
  rc=mqtt_broker(broker_url,broker_user,broker_passwd,CLIENT);  /* connect to mqtt broker */
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
      typ=get_type(topics[i].last_value.pointer,topics[i].last_value.len);
      printf("%s \t%d \t%s \t\"",topics[i].topic,topics[i].anz,typ);
      rc=topics[i].last_value.len;
      if(rc>24) rc=24;
      print_value(topics[i].last_value.pointer,rc);
      if(rc<topics[i].last_value.len) printf("(%d)",topics[i].last_value.len);
      printf("\"\n");
      
      if(do_json && !strcmp(typ,"JSON")) {
        char subtopic[256];
	int level=0;
	int flag=0;
	int k=0;
	char a;
        for(j=0;j<topics[i].last_value.len;j++) {
	  a=topics[i].last_value.pointer[j];
          if(a=='\"') flag=!flag;
	  else if(!flag && a=='{') level++;
	  else if(!flag && a=='}') level--;
	  else if(level>=1 && k<sizeof(subtopic)-1 && a!='\r' && a!='\n' && (flag || (a!='\t' && a!=' '))) subtopic[k++]=a;
	}
	subtopic[k]=0;
        char aa[256];
	char b[256];
	char c[256];
	int e=wort_sep(subtopic,',',2|4,aa,b);
        while(e) {
          wort_sep(aa,':',0,aa,c);
	  xtrim(aa,0,aa);
	  xtrim(c,0,c);
	  declose(aa);
	  char *t=get_type(c,strlen(c));
          printf("%s{%s} \t%d \t%s \t\"",topics[i].topic,aa,topics[i].anz,t);
          declose(c);
	  rc=strlen(c);
	  if(rc>24) rc=24;
	  print_value(c,rc);
	  if(rc<strlen(c)) printf("(%d)",(int)strlen(c));
          printf("\"\n");
	  e=wort_sep(b,',',2|4,aa,b);
	}
      }
    }
  }
  mqtt_exit();  /* close connection to broker. */ 
  clear_all_topics();
  free(topics);
  return(EX_OK);
}
