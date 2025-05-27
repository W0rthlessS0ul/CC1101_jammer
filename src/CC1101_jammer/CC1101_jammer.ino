#include "ELECHOUSE_CC1101_SRC_DRV.h"
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
    display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 64, WHITE); 
    display.display();
    while(true);
}

void DelayHandler(){
    jam_delay = server.arg("Value").toInt();
    storeEEPROMAndSet(2, jam_delay, jam_delay, html);
}

void StopHandler(){
    jam_break = true;
    sendHtmlAndExecute(html);
    updateDisplay(menu_number);
}

void miscFrequencyHandler() {
    float frequency = server.arg("Value").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    jam(frequency);
    updateDisplay(menu_number);
}

void KeyFobHandler() {
    float frequency = server.arg("frequency").toFloat();
    String response = String(html_frequency_jam);
    response.replace("|| 0", "|| " + String(jam_delay));
    server.send(200, "text/html", response);
    jam(frequency);
    updateDisplay(menu_number);
}

void registerRoute(const char* path, void (*handler)()) {
    server.on(path, handler);
}

void misc() {
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
      jam(frequency);
      break;
    }
  }
}

void keyfob() {
  int keyfob_menu_number = 0;
  const int display_lines = 6;

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
      jam(keyfob_list[keyfob_menu_number]);
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

  for (int i = 0; i < 3; ++i) {
      if (EEPROM.read(i) == 255) {
          EEPROM.write(i, 0);
      }
  }

  logo = EEPROM.read(0);
  access_point = EEPROM.read(1);
  jam_delay = EEPROM.read(2);
  if (access_point == 0) {
      WiFi.softAP(ssid, password);

      registerRoute("/", []() { sendHtmlAndExecute(html); });

      registerRoute("/setting_logo", []() { sendHtmlAndExecute(html_logo_setings); });
      registerRoute("/setting_access_point", []() { sendHtmlAndExecute(html_access_point_settings); });
      registerRoute("/setting_jamming_time", []() { sendHtmlAndExecute(html_jamming_time_setting); });

      registerRoute("/logo_on", []() { storeEEPROMAndSet(0, 0, logo, html); });
      registerRoute("/logo_off", []() { storeEEPROMAndSet(0, 1, logo, html); });
      registerRoute("/access_point_off", []() { AccessPointHandler(1); });
      registerRoute("/editdelay", DelayHandler);

      registerRoute("/misc_jammer", []() { sendHtmlAndExecute(html_misc_jammer); });
      registerRoute("/keyfob_jammer", []() { sendHtmlAndExecute(html_keyfob_jammer); });

      registerRoute("/misc_jam", miscFrequencyHandler);
      registerRoute("/keyfob_jam", KeyFobHandler);
      registerRoute("/stop_jamming", StopHandler);
      server.begin();
  }
  
  if (logo == 0) {
      display.clearDisplay();
      display.drawBitmap(0, 0, bitmap_logo, 128, 64, WHITE);
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
    int max_menu = (access_point == 0) ? 2 : 3;
    menu_number = (menu_number + 1) % max_menu;
    updateDisplay(menu_number);
  }

  if (buttPREVIOUS.isSingle()) {
    int max_menu = (access_point == 0) ? 2 : 3;
    menu_number = (menu_number - 1 + max_menu) % max_menu;
    updateDisplay(menu_number);
  }
}
void updateDisplay(int menuNum) {
  display.clearDisplay();
  const uint8_t* bitmap = (menu_number == 0) ? bitmap_keyfob_jammer : 
                          (menu_number == 1) ? bitmap_misc_jammer : bitmap_access_point;
  display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
  display.display();
}
void executeAction(int menuNum) {
  switch (menuNum) {
    case 0: keyfob(); break;
    case 1: misc(); break;
    case 2: AccessPointHandler(0); break;
  }
  updateDisplay(menuNum);
}
