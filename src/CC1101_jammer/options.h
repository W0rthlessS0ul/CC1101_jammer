//changeable parameters
float keyfob_list[] = {303.00, 310.00, 315.00, 330.00, 350.00, 370.00, 390.00, 418.00, 433.92};
const char *ssid = "jammer";
const char *password = "W0rthlessS0ul";
//better not change
GButton buttNEXT(14);
GButton buttOK(12);
GButton buttPREVIOUS(13);
byte sck = 18;
byte miso = 19;
byte mosi = 23;
byte ss = 5;
int gdo0 = 2;
int gdo2 = 4;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define EEPROM_SIZE 512
//unchangeable parameters
byte jamdata[60];
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80); 
int menu_number = 0;
int logo;
int access_point;