/* INPUT.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
int fileselect_dialog(char *filename, const char *path, const char *mask);
int message_dialog(char *title,char *text, int anzbut);
int property_dialog(char *elline);
int colorselect_dialog(const char *title,char *gc);
int listselect_dialog(char *topic);
