#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "GyverButton.h"
#include "WiFi.h"
#include "WebServer.h"
#include "EEPROM.h"
#include "bitmap.h"
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

void AccessPointHandler(int status) {
    storeEEPROMAndSet(1, status, access_point, html_pls_reboot);
    display.clearDisplay(); 
    display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 32, WHITE); 
    display.display();
    while(true);
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
    updateDisplay(menu_number);
}

void StopHandler(){
    jam_break = true;
    sendHtmlAndExecute(html);
    updateDisplay(menu_number);
}

void SpotFrequencyHandler() {
    float frequency = server.arg("Value").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    jam(frequency, payload);
    updateDisplay(menu_number);
}

void KeyFobHandler() {
    float frequency = server.arg("frequency").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    jam(frequency, payload);
    updateDisplay(menu_number);
}

void RangeHandler() {
    float Start = server.arg("Start").toFloat();
    float Stop = server.arg("Stop").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    range_jam(Start, Stop, range_step, payload);
    updateDisplay(menu_number);
}

void registerRoute(const char* path, void (*handler)()) {
    server.on(path, handler);
}

void spot() {
  float frequency = 300.00;
    
  auto display_info = [&](float flag) {
    display.clearDisplay();
    display.setCursor(0, 0);
    flag = (flag > 928.00) ? 928.00 : (flag < 300.00) ? 300.00 : flag;
    display.println("Frequency: " + String(flag) + " MHz");
    display.display();
  };

  display_info(frequency);
    
  while (true) {
    buttOK.tick();
    buttNEXT.tick();
    buttPREVIOUS.tick();

    auto handleButton = [&](GButton &btn, float step_single, float step_double, float step_triple, float step_hold) {
      if (btn.isSingle()) frequency += step_single;
      else if (btn.isDouble()) frequency += step_double;
      else if (btn.isTriple()) frequency += step_triple;
      else if (btn.isHold()) frequency += step_hold;
      else return;
            
      display_info(frequency);
    };

    handleButton(buttNEXT, 0.1, 0.01, 100.0, 0.1);
    handleButton(buttPREVIOUS, -0.1, -0.01, -100.0, -0.1);

    if (buttOK.isSingle()) {
      jam(frequency, payload);
      break;
    }
  }
}

void hopper() {
  float frequency = 300.00;
  float* hopperlist = nullptr;
  int hopperCount = 0;
  int hopperCapacity = 0;

  auto append = [&](float value) {
    if (hopperCount == hopperCapacity) {
      int newCapacity = hopperCapacity == 0 ? 1 : hopperCapacity * 2;
      float* newArray = (float*)malloc(newCapacity * sizeof(float));
      if (newArray == nullptr) return;
      
      for (int i = 0; i < hopperCount; i++) {
        newArray[i] = hopperlist[i];
      }
      free(hopperlist);
      hopperlist = newArray;
      hopperCapacity = newCapacity;
    }
    hopperlist[hopperCount] = value;
    hopperCount++;
  };

  auto display_info = [&](float flag, const char* text) {
    display.clearDisplay();
    display.setCursor(0, 0);
    flag = constrain(flag, 300.00, 928.00);
    display.println("Frequency: " + String(flag) + " MHz");
    display.setCursor(0, 10);
    display.println(text);
    display.display();
  };

  display_info(frequency, "");
    
  while (true) {
    buttOK.tick();
    buttNEXT.tick();
    buttPREVIOUS.tick();

    auto handleButton = [&](GButton &btn, float step_single, float step_double, float step_triple, float step_hold) {
      if (btn.isSingle()) frequency += step_single;
      else if (btn.isDouble()) frequency += step_double;
      else if (btn.isTriple()) frequency += step_triple;
      else if (btn.isHold()) frequency += step_hold;
      else return;
            
      display_info(frequency, "");
    };

    handleButton(buttNEXT, 0.1, 0.01, 100.0, 0.1);
    handleButton(buttPREVIOUS, -0.1, -0.01, -100.0, -0.1);

    if (buttOK.isSingle()) {
      append(frequency);
      display_info(frequency, "Value is saved");
      delay(500);
      display_info(frequency, "");
    }
    else if (buttOK.isHolded()) {
      if (hopperCount > 0) {
        hopper_jam(hopperlist, hopperCount, payload);
      }
      break;
    }
  }
  free(hopperlist);
}

void range() {
  float start_frequency = 300.00;
  float stop_frequency = 300.00;
  bool temp = true;
  auto display_info = [&](float start, float stop) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Start");
    display.setCursor(94, 0);
    display.println("Stop");
    display.setCursor(0, 10);
    display.println(String(start));
    display.setCursor(89, 10);
    display.println(String(stop));
    display.setCursor(0, 20);
    display.display();
  };

  display_info(start_frequency, stop_frequency+range_step);
    
  while (true) {
    buttOK.tick();
    buttNEXT.tick();
    buttPREVIOUS.tick();

    auto handleButton = [&](GButton &btn, float step_single, float step_double, float step_hold, bool temp) {
      auto check = [&](float step){
        if (start_frequency < 300.00) start_frequency = 300.00;
        if (start_frequency > 928.00-step) start_frequency = 928.00-step;
        if (stop_frequency < start_frequency+step) stop_frequency = start_frequency + step;

        display_info(start_frequency, stop_frequency);
      };
      if (temp){
        if (btn.isSingle()) start_frequency += step_single;
        else if (btn.isDouble()) start_frequency += step_double;
        else if (btn.isHold()) start_frequency += step_hold;
        else return;
      } else{
        if (btn.isSingle()) stop_frequency += step_single;
        else if (btn.isDouble()) stop_frequency += step_double;
        else if (btn.isHold()) stop_frequency += step_hold;
        else return;
      }

      check(step_single);
    };
    
    if (temp){
      handleButton(buttNEXT, range_step, 100.0, range_step, temp);
      handleButton(buttPREVIOUS, -range_step, -100.0, -range_step, temp);
    } else{
      handleButton(buttNEXT, range_step, 100.0, range_step, temp);
      handleButton(buttPREVIOUS, -range_step, -100.0, -range_step, temp);
    }

    if (buttOK.isSingle()) {
      if (temp) temp = false;
      else {
        range_jam(start_frequency, stop_frequency, range_step, payload);
        break;
      }
    }
    delay(10);
  }
}

void keyfob() {
  int keyfob_menu_number = 0;
  const int display_lines = 3;

  auto display_info = [&](int selected_idx) {
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("->" + String(keyfob_list[selected_idx], 2) + " MHz");
    int line = 1;

    for (int offset = 1; offset < display_lines; offset++) {
      int idx = selected_idx + offset;
      if (idx >= 9) break;
      display.setCursor(0, line * 10);
      display.println(" " + String(keyfob_list[idx], 2) + " MHz");
      line++;
    }
    display.display();
  };

  display_info(keyfob_menu_number);

  while (true) {
    buttOK.tick();
    buttNEXT.tick();
    buttPREVIOUS.tick();

    if (buttOK.isSingle()) {
      jam(keyfob_list[keyfob_menu_number], payload);
      break;
    }

    if (buttNEXT.isSingle()) {
      keyfob_menu_number = (keyfob_menu_number + 1) % 9;
      display_info(keyfob_menu_number);
    }

    if (buttPREVIOUS.isSingle()) {
      keyfob_menu_number = (keyfob_menu_number - 1 + 9) % 9;
      display_info(keyfob_menu_number);
    }
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  cc1101initialize();
  ELECHOUSE_cc1101.getCC1101();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  for (int i = 0; i < 5; ++i) {
      if (EEPROM.read(i) == 255) {
          if (i == 3) EEPROM.write(i, 1);
          if (i == 4) EEPROM.write(i, 60);
          else EEPROM.write(i, 0);
      }
  }

  buttOK.setClickTimeout(200);
  buttNEXT.setClickTimeout(200);
  buttPREVIOUS.setClickTimeout(200);

  logo = EEPROM.read(0);
  access_point = EEPROM.read(1);
  jam_delay = EEPROM.read(2);
  EEPROM.get(3, range_step);
  payload = (byte) EEPROM.read(4);
  jamdata = new byte[payload];
  if (access_point == 0) {
      WiFi.softAP(ssid, password);

      registerRoute("/", []() { sendHtmlAndExecute(html); });

      registerRoute("/setting_logo", []() { sendHtmlAndExecute(html_logo_setings); });
      registerRoute("/setting_access_point", []() { sendHtmlAndExecute(html_access_point_settings); });
      registerRoute("/setting_jamming_time", []() { sendHtmlAndExecute(html_jamming_time_setting); });
      registerRoute("/setting_range_step", []() { sendHtmlAndExecute(html_step_setting); });
      registerRoute("/setting_payload_size", []() { sendHtmlAndExecute(html_payload_settings); });

      registerRoute("/logo_on", []() { storeEEPROMAndSet(0, 0, logo, html); });
      registerRoute("/logo_off", []() { storeEEPROMAndSet(0, 1, logo, html); });
      registerRoute("/access_point_off", []() { AccessPointHandler(1); });
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
  
  if (logo == 0) {
      display.clearDisplay();
      display.drawBitmap(0, 0, bitmap_logo, 128, 32, WHITE);
      display.display();
      delay(2000);
  }

  updateDisplay(menu_number);
}

void loop() {
  if (access_point == 0) server.handleClient();
  buttOK.tick();
  buttNEXT.tick();
  buttPREVIOUS.tick(); 
  if (buttOK.isSingle()) {
    executeAction(menu_number);
  }

  if (buttNEXT.isSingle()) {
    int max_menu = (access_point == 0) ? 4 : 5;
    menu_number = (menu_number + 1) % max_menu;
    updateDisplay(menu_number);
  }

  if (buttPREVIOUS.isSingle()) {
    int max_menu = (access_point == 0) ? 4 : 5;
    menu_number = (menu_number - 1 + max_menu) % max_menu;
    updateDisplay(menu_number);
  }
}
void updateDisplay(int menuNum) {
  display.clearDisplay();
  const uint8_t* bitmap = (menu_number == 0) ? bitmap_keyfob_jammer : 
                          (menu_number == 1) ? bitmap_spot_jammer : 
                          (menu_number == 2) ? bitmap_range_jammer :
                          (menu_number == 3) ? bitmap_hopper_jammer : bitmap_access_point;
  display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  display.display();
}
void executeAction(int menuNum) {
  switch (menuNum) {
    case 0: keyfob(); break;
    case 1: spot(); break;
    case 2: range(); break;
    case 3: hopper(); break;
    case 4: AccessPointHandler(0); break;
  }
  updateDisplay(menuNum);
}
