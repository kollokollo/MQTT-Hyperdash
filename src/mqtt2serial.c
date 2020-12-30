/* MQTT2SERIAL.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/*
   This is a generic application (similar to the rule engine framework) to 
   asynchronoulsy pass mqtt content to a serial line, and also accept 
   asynchronously content from that line.

   This way an arduino could be easily hooked to the serial line and pass its 
   values and accept commands from it. (See the example sketch coming with this 
   repository).


There are two options: eiter use a binary package format or 
use an ASCII representation (which would be much more easy to implement in 
the arduino firmware). However there need to be made several design choices.

The format could be:

MQTT:=, where the payload should be enclosed with "

The TOPIC would be a local topics name, and the framework can add a prefix to 
it to make it available on the mqtt broker. However, this way no binary data 
could be transfered. Here possibly a base64 encoding could be used.

The framework (a simple excecutable running on the (linux/Raspberry), made out 
of the rule engines) could take as commandline parameters:

    serial device
    baud rate
    device settings (like 8:N:1)
    the MQTT prefix for local topics
    behaviour on connection loss (serial device)

Somewhere the list of MQTT topics to listen for need to be defined, maybe as a 
wildcard pattern.

TODO: implement a list of different prefixes (to be added with wildcards...) or 
a list with individual topics.
TODO: option to set the baud rate


Run this e.g. with: 

./mqtt2serial --broker tcp://localhost:1883 --prefix "ARDUINO"

and watch the topics with 

mosquitto_sub -h localhost -t "ARDUINO/#" -v

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined WINDOWS
  #define EX_OK		  0	/* successful termination */
  #define EX_UNAVAILABLE 69	/* service unavailable */
#else
  #include <sysexits.h>
#endif
#ifndef WINDOWS
#include <sys/ioctl.h>
#include <fnmatch.h>
#else
#define mkdir(a,b) mkdir(a)
#define O_NOCTTY 0
#define O_SYNC 0
#define B115200 0
#endif
#include <dirent.h>
#include <errno.h>
#ifndef WINDOWS
#include <termios.h>
#include <fnmatch.h>
#include <poll.h>
#endif
#include <sys/time.h>
#include <time.h>

#include <MQTTClient.h>
#include "file.h"

#include "subscribe.h"
#include "mqtt.h"
#include "util.h"

#define CLIENT "mqtt2serial"

#define RECONNECTION_PAUSE 2  /*  2 Seconds */
#define RECONNECTION_TIME 60  /* 60 Seconds */

/* function prototypes */

int prepare_device();
int open_device();
int device_loop();
double v_timer();

/* Configuration variables */

char *broker_url=DEFAULT_BROKER;
char *broker_user=NULL;
char *broker_passwd=NULL;
char *topic_pattern="#";

char device[128]="/dev/ttyUSB0";

int device_fd=-1;   /* serial device file descriptor (or -1 if not yet opened) */
int device_init_success=0;

extern char clientID[];
int verbose=0;    /* Verbosity level */
int do_persist=0; /* Shall the engine persist to connect ad infinitum? */
int do_json=1;    /* Expand JSON properties by default */

char *topic_prefix="SERIAL"; /* Can be configured */

/* Callback from the mqtt hyperdash rule engine framework. sub=-1 means
  that the topic was not in the subscription list (can happen when wildcards are used) */

void update_topic_message(int sub, const char *topicname, STRING message) {
  char buf[128];
  
  /* Remove the prefix from the topic name and pass the rest to 
     the serial line. 
     TODO: encode binary data to base64.
     */
  char *p;
  if(*topic_prefix) {
    int l=strlen(topic_prefix);
    if(!strncmp(topicname,topic_prefix,l)) {
      p=(char *)topicname+l+1;
    } else {
      if(verbose>=0) printf("WARNING: received unmatched topic <%s>\n",topicname);
      return;
    }
  } else p=(char *)topicname;
     
  sprintf(buf,"MQTT:%s=\"%s\"\n",p,message.pointer);
  if(device_fd!=-1) {
    write(device_fd,buf,strlen(buf));
    fsync(device_fd);
  }
  if(verbose>0) printf("%s",buf);
}



static void usage() {
  printf(
    "\nUsage: %s [-hvq] ---\tbridge to serial devices.\n\n"
    "  -h --help\t\t---\tusage\n"
    "  --broker  <url>\t---\tdefine the broker url used [%s]\n"
    "  --persist\t\t---\tbe persistent even when connection is impossible\t\n"
    "  --prefix <prefix>\t---\tset topic prefix\t\n"
    "  --pattern <pat>\t---\tset topic pattern [%s]\n"
    "  --json\t\t---\texpand JSON properties. (default)\n"
    "  --nojson\t\t---\tdo not expand JSON properties.\n"
    "  --device <devicename>\t---\tSet serial devicename [%s]\n"
    "  -v\t\t\t---\tbe more verbose\n"
    "  -q\t\t\t---\tbe more quiet\n"
    ,CLIENT,broker_url,topic_pattern,device);
}

  /* process command line parameters */

static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"--broker"))   broker_url=argumente[++count];
    else if(!strcmp(argumente[count],"--user"))     broker_user=argumente[++count];
    else if(!strcmp(argumente[count],"--passwd"))   broker_passwd=argumente[++count];
    else if(!strcmp(argumente[count],"--pattern"))  topic_pattern=argumente[++count];
    else if(!strcmp(argumente[count],"--persist"))  do_persist=1; 
    else if(!strcmp(argumente[count],"--prefix"))   topic_prefix=argumente[++count];
    else if(!strcmp(argumente[count],"--nojson"))   do_json=0;
    else if(!strcmp(argumente[count],"--json"))     do_json=1;
    else if(!strcmp(argumente[count],"--device"))   strncpy(device,argumente[++count],sizeof(device));
    else if(!strcmp(argumente[count],"-v"))	    verbose++;
    else if(!strcmp(argumente[count],"-q"))	    verbose--;
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the rule itself */
    else {
      /* do nothing, these could be options for rule itself */
    }
  }
  if(quitflag) exit(EX_OK);
}

/* Set up the (usb) serial device */

int device_setup() {
  /* Try to find the device */
  if(prepare_device()) return(-1);    /*If device not found*/
  if(open_device())   {
    perror("ERROR: could not open the USB-Device. No connection.");
    return(-8);
  }
  return(0);  
}


static char *make_topic(char *name) {
  static char topic[256]; 
  snprintf(topic,sizeof(topic),"%s/%s",topic_prefix,name);
  return(topic);
}

#define PUBLISH_ANY(a,b,c)  snprintf(buf,sizeof(buf),c,a); \
  message_string.pointer=buf; \
  message_string.len=strlen(buf); \
  mqtt_publish(make_topic(b),message_string,0,1)

static int mqtt_publish_status(int status, char *status_s) {
  char buf[256];
  STRING message_string;

  PUBLISH_ANY(status,     "STATUS_DM","%d");
  PUBLISH_ANY(status_s,   "STATUS_SM","%s");
  return(0);
}

 /* Prepare the USB device, if everythin is OK, return 0, else return !0*/

int prepare_device() {
  int try=0;
  while(!exist(device) && try++<5) { 
    printf("WARNING: device %s existiert nicht!\n",device);
    if(*device) {
      device[strlen(device)-1]++;
      printf("Trying %s instead.\n",device);
    }
  }
  if(!exist(device)) {
    device[strlen(device)-1]='0';
    perror("ERROR: could not use any device. Giving up. Abort.\n");
    return(-1);
  }
  return 0;
}


/* Handling the COM Port in WINDOWS needs to be implemented here....*/
#ifdef WINDOWS
int set_interface_attribs (int fd, int speed, int parity) {return(0);}
void set_blocking (int fd, int should_block) {}
#else
int set_interface_attribs (int fd, int speed, int parity) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if(tcgetattr(fd, &tty) != 0) {perror("error from tcgetattr");return -1;}

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;	  // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
  tty.c_iflag &= ~IGNBRK;	  // disable break processing
  tty.c_lflag = 0;		  // no signaling chars, no echo,
  				  // no canonical processing
  tty.c_oflag = 0;		  // no remapping, no delays
  tty.c_cc[VMIN]  = 0;  	  // read doesn't block
  tty.c_cc[VTIME] = 5;  	  // 0.5 seconds read timeout
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
  tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  				  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);	  // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if(tcsetattr (fd, TCSANOW, &tty) != 0) {perror("error  from tcsetattr");return -1;}
  return 0;
}

void set_blocking(int fd, int should_block) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if(tcgetattr (fd, &tty) != 0) {perror("error  from tggetattr");return;}
  tty.c_cc[VMIN]=should_block?1:0;
  tty.c_cc[VTIME]=5;  	  // 0.5 seconds read timeout
  if(tcsetattr(fd,TCSANOW,&tty)!=0)  perror("error %d setting term attributes");
}
#endif




/* open the serial device, if everything is OK, return 0, else return !0
 *
 */

int open_device() {
  if(verbose>=0) {printf("<-- %s [",device);fflush(stdout);}
  device_fd=open(device, O_RDWR | O_NOCTTY | O_SYNC);
  if(device_fd<0) {perror("opening device"); return(-1);}
  set_interface_attribs(device_fd, 9600, 0);// set speed to 115,200 bps, 8n1 (no parity)
  set_blocking(device_fd,1);

  sleep(1); /* I am not sure, if this is necessary....*/
#ifndef WINDOWS
  int anz;
  ioctl(device_fd, FIONREAD, &anz);
  if(anz>0) {
    char buf[1024];
    if(verbose>=0) printf("%d can be read. --> ",anz);
    if(read (device_fd, buf, sizeof buf)<0) perror("read");
    if(verbose>=0) printf("buffer clear.");
  }
#endif
  if(verbose>=0) printf("]\n");
  return 0;
}

/* This procedure is called whenever a complete line has been received from 
   the device.
 */

int processinput(char *line) {
  char buf[256];
  STRING message_string;
  double timestamp=v_timer();
  int l=strlen(line);
  if(l<1) return(0);
  if(line[l-1]=='\r') {line[l-1]=0;l--;}
  if(!strncmp(line,"MQTT:",5)) {
    char *p=line+5;
    while (*p==' ') p++;
    char *topic=p;
    while (*p!='=') p++;
    *p++=0;
    l=strlen(p);
    if(*p=='\"' && p[l-1]=='\"') {
      p[l-1]=0;
      p++;
    }
    if(verbose>0) printf("--> %.13g %s=<%s>\n",timestamp,topic,p); 
    PUBLISH_ANY(p,topic,"%s");
  } else {
    if(verbose>=0) printf("unrecognized output <%s>\n",line);
  }
  return(0);
}


/* The data taking loop. 
   This function is called inside an (endless) loop. 
   This could be outsourced to an independant thread.
   if the return value is <0, a read error occured.
 */

unsigned int today;
/* return a 64 bit UNIX timestamp (in double)*/
double v_timer() {
  struct timeval t;
  struct timezone tz;
  gettimeofday(&t,&tz);
  return((double)t.tv_sec+(double)t.tv_usec/1000000);
}
int checkstate(int fd) {
  int ret=0;
#if DEBUG
  pollfd a;
  a.fd=fd;
  a.events=POLLIN|POLLERR|POLLHUP;
  ret=poll(&a, 1, 100);
  printf(">>>>>> G %d %d\n",ret,a.revents);
#endif
  return(ret);
}

/* This is the main routine for handling device input. 
   It sould be called inside a loop. The procedure returns
   latest at the end of the day or when an error occurs.
   */

int device_loop() {   
  static char buf[512];
  static int anzbytes=0;
  int ret=0;

    /* Calculate the day number out of UNIX Timestamp. */
  today=(unsigned int)(v_timer()/24/3600);
      
  unsigned int t=today;
  int n,j;
  while(t==today) {
    
    /* Here we want to monitor the state of the input device, 
     * if it is disconnected or so....
     */
    n=checkstate(device_fd);
    if(n<0) { /* This means fatal state, we should close it.*/
      perror("ERROR: Connection to device lost.");
      ret=-1;
      sleep(1);
      break;
    }
    
    n=read(device_fd,&buf[anzbytes],sizeof buf-anzbytes); /* blocking read */
    /*Check for error*/
    if(n<0) {
      perror("reading from device."); 
      if(errno==EIO) {
        perror("ERROR: reading from device. Connection lost.");
        printf("Probably the device was hotunplugged/disconnected.\n");
	/* The best we can do here is to close the device and try to 
	   reopen it after a while (in the server) or to just 
	   exit the program. */
	ret=-1;
      }
      sleep(1);
      break;
    } else if(n==0) {
      /* This should mean:
      If no process has the pipe open for writing, read() shall return 0 to 
      indicate end-of-file. The same if the USB-Device is unplugged.
      It only works if the device was opened READ-ONLY !
      */
      perror("ERROR: reading nothing from device. Connection lost.");
      ret=-1;
      sleep(1);
      break;
    }
    anzbytes+=n;
    buf[anzbytes]=0;
    #if DEBUG
      printf("read %d bytes. buf=<%s>",n,buf);
    #endif
    j=0;
    while(j<anzbytes) {
      if(buf[j]=='\n') {
	buf[j++]=0;
	processinput(buf);
	anzbytes-=j;
	memmove(buf,&buf[j],anzbytes);
	j=0;
      } else j++;
    }
    t=(int)(v_timer()/24/3600);
  }
  return(ret);
}

/* Close the device connection and do cleanup. */

void device_close() {
  if(device_fd!=-1) close(device_fd);
}

int main(int argc, char* argv[]) {
  int has_been_connected=0;
  int err=0;

  kommandozeile(argc, argv);    /* process command line */
  if(do_persist) has_been_connected++;
   if(verbose>0) {
    printf("Persistence mode=%d\n",do_persist);
    printf("Topic Prefix:    <%s>\n",topic_prefix);
    printf("Device : <%s>\n",device);
  }
  char buf[256];
  if(*topic_prefix) sprintf(buf,"%s/%s",topic_prefix,topic_pattern);
  else sprintf(buf,"%s",topic_pattern); 
  add_subscription(buf,0);
  int rc;

  while(1) {
    again:
    rc=mqtt_broker(broker_url,broker_user,broker_passwd,CLIENT);  /* connect to mqtt broker */
    if(rc==-1) {
      printf("MQTT Error: Could not connect to the MQTT broker %s.\n",broker_url);
      if(!has_been_connected) {
        printf("Quit.\n");
        exit(EX_UNAVAILABLE);
      } else {
        printf("Try to reconnect in 1 Minute.\n");
        sleep(RECONNECTION_TIME);
	goto again;
      }
    }
    has_been_connected++;
    mqtt_subscribe_all();
    if(verbose>0) printf("INFO: MQTT engine up and running. Client Id=<%s>\n",clientID);
    /* This is the main loop. */
    while(mqtt_isconnected) {
      if(device_setup()) {
        device_init_success=0;
	perror("Device Setup failed.");
	mqtt_publish_status(EX_UNAVAILABLE,"Serial Device Setup failed.");
      } else {
        device_init_success=1;
	if(verbose>=0) printf("Device Setup complete.\n");
	mqtt_publish_status(0,"Serial Device up and running.");
	while((err=device_loop())==0) ;  /* Endless loop until error occurs ....*/
        mqtt_publish_status(1,"Device stopped.");
        if(verbose>=0) printf("Device abort. Closing...\n");
        device_close();
      }
      if(!do_persist) break;
      if(verbose>=0) printf("Device waiting 15min to reconnect...\n");
      sleep(1000);  /* wait 15 min before try again to reconnect to the device. */
    }
    mqtt_unsubscribe_all();
    printf("INFO: try to reconnect in %d secs...\n",RECONNECTION_PAUSE);
    sleep(RECONNECTION_PAUSE);
  }
  mqtt_exit();  /* Verbindung zum Broker trennen. */ 
  if(!device_init_success) return(EX_UNAVAILABLE);
  if(err) return(EX_IOERR);
  return(EX_OK);
}
