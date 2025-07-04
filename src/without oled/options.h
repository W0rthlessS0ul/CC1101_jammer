#ifndef OPTIONS_H
#define OPTIONS_H

#include "rec_play.h"
#include "jam.h"

extern float keyfob_list[];
extern const char *ssid;
extern const char *password;
extern const int display_lines;
extern float rec_frequency;

extern byte sck;
extern byte miso;
extern byte mosi;
extern byte ss;
extern int gdo0;
extern int gdo2;
extern size_t EEPROM_SIZE;
extern const int RECORDINGBUFFERSIZE;

extern WebServer server;
extern byte *jamdata;
extern int menu_number;
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