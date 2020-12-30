#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1


extern SSD1306AsciiWire oled;
void OLED_setup();
void show_logo();
void bargraph(byte x, byte y,byte w,int val);
void bargraph_s(byte x, byte y,byte w,int val);
void OLED_memdump(byte *p,unsigned char len);
void bat_status(unsigned char full);
void status_indicator(byte x, byte y,byte typ);
