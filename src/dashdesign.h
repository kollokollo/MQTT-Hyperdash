/* DASHDESIGN.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/* Define Actions */

enum ACTIONS {
  A_NONE,
  A_MOVE,
  A_COPY,
  A_RESIZE,
  A_MTB,
  A_ADD,
  A_ADDGROUP,
  A_DELETE,
  A_EDIT,
  A_SFGC,
  A_SBGC,
  A_SFONT,
  A_STOPIC

};

extern const char *action_names[];

WINDOW *open_pixmap(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int fullscreen);
void close_pixmap(WINDOW *nw);
void init_newloaded_dash();
void emergency_save_dialog();
void update_statusline();
void update_drawarea();
void update_title(const char *t);
void redraw_panel(GtkWidget *widget);


extern char ifilename[];
extern int verbose;
extern int current_element;
extern int current_group;
extern int current_action;
unsigned long int current_fgc;
unsigned long int current_bgc;
extern char current_font[];
extern char current_topic[];
extern int is_modified;
extern int do_grid;

extern DASH *maindash;
extern WINDOW *mainwindow;
extern GdkPixmap *pixmap;
extern ELEMENT *undo_element;
extern GtkWidget *drawing_area;
extern GtkWidget *window;
