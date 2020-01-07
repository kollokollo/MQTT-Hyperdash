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

int verbose=0;

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
      mqtt_publish(subscriptions[pd->sub].topic,message,subscriptions[pd->sub].qos,1);
    }
    free(message.pointer);
  }
}


#include RULE

int num_rules=sizeof(rules)/sizeof(RULEDEF);

/* TODO: define client ID */
// char *clientID=CLIENT;

#define TIMEOUT     10000L
/*
  check if topic was a triggertopic. if so, make a snapshot of all input params
  and  call the rule procedure.

  afterwards format output params and publish
 */

void update_topic_message(int sub, STRING message) {
  int i,j,k;
  if(verbose>0) printf("update_topic_message: %d <%s>\n",sub,message.pointer);

  if(num_rules>0) {
    for(i=0;i<num_rules;i++) {
      if(rules[i].numparms>0 && rules[i].params) {
        for(j=0;j<rules[i].numparms;j++) {
	  if(verbose>0) printf("check rule #%d Parm %d: %s\n",i,j,rules[i].params[j].topic);
	  if(rules[i].params[j].sub==sub) {
	    if(verbose>0) printf("Have a topic match.\n");
	    if((rules[i].params[j].type&PARM_TRIGGER)==PARM_TRIGGER) {
	      if(verbose>0) printf("have trigger match.\n");
	      break;
	    }
	  }
	}
	if(j<rules[i].numparms) {
	  if(verbose>0) printf("trigger: Rule function shall be called.\n");
	      /* generate snapshot */
	  PARMBUF *params=calloc(rules[i].numparms,sizeof(PARMBUF));
	  for(k=0;k<rules[i].numparms;k++) {
	    params[k].string=double_string(&(subscriptions[rules[i].params[k].sub].last_value));
            if((rules[i].params[k].type&PARM_ISNUMBER)==PARM_ISNUMBER) {
	      params[k].value=myatof(params[k].string.pointer);
	    }
	    if(verbose>0) printf("Snapshot-entry #%d: <%s> %g\n",k,params[k].string.pointer,params[k].value);
	  }
	  
	  
	  /* call rule function */
	  (rules[i].rule)(j,params);
	  
	  /* publish results */
	  for(k=0;k<rules[i].numparms;k++) {
            if((rules[i].params[k].type&PARM_OUT)==PARM_OUT) publish_if_different(&(rules[i].params[k]), &params[k]);
	  }
	  for(k=0;k<rules[i].numparms;k++) free(params[k].string.pointer);
	  free(params);
	}
      }
    }
  }
}

int main(int argc, char* argv[]) {
  int cnt=0;
  int rc,i,j;
  char buffer[64];
  STRING a;
  a.pointer=buffer;
  if(verbose>0) printf("We have %d rules.\n",num_rules);
  if(num_rules>0) {
    for(i=0;i<num_rules;i++) {
      if(verbose>0) printf("Rule #%d: \n",i);
      if(rules[i].initrule) (rules[i].initrule)(argc,argv);   /* Maybe process commandline ...*/
      if(rules[i].numparms>0 && rules[i].params) {  /* COllect subscriptions*/
        for(j=0;j<rules[i].numparms;j++) {
	  if(verbose>0) printf("Rule #%d Parm %d: %s\n",i,j,rules[i].params[j].topic);
	  if(rules[i].params[j].topic) {
	    rules[i].params[j].sub=add_subscription(rules[i].params[j].topic,rules[i].params[j].qos);
	  }
	}
      }
    }
  }
  while(1) {
    int rc=mqtt_broker(BROKER,USER,PASSWD);  /* connect to mqtt broker */
    if(rc==-1) {
      printf("MQTT Error: Could not connect to the MQTT broker %s.\n",BROKER);
      exit(0);
    }
 
    mqtt_subscribe_all();
    if(verbose>0) printf("INFO: engine up and running.\n");
    while(mqtt_isconnected) {
      sleep(1);
      /* publisg ACTivity_dm */
      snprintf(buffer,sizeof(buffer),"%d",cnt);
      a.len=strlen(a.pointer);
      mqtt_publish(ACTIVITY_TOPIC,a,0,1);
      cnt++;
      if(cnt>=4) cnt=0;
    }
    mqtt_unsubscribe_all();
    printf("INFO: try to reconnect in 2 secs...\n");
    sleep(2);
  }
  mqtt_exit();  /* Verbindung zum Broker trennen. */ 
  return rc;
}
