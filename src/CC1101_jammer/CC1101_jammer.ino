#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "GyverButton.h"
#include "bitmap.h"
#include "options.h"
#include "jam.h"

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
      display.setCursor(0, 10);
      display.println("Jamming Started");
      display.display();
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
      display.clearDisplay();
      display.drawBitmap(0, 0, bitmap_keyfob_jam, 128, 64, WHITE);
      display.display();
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
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  cc1101initialize();
  ELECHOUSE_cc1101.getCC1101();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  updateDisplay(menu_number);
}

void loop() {
  buttOK.tick();
  buttNEXT.tick();
  buttPREVIOUS.tick(); 
  if (buttOK.isSingle()) {
    executeAction(menu_number);
  }

  if (buttNEXT.isSingle()) {
    menu_number = (menu_number + 1) % 2;
    updateDisplay(menu_number);    
  }

  if (buttPREVIOUS.isSingle()) {
    menu_number = (menu_number - 1) % 2;
    updateDisplay(menu_number);
  }
}
void updateDisplay(int menuNum) {
  display.clearDisplay();
  const uint8_t* bitmap = (menu_number == 0) ? bitmap_keyfob_jammer : bitmap_misc_jammer;
  display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
  display.display();
}
void executeAction(int menuNum) {
  switch (menu_number) {
    case 0: keyfob(); break;
    case 1: misc(); break;
  }
  updateDisplay(menu_number);
}
