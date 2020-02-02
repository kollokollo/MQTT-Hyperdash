/* SUBSCRIBE.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "basics.h"
#include "subscribe.h"

SUBSCRIPTION subscriptions[256*2];
int anzsubscriptions;
int find_subscription(const char *topic) {
  int ret=-1;
  int i;
  if(anzsubscriptions>0) {
    for(i=0;i<anzsubscriptions;i++) {
      if(!strcmp(topic,subscriptions[i].topic)) return(i); 
    }
  }
  return(ret);
}
void clear_subscription(int idx) {
  if(idx>=0 && idx<anzsubscriptions) {
    subscriptions[idx].anz=0;
    free(subscriptions[idx].topic);
    subscriptions[idx].topic=NULL;
    subscriptions[idx].qos=0;
    free(subscriptions[idx].last_value.pointer);
    subscriptions[idx].last_value.len=0;
    subscriptions[idx].last_value.pointer=NULL;
    if(idx==anzsubscriptions-1) anzsubscriptions--;
  }
  while(anzsubscriptions>0 && subscriptions[anzsubscriptions-1].anz==0) {
     anzsubscriptions--;
  }
}
void clear_all_subscriptions() {
  int i;
  if(anzsubscriptions>0) {
    for(i=0;i<anzsubscriptions;i++) {
      subscriptions[i].anz=0;
      free(subscriptions[i].topic);
      subscriptions[i].topic=NULL;
      subscriptions[i].qos=0;
      free(subscriptions[i].last_value.pointer);
      subscriptions[i].last_value.len=0;
      subscriptions[i].last_value.pointer=NULL;  
    }
  }
  anzsubscriptions=0;
}
int add_subscription(const char *topic, int qos) {
  int i=find_subscription(topic);
  if(i>=0) subscriptions[i].anz++;
  else {
    i=anzsubscriptions;
    if(anzsubscriptions<sizeof(subscriptions)/sizeof(SUBSCRIPTION)-1) {
      anzsubscriptions++;
      subscriptions[i].anz=1;
      subscriptions[i].topic=strdup(topic);
    } else {
      printf("Error: cannot subscribe to more than %ld topics.\n",sizeof(subscriptions)/sizeof(SUBSCRIPTION));
    }
  }
  /* If new quality of service is higher, take this.*/
  if(qos>subscriptions[i].qos) subscriptions[i].qos=qos;
  return(i);
}
