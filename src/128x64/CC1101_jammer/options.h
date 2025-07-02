//changeable parameters
float keyfob_list[] = {303.00, 310.00, 315.00, 330.00, 350.00, 370.00, 390.00, 418.00, 433.92};
const char *ssid = "jammer";
const char *password = "W0rthlessS0ul";
const int display_lines = 6;
float rec_frequency = 433.92;
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
#define RECORDINGBUFFERSIZE 32768
//unchangeable parameters
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80); 
byte* jamdata;
int menu_number = 0;
int logo;
int access_point;
int jam_delay;
float range_step;
int rec_sampling;
bool jam_break = false;
int recordedLength = 0;
int main_menu_number = 0;
int sub_menu_number = 0; 
volatile int currentOperation = 0;
volatile bool force_stop = false;
volatile int operationProgress = 0;
volatile int operationCurrent = 0;
volatile int operationTotal = 0;
byte bigrecordingbuffer[RECORDINGBUFFERSIZE] = {0};
byte payload;