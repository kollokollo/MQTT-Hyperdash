/* FILE.H (c) Markus Hoffmann  */
/* Erweiterungen fuer die Datei Ein- und Ausgabe ....   */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


size_t lof(FILE *);
int myeof( FILE *);

int bsave(const char *, char *, size_t);
size_t bload(const char *, char *, size_t);

int exist(const char *);
#ifndef __HYPERDASH
typedef struct {
  unsigned int len;
  char *pointer;
} STRING;
#endif
STRING get_file(const char *name);

