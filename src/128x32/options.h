#ifndef OPTIONS_H
#define OPTIONS_H

#include "rec_play.h"
#include "jam.h"
#include "bitmap.h"

extern float keyfob_list[];
extern const char *ssid;
extern const char *password;
extern const int display_lines;
extern float rec_frequency;

extern GButton buttNEXT;
extern GButton buttOK;
extern GButton buttPREVIOUS;
extern byte sck;
extern byte miso;
extern byte mosi;
extern byte ss;
extern int gdo0;
extern int gdo2;
extern uint8_t SCREEN_WIDTH;
extern uint8_t SCREEN_HEIGHT;
extern size_t EEPROM_SIZE;
extern const int RECORDINGBUFFERSIZE;

extern int8_t OLED_RESET;
extern uint8_t SCREEN_ADDRESS;
extern Adafruit_SSD1306 display;
extern WebServer server;
extern byte *jamdata;
extern int menu_number;
extern int logo;
extern int access_point;
extern int jam_delay;
extern float range_step;
extern int rec_sampling;
extern bool jam_break;
extern int recordedLength;
extern int main_menu_number;
extern int sub_menu_number;
extern volatile int currentOperation;
extern volatile bool force_stop;
extern volatile int operationProgress;
extern volatile int operationCurrent;
extern volatile int operationTotal;
extern byte bigrecordingbuffer[];
extern byte payload;

#endif