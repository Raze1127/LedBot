#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include "FastLED.h"
#include <Thread.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <EEPROM.h>
#define NUM_LEDS 240

#define PIN 3
const uint8_t PixelPin = 3;
#include "CTBot.h"
CTBot myBot;

#define LED_COUNT 240  
int max_bright = 255;   
int ledMode = 1;

int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];

// цвета мячиков для режима
byte ballColors[3][3] = {
  {0xff, 0, 0},
  {0xff, 0xff, 0xff},
  {0   , 0   , 0xff},
};

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int modik = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i].setRGB( cred, cgrn, cblu);
  }
}

void one_color_allHSV(int ahue) {    //-SET ALL LEDS TO ONE COLOR (HSV)
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i] = CHSV(ahue, thissat, 255);
  }
}
#define LED_DT 3


String ssid  = "TP-Link_4885"    ; // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "29647835"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY

//String ssid  = "cybero3g"    ; // REPLACE mySSID WITH YOUR WIFI SSID
//String pass  = "MVTKac4U"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY


String token2 = "5886982997:AAHiErhsBQHGvTw0_3wKZ-XVd5gESQmmSoи"   ;
unsigned int localPort = 44444;
#define BUFFER_LEN 1024
char packetBuffer[BUFFER_LEN];
uint8_t N = 0;
WiFiUDP port;
byte selectedEffect = 1 ;
CTBotInlineKeyboard myKbd;
CTBotReplyKeyboard myKbd2;

#define PRINT_FPS 1
String token = "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";  // openAI API Key
int max_tokens = 1024;
TaskHandle_t Task1;
TaskHandle_t Task2;
int dat;
IPAddress ip(192, 168, 0, 100);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> ledstrip(NUM_LEDS, PixelPin);

/** Define callback class for server events */


#define TURN_ON_CALLBACK "TURNON" 
#define TURN_OFF_CALLBACK "TURNOFF" 
#define NEXT_CALLBACK "NEXT"
#define PREV_CALLBACK "PREV"
#define MUSIC_CALLBACK "MUSIC"
const char * udpAddress = "192.168.0.100";
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");
  delay(1000);
  
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token2);
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
    
  else
    Serial.println("\ntestConnection NOK");

  myKbd.addButton("Previus", PREV_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Next", NEXT_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Turn off", TURN_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Turn off", TURN_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Music party", MUSIC_CALLBACK, CTBotKeyboardButtonQuery);

  myKbd2.addButton("/turnon");
   
  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость
//
  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  one_color_all(0, 0, 0);          // погасить все светодиоды
 LEDS.show(); 

//    ledstrip.Begin();//Begin output
//    ledstrip.Show();//Clear the strip for use
 
  Serial.println("The device started, now you can pair it with bluetooth!");
   WiFi.mode(WIFI_STA);
    
    WiFi.begin(ssid, pass);
    Serial.println("");
    // Connect to wifi and print the IP address over serial
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    delay(10000);
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    port.begin(localPort);
    
    
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}


void change_mode(int newmode) {
  thissat = 255;
  switch (newmode) {
    case 0: one_color_all(0, 0, 0); LEDS.show(); break; //---ALL OFF
    case 1: one_color_all(255, 255, 255); LEDS.show(); break; //---ALL ON
    
    case 2: thisdelay = 20; break;                      //---STRIP RAINBOW FADE
    case 3: thisdelay = 20; thisstep = 10; break;       //---RAINBOW LOOP
    case 4: thisdelay = 40; thishue = 0; break;         //---POLICE LIGHTS SOLID
    case 5: thishue = 160; thissat = 50; break;         //---STRIP FLICKER
    case 6: thisdelay = 15; thishue = 0; break;        //---PULSE COLOR SATURATION
    case 7: thisdelay = 80; break;                     //---MARCH RWB COLORS
    case 9: thisdelay = 25; thishue = 0; break;        //---EMERGECNY STROBE
    case 10: thisdelay = 100; thishue = 0; break;       //---KITT
    case 11: thisdelay = 5; break;                      //---NEW RAINBOW LOOP
    case 12: thisdelay = 20; break;                     // rainbowCycle
    case 13: thisdelay = 100; break;                    // Strobe
    
    case 14: one_color_all(255, 0, 0); LEDS.show(); break; //---ALL RED
    case 15: one_color_all(0, 255, 0); LEDS.show(); break; //---ALL GREEN
    case 16: one_color_all(0, 0, 255); LEDS.show(); break; //---ALL BLUE
    case 17: one_color_all(255, 255, 0); LEDS.show(); break; //---ALL COLOR X
    case 18: one_color_all(0, 255, 255); LEDS.show(); break; //---ALL COLOR Y
    case 19: one_color_all(255, 0, 255); LEDS.show(); break; //---ALL COLOR Z
  }
  Serial.println(ledMode);
  bouncedirection = 0;
  one_color_all(0, 0, 0);
}


#if PRINT_FPS
    uint16_t fpsCounter = 0;
    uint32_t secondTimer = 0;
#endif

int current_level = 0; // Текущий уровень громкости на ленте
int new_level = 0; // Новый уровень громкости, который приходит из Python скрипта
unsigned long previousMillis = 0;
#define FADE_RATE 5
#define STEP_DELAY 10
int current[NUM_LEDS] = {0}; // Текущая яркость каждого светодиода
int target[NUM_LEDS] = {0};
int poi;
// *** REPLACE FROM HERE ***
void loop() {

   if (Serial.available() > 0) {     // если что то прислали
    ledMode = Serial.parseInt();    // парсим в тип данных int
    Serial.println(ledMode);
    change_mode(ledMode); // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)
    modik = Serial.parseInt();
  }
    
    if(ledMode == 1000){
      int packetSize = port.parsePacket();
      
    if (packetSize) {
        int len = port.read(packetBuffer, BUFFER_LEN);
        for(int i = 0; i < len; i+=4) {
            packetBuffer[len] = 0;
            N = packetBuffer[i];
            RgbColor pixel((uint8_t)packetBuffer[i+1], (uint8_t)packetBuffer[i+2], (uint8_t)packetBuffer[i+3]);//color
            ledstrip.SetPixelColor(N, pixel);//N is the pixel number
        } 
        ledstrip.Show();
        #if PRINT_FPS
            fpsCounter++;
            Serial.print("/");//Monitors connection(shows jumps/jitters in packets)
        #endif
    }
    
    }else{
      TelegramCheck();
      Led();
      }


    
}






void response(String qa) {
  HTTPClient http;

  http.begin("https://api.writesonic.com/v2/business/content/chatsonic?engine=premium&language=ru");
  http.addHeader("accept", "application/json");
  http.addHeader("content-type", "application/json");
  http.addHeader("X-API-KEY", "fe7e6dd6-1680-4724-8742-b931102507b3");

  String lol = "{\"enable_google_results\": false,\"enable_memory\": true,\"input_text\":\"ты бот для управления лед лентой. Выбери один из существующих вариантов подсветки более подходящий под запрос, ответ должен содержать ТОЛЬКО номер пункта, без слов, только цифра: 1. Изменение цветов: зеленого, синего и красного при помощи медленных затуханий. 2. эффект стробоскопа. Быстрое мерцание белым ярким святом, почти как в рейв клубах 3. Появление рандомное двух красных глаз, моежт быть страшным в темноте 4. небольшая полосочка, бегающая из стороны в стороны, выглядит эффектно 5. две полосочки бегающие из стороны в сторону, достаточно интересный эффект  6. в разных точках точках ленты светодиоды загораются красным цветом 7. в разных точках точках ленты светодиоды загораются разными цветами 8. лента загорается в разных точках и мерцает в разных местах, эффект похож на блестки, очень быстрый и тусовочный 9. лента горит мягким белым светом и в некоторых точках ленты светодиоды мерцают, эффект похож на мерцание снежинок на снегу, очень спокойный эффект 10. по ленте идет линия из множества маленьких полосочек белого цвета 11. лента заполняется зеленым цветом и потом также постепенно гаснет 12. эффект переливания цветов радуги, очень спокойный и универсальный эффект 13. лента из полосочек маленьких, юыстро идущих в сторону красного цвета 14. лента из полосочек маленьких, юыстро идущих в сторону цветов радуги 15. эффект горящего огня 16. эффект прыгающего одного мячика, интересный эффект 17. эффект прыгающих трех мячиков, интересный эффект 18. эффект метеора,самый интересный и завораживающий эффект Запрос: "" + qa + "" \"}";


  const char* payload = lol.c_str();
  int payloadLength = strlen(payload);
  uint8_t payloadBytes[payloadLength];
  for (int i = 0; i < payloadLength; i++) {
    payloadBytes[i] = payload[i];
  }
  http.setTimeout(100000);
  int httpCode = http.sendRequest("POST", payloadBytes, payloadLength);

  if (httpCode > 0) {
    String response = http.getString();
    char message = response[12];
    selectedEffect = message;
    Serial.println(message);
    //Serial.println(response);

  } else {
    Serial.println("Error: " + http.errorToString(httpCode));
    selectedEffect = 0;
  }

  http.end();

}




void Led() {
  
  switch (ledMode) {
    case 999: break;                           // пазуа
    case  2: rainbow_fade(); break;            // плавная смена цветов всей ленты
    case  3: rainbow_loop(); break;            // крутящаяся радуга
    case  4: ems_lightsALL(); break;           // вращается половина красных и половина синих
    case  5: flicker(); break;                 // случайный стробоскоп
    case 6: pulse_one_color_all_rev(); break; // пульсация со сменой цветов
    case 7: rwb_march(); break;               // белый синий красный бегут по кругу (ПАТРИОТИЗМ!)
    case 8: white_temps(); break;             // бело синий градиент (?)
    case 9: ems_lightsSTROBE(); break;        // полицейская мигалка
    case 10: kitt(); break;                    // случайные вспышки красного в вертикаьной плоскости
    case 11: new_rainbow_loop(); break;        // крутая плавная вращающаяся радуга
    case 12: rainbowCycle(thisdelay); break;                                        // очень плавная вращающаяся радуга
    case 13: Strobe(0xff, 0xff, 0xff, 10, thisdelay, 10); break;                  // стробоскоп
  }
}




// *************************
// ** LEDEffect Functions **
// *************************
#define TURN_ON_CALLBACK "TURNON" 
#define TURN_OFF_CALLBACK "TURNOFF" 
#define NEXT_CALLBACK "NEXT"
#define PREV_CALLBACK "PREV"
#define MUSIC_CALLBACK "MUSIC"

void TelegramCheck() {
  TBMessage msg;

  if (myBot.getNewMessage(msg)) {
    // check what kind of message I received
    if (msg.messageType == CTBotMessageText) {
      // received a text message
      if (msg.text.equalsIgnoreCase("/start")) {
        ledMode = 12;
        // the user is asking to show the inline keyboard --> show it
        myBot.sendMessage(msg.sender.id, "Привет, лента готова работать", myKbd);
      }
      else {
        // the user write anithing else --> show a hint message
        myBot.sendMessage(msg.sender.id, "Try 'show keyboard'");
      }
    } else if (msg.messageType == CTBotMessageQuery) {
      // received a callback query message
      if (msg.callbackQueryData.equals(TURN_OFF_CALLBACK)) {
        
         ledMode = 9999;
        myBot.endQuery(msg.callbackQueryID, "Лента выключена", true);

        
      } else if (msg.callbackQueryData.equals(NEXT_CALLBACK)) {
        
        myBot.endQuery(msg.callbackQueryID, "Следующий эффект включен");
        ledMode++;
        
      }else if (msg.callbackQueryData.equals(PREV_CALLBACK)) {
        ledMode--;
        myBot.endQuery(msg.callbackQueryID, "Прерыдущий эффект включен");
        
      }else if (msg.callbackQueryData.equals(MUSIC_CALLBACK)) {
        ledMode =1000;
        myBot.endQuery(msg.callbackQueryID, "Включите Вовин комп и наслаждайтесь музыкой)");
        
      }
      else if (msg.callbackQueryData.equals(TURN_ON_CALLBACK)) {
        ledMode =1;
        myBot.endQuery(msg.callbackQueryID, "Лента работает");
        
      }
    }
  }
}
