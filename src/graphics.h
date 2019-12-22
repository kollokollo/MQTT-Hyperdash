/* GRAPHICS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

typedef struct {
  int x,y;
  unsigned int w,h,b,d;
  unsigned int fcolor,bcolor;
  char *title;
  char *info;
  unsigned long flags;
  unsigned short chw,chh,baseline;
  SDL_Surface *display;

} WINDOW;

WINDOW *create_window();
void close_window(WINDOW *win);
void set_font(char *name, WINDOW *window);
