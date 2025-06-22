#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "WiFi.h"
#include "WebServer.h"
#include "EEPROM.h"
#include "options.h"
#include "jam.h"
#include "html.h"

void sendHtmlAndExecute(const char* htmlResponse, void (*action)() = nullptr) {
    server.send(200, "text/html", htmlResponse);
    if (action) action();
}

void storeEEPROMAndSet(int index, int value, int& targetVar, const char* htmlResponse) {
    EEPROM.write(index, value);
    EEPROM.commit();
    targetVar = value;
    sendHtmlAndExecute(htmlResponse);
}

void PayloadHandler(){
    int temp_payload = server.arg("Value").toInt();
    payload = (byte) temp_payload;
    EEPROM.write(4, temp_payload);
    EEPROM.commit();
    sendHtmlAndExecute(html);
}

void StepHandler(){
    range_step = server.arg("Value").toFloat();
    EEPROM.put(3, range_step);
    EEPROM.commit();
    sendHtmlAndExecute(html);
}

void DelayHandler(){
    jam_delay = server.arg("Value").toInt();
    storeEEPROMAndSet(2, jam_delay, jam_delay, html);
}

void HopperHandler(){
    float* hopperlist;
    String temp_hopper = server.arg("Value");
    int hoppercount = 1;
    for(int i = 0; i < temp_hopper.length(); i++) {
        if(temp_hopper.charAt(i) == '|') hoppercount++;
    }
    hopperlist = new float[hoppercount];
    int lastindex = 0;
    for(int i = 0; i < hoppercount; i++) {
        int nextindex = temp_hopper.indexOf('|', lastindex);
        if(nextindex == -1) nextindex = temp_hopper.length();
        hopperlist[i] = temp_hopper.substring(lastindex, nextindex).toFloat();
        lastindex = nextindex + 1;
    }
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    hopper_jam(hopperlist, hoppercount, payload);
}

void StopHandler(){
    jam_break = true;
    sendHtmlAndExecute(html);
}

void SpotFrequencyHandler() {
    float frequency = server.arg("Value").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    jam(frequency, payload);
}

void KeyFobHandler() {
    float frequency = server.arg("frequency").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    jam(frequency, payload);
}

void RangeHandler() {
    float Start = server.arg("Start").toFloat();
    float Stop = server.arg("Stop").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    range_jam(Start, Stop, range_step, payload);
}

void registerRoute(const char* path, void (*handler)()) {
    server.on(path, handler);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  cc1101initialize();
  ELECHOUSE_cc1101.getCC1101();

  for (int i = 0; i < 5; ++i) {
      if (EEPROM.read(i) == 255) {
          if (i == 3) EEPROM.write(i, 1);
          if (i == 4) EEPROM.write(i, 60);
          else EEPROM.write(i, 0);
      }
  }

  jam_delay = EEPROM.read(2);
  EEPROM.get(3, range_step);
  payload = (byte) EEPROM.read(4);
  jamdata = new byte[payload];
  WiFi.softAP(ssid, password);

  registerRoute("/", []() { sendHtmlAndExecute(html); });

  registerRoute("/setting_jamming_time", []() { sendHtmlAndExecute(html_jamming_time_setting); });
  registerRoute("/setting_range_step", []() { sendHtmlAndExecute(html_step_setting); });
  registerRoute("/setting_payload_size", []() { sendHtmlAndExecute(html_payload_settings); });

  registerRoute("/editdelay", DelayHandler);
  registerRoute("/stepinterval", StepHandler);
  registerRoute("/payload", PayloadHandler);

  registerRoute("/spot_jammer", []() { sendHtmlAndExecute(html_spot_jammer); });
  registerRoute("/keyfob_jammer", []() { sendHtmlAndExecute(html_keyfob_jammer); });
  registerRoute("/range_jammer", []() { sendHtmlAndExecute(html_range_jammer); });
  registerRoute("/hopper_jammer", []() { sendHtmlAndExecute(html_hopper_jammer); });

  registerRoute("/spot_jam", SpotFrequencyHandler);
  registerRoute("/keyfob_jam", KeyFobHandler);
  registerRoute("/range_jam", RangeHandler);
  registerRoute("/hopper_jam", HopperHandler);
  registerRoute("/stop_jamming", StopHandler);
  server.begin();
}

void loop() {
  server.handleClient();
}
