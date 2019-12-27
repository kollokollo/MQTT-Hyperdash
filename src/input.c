/* INPUT.C (c) Markus Hoffmann  */

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
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "input.h"


/*TODO: 

The last message ssen cannot be received from the broker, 
so one has to store them on reception. This involves a more difficult handling of
incoming messages...


SAVE and RESTORE Buttons in the dialog which can save a value on a per topic basis for
later restore.


The input dialog windows are tricky. Dowe need to use a widget library?
What about portability?

*/


void input_dialog(char *topic) {
  char title[256];
  char cmd[512];
  STRING a;
  a.pointer="test";
  a.len=strlen(a.pointer);
  sprintf(title,"Input %s:",topic);
  sprintf(cmd,"yad --form --width 500 --title \"%s\" --text \"%s\" "
  "--field \"\" \"%s\"",title,topic,topic);
  system(cmd);
/*  Get the user input 
*/
/* Publish it using QOS=2*/
  mqtt_publish(topic,a,2,1);
}
