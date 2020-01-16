/* HDDASHGEN.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <fnmatch.h>
#if defined WINDOWS || defined ANDROID
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif

#include "hddashgen.h"

#define DEFAULT_BROKER "tcp://localhost:1883"

int verbose=0;    /* Verbosity level */

static void intro() {
  puts("hddashgen V.1.02 (c) 2020 by Markus Hoffmann\n"
         "This tool is part of MQTT-Hyperdash, the universal MQTT Dashboard for linux.");
}
static void usage() {
  printf(
    "\nUsage: %s [-hvq] ---\tcreate dashboards from topic lists.\n\n"
    "  -h --help\t\t---\tusage\n"
    "  -v\t\t\t---\tbe more verbose\n"
    "  -q\t\t\t---\tbe more quiet\n"
    ,"hddashgen");
}
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"--version"))  {
      intro();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"-v"))	     verbose++;
    else if(!strcmp(argumente[count],"-q"))	     verbose--;
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the rule itself */
    else {
      /* do nothing, these could be options for rule itself */
    }
  }
  if(quitflag) exit(EX_OK);
}

typedef struct {
  char *name;
  char *typ;
  int anz;
  char *inhalt;
} TOPIC;

TOPIC topics[1000];
int anztopics=0;

#define advance() y+=30; \
	    if(y>560) {y=70;x+=250;} \
	    if(h<y+30) h=y+30; \
	    if(w<x+250) w=x+250


int do_level(int level,char *match, char *filename) {
  int i,offset=0;
  int x=5,y;
  char buffer[128];
  char old[128];
  char *p1,*p2;
  int sublevel;
  int w=200;
  int h=100;
  *old=0;
  printf("--> %s [%s]\n",filename,match);
  /* Open/create file */
  FILE *fp=fopen(filename,"w");
  fprintf(fp,"PANEL: TITLE=\"%s\" W=%d H=%d FGC=$ffffffff BGC=$000040ff\n##############################\n",filename,w,h);
  fprintf(fp,"# generated by hddasghen V.1.0\n");
  fprintf(fp,"BROKER: URL=\"%s\"\n",DEFAULT_BROKER);
  if(match) {
    fprintf(fp,"TEXT: X=10 Y=30 FGC=$ffff00FF h=40 TEXT=\"%s\" FONT=\"Arial_Bold\" FONTSIZE=20\n",match);
    if(w<10+strlen(match)*14) w=10+strlen(match)*14;
  } else fprintf(fp,"TEXT: X=10 Y=30 FGC=$ffffffFF h=40 TEXT=\"%s\" FONT=\"Arial_Bold\" FONTSIZE=20\n","Root of Broker");
  
  y=70;
  
  if(match) offset=strlen(match);
  for(i=0;i<anztopics;i++) {
    if(match==NULL || !strncmp(topics[i].name,match,strlen(match))) {
      p1=&(topics[i].name[offset]);
    //  printf("Match: <%s>\n",p1);
      p2=buffer;
      sublevel=0;
      while(*p1 && *p1!='/') *p2++=*p1++; 
      if(*p1=='/') sublevel=1;
      *p2=0;
      if(strncmp(old,buffer,128)) {

	if(sublevel) {
	  /* Make a Button to follow the tree */
	  /* Process new branch*/
	  char newmatch[128];
	  if(match) snprintf(newmatch,sizeof(newmatch),"%s%s/",match,buffer);
	  else snprintf(newmatch,sizeof(newmatch),"%s/",buffer);
	  char nfilename[128];
	  p1=nfilename;
	  p2=newmatch;
	  while(*p2) {
	    if(*p2=='/') *p1='_';
	    else *p1=tolower(*p2);
	    p1++;
	    p2++;
	  }
	  *p1=0;
	  strcat(nfilename,"hd");
	  char text[256];
	  p1=text;
	  p2=newmatch;
	  while(*p2) {
	    if(*p2=='/' && p2[1]) p1=text;
	    else *p1++=*p2;
	    p2++;
	  }
	  if(p1>text) --p1;
	  *p1=0;
	  
	  
	  fprintf(fp,"PBOX: X=%d Y=%d W=%d H=%d FGC=$808080ff BGC=$808080ff\n",x+5,y,180,20);
	  fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Arial\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,text);
	  fprintf(fp,"FRAMETOGGLE: X=%d Y=%d W=%d H=%d\n",x+5,y,180,20);
	  fprintf(fp,"DASH: X=%d Y=%d W=%d H=%d DASH=\"%s\"\n",x+5,y,180,20,nfilename);

	  strcat(nfilename,".dash");
	  do_level(level+1,newmatch,nfilename);
	  printf("}\n");

          advance();
	} else {
	  if(!strcmp(buffer,"ACTIVITY_DM")) {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
            fprintf(fp,"BITMAPLABEL: x=%d y=%d BGC=$000040ff TOPIC=\"%s\" "
                       "  BITMAP[0]=\"0|Disc1|$ffffffff\" \\\n" 
                       "  BITMAP[1]=\"1|Disc2|$ffffffff\" \\\n"
                       "  BITMAP[2]=\"2|Disc3|$ffffffff\" \\\n"
                       "  BITMAP[3]=\"3|Disc4|$ffffffff\" \n",x+160,y,topics[i].name);

	    advance();
	  } else if(!strcmp(buffer,"STATUS_DM")) {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
            fprintf(fp,"BITMAPLABEL: x=%d y=%d BGC=$000040ff TOPIC=\"%s\" "
                       "  BITMAP[0]=\"0|SmallCircle|$00ff00ff\" \\\n" 
                       "  BITMAP[1]=\"1|SmallCircle|$ff0000ff\" \\\n"
                       "  BITMAP[2]=\"2|SmallCircle|$ffff00ff\" \\\n"
                       "  BITMAP[3]=\"3|SmallCircle|$ff00ffff\" \n",x+160,y,topics[i].name);

            advance();
	  } else if(!strcmp(buffer,"STATUS_SM")) {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
	    fprintf(fp,"TOPICSTRING: X=%d Y=%d W=110 H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TOPIC=\"%s\" BGC=$00ff FGC=$ffff00ff\n",x+130,y,20,topics[i].name);
	    fprintf(fp,"FRAME: X=%d Y=%d W=%d H=%d REVERT=1\n",x+130-2,y-2,110+4,20+4);
            advance();
	    
	  } else if(!fnmatch("*_AM",buffer,FNM_NOESCAPE|FNM_PATHNAME)) {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
	    fprintf(fp,"TOPICNUMBER: X=%d Y=%d W=100 H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TOPIC=\"%s\" BGC=$40ff FGC=$ffff00ff FORMAT=\"######.###\"\n",x+150,y,20,topics[i].name);
            if(w<x+250) w=x+250;
            advance();
	  } else if(!fnmatch("*_DM",buffer,FNM_NOESCAPE|FNM_PATHNAME)) {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
	    fprintf(fp,"TOPICNUMBER: X=%d Y=%d W=100 H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TOPIC=\"%s\" BGC=$40ff FGC=$ffff00ff FORMAT=\"######\"\n",x+150,y,20,topics[i].name);
            if(w<x+250) w=x+250;
            advance();
	  } else if(!fnmatch("*_SM",buffer,FNM_NOESCAPE|FNM_PATHNAME)) {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
	    fprintf(fp,"TOPICSTRING: X=%d Y=%d W=110 H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TOPIC=\"%s\" BGC=$00ff FGC=$ffff00ff\n",x+130,y,20,topics[i].name);
	    fprintf(fp,"FRAME: X=%d Y=%d W=%d H=%d REVERT=1\n",x+130-2,y-2,110+4,20+4);
            advance();

	  } else {
	    fprintf(fp,"TEXT: X=%d Y=%d H=%d FONT=\"Courier_New_Bold\" FONTSIZE=16 TEXT=\"%s\" FGC=$ffffffff\n",x+10,y,20,buffer);
	    if(!strcmp(topics[i].typ,"binary")) {
	      fprintf(fp,"TEXTAREA: X=%d Y=%d W=100 H=%d FONT=\"Courier_New_Bold\" FONTSIZE=10 TOPIC=\"%s\" BGC=$00ff FGC=$ff00ffff\n",x+150,y,20,topics[i].name);
	    } else {
	      fprintf(fp,"TEXTAREA: X=%d Y=%d W=100 H=%d FONT=\"Courier_New_Bold\" FONTSIZE=10 TOPIC=\"%s\" BGC=$00ff FGC=$ffffffff\n",x+150,y,20,topics[i].name);
	      fprintf(fp,"TOPICINSTRING: X=%d Y=%d W=%d H=%d TOPIC=\"%s\"\n",x+150-2,y-2,100+4,20+4,topics[i].name);
	    }
	    fprintf(fp,"FRAME: X=%d Y=%d W=%d H=%d REVERT=1\n",x+150-2,y-2,100+4,20+4);
  	    advance();
	    printf("undeal: <%s>  %s\n",buffer,topics[i].typ);
	  }
	}
      }
      strncpy(old,buffer,128);
    }
  }
  fprintf(fp,"TEXT: X=%d Y=%d FGC=$ffff00FF h=20 TEXT=\"%s\" FONT=\"Arial\" FONTSIZE=10\n",x+5,y+5,
  "generated by hddashgen V.1.0");
  
  rewind(fp);
  fprintf(fp,"PANEL: TITLE=\"%s\" W=%d H=%d FGC=$ffffffff BGC=$000040ff\n",filename,w,h);
  /* close file */
  fclose(fp);
  return(1);
}


int main(int argc, char* argv[]) {
  kommandozeile(argc, argv);    /* process command line */

  /* Read from stdin */

    char *line = NULL;
 
    char *p1,*p2;
    size_t size;
    while(getline(&line, &size, stdin) != -1) {
      p1=line;
      if(*p1==0) continue;
      while(*p1 && isspace(*p1)) p1++;
      if(*p1==0) continue;
      if(*p1=='#') continue;
      p2=p1;
      while(*p2 && !isspace(*p2)) p2++;
      *p2++=0;
      topics[anztopics].name=strdup(p1);
      p1=p2;
      while(*p1 && isspace(*p1)) p1++;

      p2=p1;
      while(*p2 && !isspace(*p2)) p2++;
      *p2++=0;
      topics[anztopics].anz=atoi(p1);

      p1=p2;
      while(*p1 && isspace(*p1)) p1++;
      p2=p1;
      while(*p2 && !isspace(*p2)) p2++;
      *p2++=0;
      topics[anztopics].typ=strdup(p1);
      
      p1=p2;
      while(*p1 && isspace(*p1)) p1++;
      p2=p1;
      while(*p2 && !isspace(*p2)) p2++;
      *p2++=0;
      topics[anztopics].inhalt=strdup(p1);
      anztopics++;
    }
    free(line);
    
    printf("Have %d topics from list.\n",anztopics);
    do_level(0,NULL,"mainroot_hd.dash");
    
    
    
  return(EX_OK);
}
