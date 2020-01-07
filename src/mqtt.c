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
#include "config.h"
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


void mqtt_subscribe_all() {
  if(mqtt_isconnected && anzsubscriptions>0) {
    int i;
    for(i=0;i<anzsubscriptions;i++) {
      if(subscriptions[i].anz>0) {
        mqtt_subscribe(subscriptions[i].topic,subscriptions[i].qos);
      }
    }
  }
}

void connlost(void *context, char *cause) {
  printf("ERROR: MQTT-Connection lost, cause: %s\n", cause);
  mqtt_isconnected=0;
}

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
    update_topic_message(sub,m);
  }
  else printf("ERROR: Topic %s was not subscribed!\n",topicName);
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
  snprintf(clientID,sizeof(clientID),PACKAGE_NAME "-%ld",clock()); /* Make a unique client ID */
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
