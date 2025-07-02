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
#include "rec_play.h"

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

void RecPlayHandler() {
  sendHtmlAndExecute(html_rec_play);
}

void RecordHandler() {
  float rec_frequency = server.arg("frequency").toFloat();

  server.send(200, "text/plain", "Recording started");
  currentOperation = 1;
  record(rec_frequency, rec_sampling);
  currentOperation = 0;
}

void PlayHandler() {
  float rec_frequency = server.arg("frequency").toFloat();

  server.send(200, "text/plain", "Playback started");
  currentOperation = 2;
  play(rec_frequency, rec_sampling);
  currentOperation = 0;
}

void StopOperationHandler() {
  force_stop = true;
  server.send(200, "text/plain", "Stop requested");
}

void RecPlayStatusHandler() {
  String status = "none";
  if (currentOperation == 1) status = "recording";
  if (currentOperation == 2) status = "replaying";

  String json = "{";
  json += "\"operation\":\"" + status + "\",";
  json += "\"progress\":" + String(operationProgress) + ",";
  json += "\"current\":" + String(operationCurrent) + ",";
  json += "\"total\":" + String(operationTotal);
  json += "}";

  server.send(200, "application/json", json);
}

void AccessPointHandler(int status) {
  storeEEPROMAndSet(1, status, access_point, html_pls_reboot);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 32, WHITE);
  display.display();
  while (true)
    ;
}

void PayloadHandler() {
  int temp_payload = server.arg("Value").toInt();
  payload = (byte)temp_payload;
  EEPROM.write(4, temp_payload);
  EEPROM.commit();
  sendHtmlAndExecute(html);
}

void StepHandler() {
  range_step = server.arg("Value").toFloat();
  EEPROM.put(3, range_step);
  EEPROM.commit();
  sendHtmlAndExecute(html);
}

void SampleHandler() {
  rec_sampling = server.arg("Value").toInt();
  EEPROM.put(5, rec_sampling);
  EEPROM.commit();
  sendHtmlAndExecute(html);
}

void DelayHandler() {
  jam_delay = server.arg("Value").toInt();
  storeEEPROMAndSet(2, jam_delay, jam_delay, html);
}

void HopperHandler() {
  float* hopperlist;
  String temp_hopper = server.arg("Value");
  int hoppercount = 1;
  for (int i = 0; i < temp_hopper.length(); i++) {
    if (temp_hopper.charAt(i) == '|') hoppercount++;
  }
  hopperlist = new float[hoppercount];
  int lastindex = 0;
  for (int i = 0; i < hoppercount; i++) {
    int nextindex = temp_hopper.indexOf('|', lastindex);
    if (nextindex == -1) nextindex = temp_hopper.length();
    hopperlist[i] = temp_hopper.substring(lastindex, nextindex).toFloat();
    lastindex = nextindex + 1;
  }
  String response = String(html_frequency_jam);
  response.replace("|| 0", "|| " + String(jam_delay));
  server.send(200, "text/html", response);
  hopper_jam(hopperlist, hoppercount, payload);
  updateDisplay(menu_number);
}

void StopHandler() {
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
    flag = (flag > 928.00) ? 928.00 : (flag < 300.00) ? 300.00
                                                      : flag;
    display.println("Frequency: " + String(flag) + " MHz");
    display.display();
  };

  display_info(frequency);

  while (true) {
    buttOK.tick();
    buttNEXT.tick();
    buttPREVIOUS.tick();

    auto handleButton = [&](GButton& btn, float step_single, float step_double, float step_triple, float step_hold) {
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

    auto handleButton = [&](GButton& btn, float step_single, float step_double, float step_triple, float step_hold) {
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
    } else if (buttOK.isHolded()) {
      if (hopperCount > 0) {
        hopper_jam(hopperlist, hopperCount, payload);
      }
      break;
    }
  }
  free(hopperlist);
}

void rec_play() {
  constexpr float MIN_FREQUENCY = 300.00f;
  constexpr float MAX_FREQUENCY = 928.00f;

  constexpr int MIN_SAMPLING = 10;
  constexpr int MAX_SAMPLING = 2000;

  const char* menu_items[] = { "record", "replay", "set frequency", "set sampling int.", "exit" };
  const int MENU_ITEMS_COUNT = sizeof(menu_items) / sizeof(menu_items[0]);

  int current_menu_index = 0;
  bool should_exit = false;

  auto clearAndSetCursor = [](int line = 0) {
    display.clearDisplay();
    display.setCursor(0, line * 10);
  };

  auto displayFrequency = [&]() {
    clearAndSetCursor();
    float constrained_freq = constrain(rec_frequency, MIN_FREQUENCY, MAX_FREQUENCY);
    display.println("Frequency: " + String(constrained_freq) + " MHz");
    display.display();
  };

  auto displaySamplingInterval = [&]() {
    clearAndSetCursor();
    int constrained_sampling = constrain(rec_sampling, MIN_SAMPLING, MAX_SAMPLING);
    display.println("Sampling int.: " + String(constrained_sampling) + " us");
    display.display();
  };

  auto displayMenu = [&](int selected_index) {
    clearAndSetCursor();

    selected_index = constrain(selected_index, 0, MENU_ITEMS_COUNT - 1);

    int visible_lines = display_lines;
    bool need_scroll_indicators = (MENU_ITEMS_COUNT > visible_lines);
    if (need_scroll_indicators) {
      visible_lines--;
    }

    int start_index = 0;
    if (MENU_ITEMS_COUNT > visible_lines) {
      start_index = max(0, selected_index - visible_lines / 2);
      start_index = min(start_index, MENU_ITEMS_COUNT - visible_lines);
    }

    for (int i = 0; i < min(visible_lines, MENU_ITEMS_COUNT); i++) {
      int item_index = start_index + i;
      display.setCursor(2, i * 10);

      if (item_index == selected_index) {
        display.print(">");
      } else {
        display.print(" ");
      }

      display.println(menu_items[item_index]);
    }

    if (need_scroll_indicators) {
      if (start_index > 0) {
        display.setCursor(display.width() - 8, 0);
        display.print("^");
      }

      if (start_index + visible_lines < MENU_ITEMS_COUNT) {
        display.setCursor(display.width() - 8, (visible_lines - 1) * 10);
        display.print("v");
      }
    }

    display.display();
  };

  auto handleFrequencyAdjustment = [&]() {
    displayFrequency();

    auto adjustFrequency = [&](GButton& btn, float step_single, float step_double, float step_triple, float step_hold) {
      if (btn.isSingle()) rec_frequency += step_single;
      else if (btn.isDouble()) rec_frequency += step_double;
      else if (btn.isTriple()) rec_frequency += step_triple;
      else if (btn.isHold()) rec_frequency += step_hold;
      else return false;

      displayFrequency();
      return true;
    };

    while (!buttOK.isSingle()) {
      buttNEXT.tick();
      buttOK.tick();
      buttPREVIOUS.tick();

      adjustFrequency(buttNEXT, 0.1, 0.01, 100, 0.1);
      adjustFrequency(buttPREVIOUS, -0.1, -0.01, -100, -0.1);
    }
  };

  auto handleSamplingAdjustment = [&]() {
    displaySamplingInterval();

    auto adjustSampling = [&](GButton& btn, float step_single, float step_double, float step_triple, float step_hold) {
      if (btn.isSingle()) rec_sampling += step_single;
      else if (btn.isDouble()) rec_sampling += step_double;
      else if (btn.isTriple()) rec_sampling += step_triple;
      else if (btn.isHold()) rec_sampling += step_hold;
      else return false;

      displaySamplingInterval();
      return true;
    };

    while (!buttOK.isSingle()) {
      buttNEXT.tick();
      buttOK.tick();
      buttPREVIOUS.tick();

      adjustSampling(buttNEXT, 1, 10, 100, 1);
      adjustSampling(buttPREVIOUS, -1, -10, -100, -1);
    }
  };

  auto handleMenuNavigation = [&]() {
    if (buttNEXT.isSingle()) {
      current_menu_index = (current_menu_index + 1) % MENU_ITEMS_COUNT;
      displayMenu(current_menu_index);
    } else if (buttPREVIOUS.isSingle()) {
      current_menu_index = (current_menu_index - 1 + MENU_ITEMS_COUNT) % MENU_ITEMS_COUNT;
      displayMenu(current_menu_index);
    }
  };

  auto handleMenuSelection = [&]() {
    if (!buttOK.isSingle()) return;

    while (buttOK.isSingle()) {
      buttOK.tick();
    }

    switch (current_menu_index) {
      case 0:
        record(rec_frequency, rec_sampling);
        displayMenu(current_menu_index);
        break;
      case 1:
        play(rec_frequency, rec_sampling);
        displayMenu(current_menu_index);
        break;
      case 2:
        handleFrequencyAdjustment();
        displayMenu(current_menu_index);
        break;
      case 3:
        handleSamplingAdjustment();
        displayMenu(current_menu_index);
        break;
      case 4: should_exit = true; break;
    }
  };

  displayMenu(current_menu_index);
  while (!should_exit) {
    buttNEXT.tick();
    buttOK.tick();
    buttPREVIOUS.tick();

    if (access_point == 0) server.handleClient();

    handleMenuNavigation();
    handleMenuSelection();
  }
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

  display_info(start_frequency, stop_frequency + range_step);

  while (true) {
    buttOK.tick();
    buttNEXT.tick();
    buttPREVIOUS.tick();

    auto handleButton = [&](GButton& btn, float step_single, float step_double, float step_hold, bool temp) {
      auto check = [&](float step) {
        if (start_frequency < 300.00) start_frequency = 300.00;
        if (start_frequency > 928.00 - step) start_frequency = 928.00 - step;
        if (stop_frequency < start_frequency + step) stop_frequency = start_frequency + step;

        display_info(start_frequency, stop_frequency);
      };
      if (temp) {
        if (btn.isSingle()) start_frequency += step_single;
        else if (btn.isDouble()) start_frequency += step_double;
        else if (btn.isHold()) start_frequency += step_hold;
        else return;
      } else {
        if (btn.isSingle()) stop_frequency += step_single;
        else if (btn.isDouble()) stop_frequency += step_double;
        else if (btn.isHold()) stop_frequency += step_hold;
        else return;
      }

      check(step_single);
    };

    if (temp) {
      handleButton(buttNEXT, range_step, 100.0, range_step, temp);
      handleButton(buttPREVIOUS, -range_step, -100.0, -range_step, temp);
    } else {
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

  for (int i = 0; i < 6; ++i) {
    if (EEPROM.read(i) == 255) {
      if (i == 3) EEPROM.write(i, 1);
      if (i == 4) EEPROM.write(i, 60);
      if (i == 5) EEPROM.write(i, 150);
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
  payload = (byte)EEPROM.read(4);
  EEPROM.get(5, rec_sampling);
  jamdata = new byte[payload];
  if (access_point == 0) {
    WiFi.softAP(ssid, password);

    registerRoute("/", []() { sendHtmlAndExecute(html); });
    registerRoute("/setting_logo", []() { sendHtmlAndExecute(html_logo_setings); });
    registerRoute("/setting_access_point", []() { sendHtmlAndExecute(html_access_point_settings); });
    registerRoute("/setting_jamming_time", []() { sendHtmlAndExecute(html_jamming_time_setting); });
    registerRoute("/setting_range_step", []() { sendHtmlAndExecute(html_step_setting); });
    registerRoute("/setting_payload_size", []() { sendHtmlAndExecute(html_payload_settings); });
    registerRoute("/setting_sampling", []() { sendHtmlAndExecute(html_sampling_settings); });

    registerRoute("/logo_on", []() { storeEEPROMAndSet(0, 0, logo, html); });
    registerRoute("/logo_off", []() { storeEEPROMAndSet(0, 1, logo, html); });
    registerRoute("/access_point_off", []() { AccessPointHandler(1); });
    registerRoute("/editdelay", DelayHandler);
    registerRoute("/stepinterval", StepHandler);
    registerRoute("/sampleinterval", SampleHandler);
    registerRoute("/payload", PayloadHandler);

    registerRoute("/spot_jammer", []() { sendHtmlAndExecute(html_spot_jammer); });
    registerRoute("/keyfob_jammer", []() { sendHtmlAndExecute(html_keyfob_jammer); });
    registerRoute("/range_jammer", []() { sendHtmlAndExecute(html_range_jammer); });
    registerRoute("/hopper_jammer", []() { sendHtmlAndExecute(html_hopper_jammer); });
    registerRoute("/rec_play", RecPlayHandler);

    registerRoute("/spot_jam", SpotFrequencyHandler);
    registerRoute("/keyfob_jam", KeyFobHandler);
    registerRoute("/range_jam", RangeHandler);
    registerRoute("/hopper_jam", HopperHandler);
    registerRoute("/stop_jamming", StopHandler);
    registerRoute("/record", RecordHandler);
    registerRoute("/play", PlayHandler);
    registerRoute("/stop_operation", StopOperationHandler);
    registerRoute("/rec_play_status", RecPlayStatusHandler);

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
    executeAction(sub_menu_number);
  }

  if (buttNEXT.isSingle()) {
    if (main_menu_number == 0) {
      sub_menu_number = (sub_menu_number + 1) % 2;
    } else if (main_menu_number == 1) {
      int max_menu = 2;
      sub_menu_number = (sub_menu_number + 1) % max_menu;
    } else if (main_menu_number == 2) {
      int max_menu = (access_point == 0) ? 5 : 6;
      sub_menu_number = (sub_menu_number + 1) % max_menu;
    }
    updateDisplay(sub_menu_number);
  }

  if (buttPREVIOUS.isSingle()) {
    if (main_menu_number == 0) {
      sub_menu_number = (sub_menu_number - 1 + 2) % 2;
    } else if (main_menu_number == 1) {
      int max_menu = (access_point == 0) ? 2 : 3;
      sub_menu_number = (sub_menu_number - 1 + max_menu) % max_menu;
    } else if (main_menu_number == 2) {
      int max_menu = (access_point == 0) ? 5 : 6;
      sub_menu_number = (sub_menu_number - 1 + max_menu) % max_menu;
    }
    updateDisplay(sub_menu_number);
  }

  if (buttOK.isHolded()) {
    if (main_menu_number != 0) {
      main_menu_number = 0;
      sub_menu_number = 0;
      updateDisplay(sub_menu_number);
    }
  }
}
void updateDisplay(int menuNum) {
  display.clearDisplay();

  if (main_menu_number == 0) {
    const uint8_t* bitmap = (menuNum == 0) ? bitmap_repeaters : bitmap_jammers;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  } else if (main_menu_number == 1) {
    const uint8_t* bitmap = (menuNum == 0) ? bitmap_raw_repeater : 
                            (menuNum == 1) ? bitmap_exit : bitmap_access_point;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  } else if (main_menu_number == 2) {
    const uint8_t* bitmap = (menuNum == 0) ? bitmap_keyfob_jammer : (menuNum == 1) ? bitmap_spot_jammer
                                                                  : (menuNum == 2) ? bitmap_range_jammer
                                                                  : (menuNum == 3) ? bitmap_hopper_jammer
                                                                  : (menuNum == 4) ? bitmap_exit
                                                                                   : bitmap_access_point;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  }

  display.display();
}
void executeAction(int menuNum) {
  if (main_menu_number == 0) {
    if (menuNum == 0) {
      main_menu_number = 1;
      sub_menu_number = 0;
    } else {
      main_menu_number = 2;
      sub_menu_number = 0;
    }
  } else if (main_menu_number == 1) {
    switch (sub_menu_number) {
      case 0: rec_play(); break;
      case 1:
        main_menu_number = 0;
        sub_menu_number = 0;
        break;
      case 2: AccessPointHandler(0); break;
    }
  } else if (main_menu_number == 2) {
    switch (sub_menu_number) {
      case 0: keyfob(); break;
      case 1: spot(); break;
      case 2: range(); break;
      case 3: hopper(); break;
      case 4:
        main_menu_number = 0;
        sub_menu_number = 1;
        break;
      case 5: AccessPointHandler(0); break;
    }
  }
  updateDisplay(sub_menu_number);
}
