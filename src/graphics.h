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

STRING pngtobmp(unsigned char *pngdata,size_t pngsize);
WINDOW *create_window(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int);
void close_window(WINDOW *win);
void set_font(char *name, WINDOW *window);
int waitmouse(WINDOW *window);
int mouseevent(WINDOW *window, int *x, int *y, int *b, int *s);
void put_bitmap(WINDOW *window, const char *adr,int x,int y,unsigned int w, unsigned int h, unsigned long int fgc);
