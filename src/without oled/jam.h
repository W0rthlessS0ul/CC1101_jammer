#ifndef JAM_H
#define JAM_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "Arduino.h"
#include "WebServer.h"
#include "options.h"

void cc1101initialize(void);
void jam(float frequency, byte payload);
void range_jam(float start_frequency, float stop_frequency, float step, byte payload);
void hopper_jam(float *hopperlist, int count, byte payload);

#endif