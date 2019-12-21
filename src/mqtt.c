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
#include "hyperdash.h"
#include "mqtt.h"


char clientID[64];
#define TIMEOUT     10000L
MQTTClient client;
int mqtt_isconnected=0;
volatile MQTTClient_deliveryToken deliveredtoken;
/* This is a callback function. 
   The client application must provide an implementation of this function
to enable asynchronous notification of the loss of connection to the server.
It is
called by the client library if the client loses its connection to the server. The client
application must take appropriate action, such as trying to reconnect or reporting the problem. This
function is executed on a separate thread to the one on which the client application is running.
*/

void connlost(void *context, char *cause) {
  printf("\nMQTT-Connection lost\n");
  printf("     cause: %s\n", cause);
  
  /* TODO: sleep a while and then try to reconnect....*/
  
  
}


/* This callback is called in a separate thread, when a message for a
   subscribed topic is received.
 */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {

  printf("Message arrived for <%s>:<%s>\n",topicName,message->payload);

  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;  /* Message successfully consumed. */
}



void delivered(void *context, MQTTClient_deliveryToken dt) {
 //   printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
void mqtt_subscribe(char *topic,int qos) {
  MQTTClient_subscribe(client, topic, qos);
}
static void mqtt_unsubscribe(char *topic) {
  MQTTClient_unsubscribe(client, topic);
}

void mqtt_init() {
  atexit(mqtt_exit);
}

void mqtt_broker(char *url,char *user, char *passwd) {
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
  }


}

void mqtt_exit() {
  /* free the subscription list */
//  while(anzsubscription>0) {
//    anzsubscription--;
//    free(subscriptions[anzsubscription].topic);
//  }

//  if(mqtt_isconnected) {
//    MQTTClient_disconnect(client, 10000);
//    MQTTClient_destroy(&client);
//    mqtt_isconnected=0;
//  }
}
