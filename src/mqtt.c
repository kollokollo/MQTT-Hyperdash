/* MQTT.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <MQTTClient.h>
#include <time.h>
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "mqtt.h"


char clientID[64];
#define TIMEOUT     10000L
MQTTClient client;
volatile int mqtt_isconnected=0;
volatile MQTTClient_deliveryToken deliveredtoken;
/* This is a callback function. 
   The client application must provide an implementation of this function
to enable asynchronous notification of the loss of connection to the server.
It is
called by the client library if the client loses its connection to the server. The client
application must take appropriate action, such as trying to reconnect or reporting the problem. This
function is executed on a separate thread to the one on which the client application is running.
*/

SUBSCRIPTION subscriptions[256];
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
  for(i=0;i<anzsubscriptions;i++) {
    subscriptions[i].anz=0;
    free(subscriptions[i].topic);
    subscriptions[i].topic=NULL;
    free(subscriptions[i].last_value.pointer);
    subscriptions[i].last_value.len=0;
    subscriptions[i].last_value.pointer=NULL;  
  }
  anzsubscriptions=0;
}
int add_subscription(const char *topic) {
  int i=find_subscription(topic);
  if(i>=0) subscriptions[i].anz++;
  else {
    i=anzsubscriptions;
    anzsubscriptions++;
    subscriptions[i].anz=1;
    subscriptions[i].topic=strdup(topic);
    mqtt_subscribe(topic,0);
  }
  return(i);
}


void connlost(void *context, char *cause) {
  printf("ERROR: MQTT-Connection lost, cause: %s\n", cause);
  mqtt_isconnected=0;
}

extern void update_dash(char *topic, STRING message);
/* This callback is called in a separate thread, when a message for a
   subscribed topic is received.
 */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
  STRING m;
  m.pointer=message->payload;
  m.len=message->payloadlen;
  char buf[m.len+1];
  strncpy(buf,m.pointer,m.len);
  buf[m.len]=0;
  if(verbose>0) printf("Message arrived for <%s>:<%s>\n",topicName,buf);
  m.pointer=buf;
  
  int sub=find_subscription(topicName);
  if(sub>=0) {
    subscriptions[sub].last_value.len=message->payloadlen;
    subscriptions[sub].last_value.pointer=realloc(subscriptions[sub].last_value.pointer,message->payloadlen+1);
    memcpy(subscriptions[sub].last_value.pointer,buf,m.len+1);
  }
  else printf("ERROR: Topic was not subscribed!\n");
  update_dash(topicName,m);
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;  /* Message successfully consumed. */
}



void delivered(void *context, MQTTClient_deliveryToken dt) {
 //   printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
void mqtt_subscribe(const char *topic,int qos) {
  MQTTClient_subscribe(client, topic, qos);
}
static void mqtt_unsubscribe(char *topic) {
  MQTTClient_unsubscribe(client, topic);
}

/* Publish the content of a string (message) to a topic on 
   a (mqtt) server. This command could also work with message queues.
 */
void mqtt_publish(char *topic, STRING payload, int qos, int retain) {
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  pubmsg.payload=payload.pointer;
  pubmsg.payloadlen=payload.len;
  pubmsg.qos =qos;
  pubmsg.retained = retain;
//  printf("publish to <%s> <%s> qos=%d\n",topic,payload.pointer,qos);
  MQTTClient_publishMessage(client,topic, &pubmsg, &token);
//  printf("done token=%d\n",token);
  // int rc=
  MQTTClient_waitForCompletion(client, token, TIMEOUT);
  // printf("Message with delivery token %d delivered\n", token);
}






void mqtt_init() {
  atexit(mqtt_exit);
}

int mqtt_broker(char *url,char *user, char *passwd) {
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_willOptions will_opts;
  
  mqtt_exit(); /* Alte Verbindung beenden.*/
  sprintf(clientID,"MQTT-Hyperdash-Panelname-%ld",clock()); /* Make a unique client ID */
  MQTTClient_create(&client,url, clientID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  conn_opts.reliable=0;

  if(user) conn_opts.username=user;
  if(passwd) conn_opts.password=passwd;

  will_opts.struct_id[0]='M';
  will_opts.struct_id[1]='Q';
  will_opts.struct_id[2]='T';
  will_opts.struct_id[3]='W';
  will_opts.struct_version=0;
  will_opts.topicName=clientID;
  will_opts.message="disconnect";
  will_opts.retained=0;
  will_opts.qos=0;
  
  conn_opts.will=&will_opts;
  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
  int rc;
  if((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
    printf("MQTT Client: <%s> ",clientID);
    printf("Failed to connect, return code %d\n", rc);
    mqtt_isconnected=1;
    return(-1);
  }
  mqtt_isconnected=1;
  return(0);
}
void mqtt_unsubscribe_all() {
  if(mqtt_isconnected) {
    int i;
    for(i=0;i<anzsubscriptions;i++) {
      if(subscriptions[i].anz>0) mqtt_unsubscribe(subscriptions[i].topic);
    }
  }
  /* free the subscription list */
  clear_all_subscriptions();
}
void mqtt_disconnect() {
  if(mqtt_isconnected) {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    mqtt_isconnected=0;
  }
}


void mqtt_exit() {
  mqtt_unsubscribe_all();
  mqtt_disconnect();
}
