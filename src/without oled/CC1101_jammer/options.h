//changeable parameters
float keyfob_list[] = {303.00, 310.00, 315.00, 330.00, 350.00, 370.00, 390.00, 418.00, 433.92};
const char *ssid = "jammer";
const char *password = "W0rthlessS0ul";
//better not change
byte sck = 18;
byte miso = 19;
byte mosi = 23;
byte ss = 5;
int gdo0 = 2;
int gdo2 = 4;
#define EEPROM_SIZE 512
WebServer server(80); 
byte* jamdata;
int jam_delay;
float range_step;
bool jam_break = false;
byte payload;