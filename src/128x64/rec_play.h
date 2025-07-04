#ifndef REC_PLAY_H
#define REC_PLAY_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "GyverButton.h"
#include "WebServer.h"
#include "options.h"

void record(float frequency, int sampling);
void play(float frequency, int sampling);

#endif