/* RULE-ENGINE.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
/* Rune-engine base framework.
   This is the part of the code which is identical for all engines. 
   The individual engines are defined in .rule files.  

Ruleengines are ment to be applications running completely in the background. 
They do not interact directly with user. Their function is, to subscribe to a 
set of topics, watch their updates and trigger a routine, which calculates 
something based on the input topics and finally puplish them to output topics, 
which then can trigger other rule engines. They also can perform actions on the
machine they are running, like excecuting shell scripts when a topic content 
matches a certain pattern. With multuple rule engines running even on different 
computers using the same broker, one can implmenent a full automation control, 
which would create the Internet of Things. However the concept of rule engines 
is not new and can be found in other automation concepts as well. This 
implementation is focused on high performance quick reaction, and reliablility. 

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

#include <MQTTClient.h>
#include "basics.h"
#include "subscribe.h"
#include "mqtt.h"
#include "util.h"

/* Possible parameter types */

#define PARM_IN 1
#define PARM_TRIGGER 2
#define PARM_OUT 4
#define PARM_ISNUMBER 8

typedef struct {
  int type;     /* Type of the parameter, a combination of 
                   PARM_IN PARM_TRIGGER PARM_OUT PARM_ISNUMBER*/
  char *topic;  /* The topics name */
  int qos;      /* The Quality of Service to be used for 
                   subscription and publication*/
  int sub;      /* In index to the subscription table, initialize with 0*/
  char *format; /* A format string to format floating point numbers to 
                   the topics content, BASIC like or printf-like */
} PARMDEF;

typedef struct {
  double value;  /* for numeric params */
  STRING string; /* for raw or string params */
} PARMBUF;

typedef struct {
  PARMDEF *params; /* The List of parameters/topics */
  int numparms;    /* Number of parameters defined*/
  void (*initrule)(); /* The initialization rule */
  void (*rule)();     /* The trigger rule */
} RULEDEF;


void (*measure_rule)(PARMBUF *);  /* Measure routine, if NULL, will be ignored.
Note: we rely on the fact, that global variables will ne initialized with 
zero by default.*/

#define ACTIVITY_INTERVAL  1  /*  1 Second */
#define RECONNECTION_PAUSE 2  /*  2 Seconds */
#define RECONNECTION_TIME 60  /* 60 Seconds */


/* Configuration variables */

extern char clientID[];
int verbose=0;    /* Verbosity level */
int do_persist=0; /* Shall the engine persist to connect ad infinitum? */

char *topic_prefix=""; /* Can be configured */

static char *make_topic(char *n) {
  char *buf=malloc(256);
  if(topic_prefix && *topic_prefix) {
    strncpy(buf,topic_prefix,256);
    strncat(buf,"/",256);
  } else *buf=0;
  strncat(buf,n,256);
  return(buf);
}


/* This function shall be used to publish results of a rule... */

void publish_if_different(PARMDEF *pd, PARMBUF *parm) {
  char buffer[]="%g";
  STRING f;
  STRING message;
  if((pd->type&PARM_OUT)==PARM_OUT) {
    if((pd->type&PARM_ISNUMBER)==PARM_ISNUMBER) {
      if(pd->format) {
        f.pointer=pd->format;
	f.len=strlen(f.pointer);
      } else {
        f.pointer=buffer;
	f.len=2;
      }
      message=do_using(parm->value,f);
    } else {
      message=double_string(&(parm->string));
    }
    if(message.len!=subscriptions[pd->sub].last_value.len ||
      memcmp(message.pointer,subscriptions[pd->sub].last_value.pointer,message.len)) {
      if(verbose>0) printf("Publish new value to %s\n",subscriptions[pd->sub].topic);
      mqtt_publish(subscriptions[pd->sub].topic,message,subscriptions[pd->sub].qos,1);
    }
    free(message.pointer);
  }
}


void default_measure_loop();

/************* include the rule definition **************************/
#include RULE
/********************************************************************/

int num_rules=sizeof(rules)/sizeof(RULEDEF);

#define TIMEOUT     10000L
/*
  check if topic was a triggertopic. if so, make a snapshot of all input params
  and  call the rule procedure.

  afterwards format output params and publish
 */
 
 
 
static void publish_results(RULEDEF *rule,PARMBUF *parms) {
  if(!rule || !parms) return;
  int k;
  for(k=0;k<rule->numparms;k++) {
    if((rule->params[k].type&PARM_OUT)==PARM_OUT) 
      publish_if_different(&(rule->params[k]), &parms[k]);
  }
}
static PARMBUF *make_snapshot(RULEDEF *rule) {
  int k;
  if(!rule) return(NULL);
  PARMBUF *params=calloc(rule->numparms,sizeof(PARMBUF));
  for(k=0;k<rule->numparms;k++) {
    params[k].string=double_string(&(subscriptions[rule->params[k].sub].last_value));
    if((rule->params[k].type&PARM_ISNUMBER)==PARM_ISNUMBER) {
      params[k].value=myatof(params[k].string.pointer);
    }
    if(verbose>0) printf("Snapshot-entry #%d: <%s> %g\n",k,params[k].string.pointer,params[k].value);
  }
  return(params);
}
static void free_snapshot(int n,PARMBUF *parms) {
  int k;
  if(n>0) {
    for(k=0;k<n;k++) free(parms[k].string.pointer);
  }
  free(parms);
}

void update_topic_message(int sub, const char *topicname, STRING message) {
  if(sub<0) return;  /* Ignore it. */
  int i,j;
  if(verbose>0) printf("update_topic_message: %d:%s: <%s>\n",sub,topicname,message.pointer);

  if(num_rules>0) {
    for(i=0;i<num_rules;i++) {
      if(rules[i].numparms>0 && rules[i].params) {
        for(j=0;j<rules[i].numparms;j++) {
	  if(verbose>1) printf("check rule #%d Parm %d: %s\n",i,j,rules[i].params[j].topic);
	  if(rules[i].params[j].sub==sub) {
	    if(verbose>0) printf("Have a topic match on <%s>\n",subscriptions[rules[i].params[j].sub].topic);
	    if((rules[i].params[j].type&PARM_TRIGGER)==PARM_TRIGGER) {
	      if(verbose>0) printf("have trigger match.\n");
	      break;
	    }
	  }
	}
	if(j<rules[i].numparms) {
	  if(verbose>0) printf("trigger: Rule function shall be called.\n");
	  /* generate snapshot */
	  PARMBUF *params=make_snapshot(&rules[i]);
	  
	  /* call rule function */
	  if(rules[i].rule) (rules[i].rule)(j,params);
	  
	  /* publish results */
	  publish_results(&rules[i],params);
	  free_snapshot(rules[i].numparms,params);
	}
      }
    }
  }
}


void default_measure_loop(PARMBUF *dummy) {
  static int cnt=0;
  char buffer[64];
  STRING a;
  a.pointer=buffer;

  /* publisg ACTivity_dm */
  snprintf(buffer,sizeof(buffer),"%d",cnt);
  a.len=strlen(a.pointer);
  char *topic=make_topic(ACTIVITY_TOPIC);
  mqtt_publish(topic,a,0,1);
  free(topic);
  cnt++;
  if(cnt>=4) cnt=0;
  sleep(ACTIVITY_INTERVAL); /* It is important to yeald here! */
}




static void usage() {
  printf(
    "\nUsage: %s [-hvq] ---\t%s ruel engine.\n\n"
    "  -h --help\t\t---\tusage\n"
    "  --persist\t\t---\tbe persistent even when connection is impossible\t\n"
    "  --prefix <prefix>\t---\tset topic prefix\t\n"
    "  -v\t\t\t---\tbe more verbose\n"
    "  -q\t\t\t---\tbe more quiet\n"
    ,CLIENT,CLIENT);
}
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"--persist"))   do_persist=1; 
    else if(!strcmp(argumente[count],"--prefix"))    topic_prefix=argumente[++count];
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
  int has_been_connected=0;
  int i,j;
  char *t;
  kommandozeile(argc, argv);    /* process command line */
  if(do_persist) has_been_connected++;
  if(verbose>0) {
    printf("We have %d rules.\n",num_rules);
    printf("Persistence mode=%d\n",do_persist);
    printf("Topic Prefix:    <%s>\n",topic_prefix);
  }
  if(num_rules>0) {
    for(i=0;i<num_rules;i++) {
      if(verbose>0) printf("Rule #%d: \n",i);
      if(rules[i].initrule) (rules[i].initrule)(argc,argv);   /* Maybe process commandline ...*/
      if(rules[i].numparms>0 && rules[i].params) {  /* COllect subscriptions*/
        for(j=0;j<rules[i].numparms;j++) {
	  if(verbose>0) printf("Rule #%d Parm %d: %s\n",i,j,rules[i].params[j].topic);
	  if(rules[i].params[j].topic) {
	    t=make_topic(rules[i].params[j].topic);
	    rules[i].params[j].sub=add_subscription(t,rules[i].params[j].qos);
            free(t);
	  }
	}
      }
    }
  }
  int rc;
  if(!measure_rule) {
    if(verbose>0) printf("Setting no/default measure rule.\n");
  } else {
    if(verbose>0) printf("We have a measure rule.\n");
  }
  while(1) {
    again:
    rc=mqtt_broker(BROKER,USER,PASSWD,CLIENT);  /* connect to mqtt broker */
    if(rc==-1) {
      printf("MQTT Error: Could not connect to the MQTT broker %s.\n",BROKER);
      if(!has_been_connected) {
        printf("Quit.\n");
        exit(EX_UNAVAILABLE);
      } else {
        printf("Try to reconnect in 1 Minute.\n");
        sleep(RECONNECTION_TIME);
	goto again;
      }
    }
    has_been_connected++;
    mqtt_subscribe_all();
    if(verbose>0) printf("INFO: engine up and running. Client Id=<%s>\n",clientID);
    /* This is the main loop. */
    while(mqtt_isconnected) {
      if(measure_rule) {
        PARMBUF *params=make_snapshot(&rules[0]);
        (measure_rule)(params);
        publish_results(&rules[0],params); /* publish results */
        free_snapshot(rules[0].numparms,params);
      } else default_measure_loop(NULL); 
    }
    mqtt_unsubscribe_all();
    printf("INFO: try to reconnect in %d secs...\n",RECONNECTION_PAUSE);
    sleep(RECONNECTION_PAUSE);
  }
  mqtt_exit();  /* Verbindung zum Broker trennen. */ 
  return(EX_OK);
}
