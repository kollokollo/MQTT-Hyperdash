/* MENU.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

typedef struct {
  int typ;
  char *text;
  void (*function)();
  GtkWidget *widget;
} MENUENTRY;

#define MENU_TITLE 1
#define MENU_ENTRY 0

#define DEFAULT_ELEMENT 3   /* COMPOUND */

extern GdkCursor *cursor;


GtkWidget *init_menu();
void menu_set_default();
