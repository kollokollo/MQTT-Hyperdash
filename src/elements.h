/* ELEMENTS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
 
 
 
 
/* Here the definitions of the individual element functions: */

void i_broker(ELEMENT *el,char *pars);

void i_panel(ELEMENT *el,char *pars);
void d_panel(ELEMENT *el,WINDOW *win);
int c_panel(ELEMENT *el,WINDOW *win,int x, int y, int b);

void i_bar(ELEMENT *el,char *pars);

void i_bitmap(ELEMENT *el,char *pars);
void d_bitmap(ELEMENT *el,WINDOW *win);

void i_bitmaplabel(ELEMENT *el,char *pars);
void d_bitmaplabel(ELEMENT *el,WINDOW *win);
void u_bitmaplabel(ELEMENT *el,WINDOW *win, char *message);

void i_icon(ELEMENT *el,char *pars);
void d_icon(ELEMENT *el,WINDOW *win);

void i_box(ELEMENT *el,char *pars);
void d_box(ELEMENT *el,WINDOW *win);

void d_circle(ELEMENT *el,WINDOW *win);

void i_frame(ELEMENT *el,char *pars);
void d_frame(ELEMENT *el,WINDOW *win);
int  c_frame(ELEMENT *el,WINDOW *win,int x, int y, int b);

void i_framelabel(ELEMENT *el,char *pars);
void d_framelabel(ELEMENT *el,WINDOW *win);
void u_framelabel(ELEMENT *el,WINDOW *win, char *message);

void d_hbar(ELEMENT *el,WINDOW *win);
void u_hbar(ELEMENT *el,WINDOW *win, char *message);

void d_vbar(ELEMENT *el,WINDOW *win);
void u_vbar(ELEMENT *el,WINDOW *win, char *message);

void i_icon(ELEMENT *el,char *pars);

void i_line(ELEMENT *el,char *pars);
void d_line(ELEMENT *el,WINDOW *win);


void i_pbox(ELEMENT *el,char *pars);
void d_pbox(ELEMENT *el,WINDOW *win);

void d_pcircle(ELEMENT *el,WINDOW *win);

void i_string(ELEMENT *el,char *pars);
void d_string(ELEMENT *el,WINDOW *win);

void i_textlabel(ELEMENT *el,char *pars);
void d_textlabel(ELEMENT *el,WINDOW *win);

void i_tstring(ELEMENT *el,char *pars);
void d_tstring(ELEMENT *el,WINDOW *win);
void u_tstring(ELEMENT *el,WINDOW *win,char *message);

void i_tnumber(ELEMENT *el,char *pars);
void d_tnumber(ELEMENT *el,WINDOW *win);
void u_tnumber(ELEMENT *el,WINDOW *win, char *message);

void u_textlabel(ELEMENT *el,WINDOW *win, char *message);

void i_tinarea(ELEMENT *el,char *pars);
void i_tinstring(ELEMENT *el,char *pars);
void i_tinnumber(ELEMENT *el,char *pars);
void i_meter(ELEMENT *el,char *pars);
void d_meter(ELEMENT *el,WINDOW *win);
void u_meter(ELEMENT *el,WINDOW *win, char *message);

void d_tvmeter(ELEMENT *el,WINDOW *win);
void u_tvmeter(ELEMENT *el,WINDOW *win, char *message);
void d_thmeter(ELEMENT *el,WINDOW *win);
void u_thmeter(ELEMENT *el,WINDOW *win, char *message);

void i_shellcmd(ELEMENT *el,char *pars);
int  c_shellcmd(ELEMENT *el,WINDOW *win,int x, int y, int b);

void i_subdash(ELEMENT *el,char *pars);
int  c_subdash(ELEMENT *el,WINDOW *win,int x, int y, int b);

void i_tticker(ELEMENT *el,char *pars);
int  c_tticker(ELEMENT *el,WINDOW *win,int x, int y, int b);
void d_subscribe(ELEMENT *el,WINDOW *win);
void i_scaler(ELEMENT *el,char *pars);
void d_hscaler(ELEMENT *el,WINDOW *win);
void d_vscaler(ELEMENT *el,WINDOW *win);
void u_hscaler(ELEMENT *el,WINDOW *win, char *message);
void u_vscaler(ELEMENT *el,WINDOW *win, char *message);
int  c_hscaler(ELEMENT *el,WINDOW *win,int x, int y, int b);
int  c_vscaler(ELEMENT *el,WINDOW *win,int x, int y, int b);
int  c_tinnumber(ELEMENT *el,WINDOW *win,int x, int y, int b);
int c_tinarea(ELEMENT *el,WINDOW *win,int x, int y, int b);
int c_tinnumber(ELEMENT *el,WINDOW *win,int x, int y, int b);
int c_tinstring(ELEMENT *el,WINDOW *win,int x, int y, int b);
void i_plot(ELEMENT *el,char *pars);
void d_plot(ELEMENT *el,WINDOW *win);
void u_plot(ELEMENT *el,WINDOW *win, char *message);

