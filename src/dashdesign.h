/* DASHDESIGN.H (c) Markus Hoffmann  */

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

/* Define Actions */

enum ACTIONS {
  A_NONE,
  A_MOVE,
  A_RESIZE,
  A_MTB,
  A_ADD,
  A_DELETE

};

const char *action_names[]={
"Identify",
"Move",
"Resize",
"Move to Bkg",
"Add",
"Delete"
};


WINDOW *open_pixmap(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int fullscreen);

void close_pixmap(WINDOW *nw);
