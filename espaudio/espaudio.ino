//TODO:
// reconnect wifi -> test
// buttons @ atom
//


#include "Arduino.h"
//old version 1.0.6
#ifdef ESP32
#include "Audio.h"
#include "SPI.h"
#else
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioGeneratorAAC.h"
#include "AudioOutputI2S.h"
#endif
#include "SD.h"
#include "FS.h"
#ifdef ESP32
#include <ArduinoOTA.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include <Wire.h>


//Pimoroni Speaker Phat
//#define PSP
//Pimoroni Speaker Phat replacement
//#define PSPM5ECHO
//Pimoroni Speaker Phat replacement2 audioshim
#define PSPSHIM
//Pimoroni Phat Beat
//#define PBT
//Pimoroni Audio Shim
//#define AUDIOSHIM
//Makerfabs Audio V2.0, diff I2S (RED)
//#define MFA
//diff I2S//throw away
//#define DAC
//M5 ATOM ECHO, diff I2S
//#define ECHO
//M5 ATOM ECHO, diff I2S
//#define ECHO2
//ESP8266
//#define D1DAC
//ATOM HAT
//#define M5SPEAKER
//m5stickc plus hat
//#define M5SPK

#ifdef PSP

#define SD_ESPONE
#define I2S_ESPONE

#define HOSTNAME "ESPSPEAKERPHAT"
#define TOPIC_STATUS "espaudiospeakerphat/status"
#define TOPIC_IP "espaudiospeakerphat/ip"
#define TOPIC_ENABLED "espaudiospeakerphat/enabled"
#define TOPIC_ENABLE "espaudiospeakerphat/enable"
#define TOPIC_VOLUME "espaudiospeakerphat/volume"
#define MAXVOL 15
#endif

#ifdef PSPSHIM

#define SD_ESPONE
#define I2S_ESPONE

#define HOSTNAME "ESPSHIMPSP"
#define TOPIC_STATUS "espaudiospeakerphat/status"
#define TOPIC_IP "espaudiospeakerphat/ip"
#define TOPIC_ENABLED "espaudiospeakerphat/enabled"
#define TOPIC_ENABLE "espaudiospeakerphat/enable"
#define TOPIC_VOLUME "espaudiospeakerphat/volume"
#define MAXVOL 19
#endif

#ifdef PSPM5ECHO

//replacement/workaround for PSP
#define M5ECHO

#define HOSTNAME "ESPM5PSP"
#define TOPIC_STATUS "espaudiospeakerphat/status"
#define TOPIC_IP "espaudiospeakerphat/ip"
#define TOPIC_ENABLED "espaudiospeakerphat/enabled"
#define TOPIC_ENABLE "espaudiospeakerphat/enable"
#define TOPIC_VOLUME "espaudiospeakerphat/volume"
#define MAXVOL 19
#endif


#ifdef PBT

#define SD_ESPONE
#define I2S_ESPONE

#define HOSTNAME "ESPPHATBEAT"
#define TOPIC_STATUS "espaudiophatbeat/status"
#define TOPIC_IP "espaudiophatbeat/ip"
#define TOPIC_ENABLED "espaudiophatbeat/enabled"
#define TOPIC_ENABLE "espaudiophatbeat/enable"
#define TOPIC_VOLUME "espaudiophatbeat/volume"
#define MAXVOL 12
#endif

#ifdef AUDIOSHIM

#define SD_ESPONE
#define I2S_ESPONE

#define HOSTNAME "ESPAUDIOSHIM"
#define TOPIC_STATUS "espaudioshim/status"
#define TOPIC_IP "espaudioshim/ip"
#define TOPIC_ENABLED "espaudioshim/enabled"
#define TOPIC_ENABLE "espaudioshim/enable"
#define TOPIC_VOLUME "espaudioshim/volume"
#define MAXVOL 12
#endif


#ifdef MFA
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define HAZDISPLAY

//SD Card
#define SD_CS 22
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

//Digital I/O used  //Makerfabs Audio V2.0
#define I2S_DOUT 27
#define I2S_BCLK 26
#define I2S_LRC 25

//SSD1306
#define MAKEPYTHON_ESP32_SDA 4
#define MAKEPYTHON_ESP32_SCL 5
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SSD1306_WHITE   1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define HOSTNAME "ESPAUDIORED"
#define TOPIC_STATUS "espaudiored/status"
#define TOPIC_IP "espaudiored/ip"
#define TOPIC_ENABLED "espaudiored/enabled"
#define TOPIC_ENABLE "espaudiored/enable"
#define TOPIC_VOLUME "espaudiored/volume"
#define MAXVOL 15
#endif

#ifdef DAC

//Digital I/O used pcm5102
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

#define HOSTNAME "ESPDAC1"
#define TOPIC_STATUS "espaudiodac1/status"
#define TOPIC_IP "espaudiodac1/ip"
#define TOPIC_ENABLED "espaudiodac1/enabled"
#define TOPIC_ENABLE "espaudiodac1/enable"
#define TOPIC_VOLUME "espaudiodac1/volume"
#define MAXVOL 15
#endif

#ifdef ECHO

#define M5ECHO

#define HOSTNAME "ESPECHO"
#define TOPIC_STATUS "espaudioecho/status"
#define TOPIC_IP "espaudioecho/ip"
#define TOPIC_ENABLED "espaudioecho/enabled"
#define TOPIC_ENABLE "espaudioecho/enable"
#define TOPIC_VOLUME "espaudioecho/volume"
#define MAXVOL 19
#endif

#ifdef ECHO2

#define M5ECHO

#define HOSTNAME "ESPECHO2"
#define TOPIC_STATUS "espaudioecho2/status"
#define TOPIC_IP "espaudioecho2/ip"
#define TOPIC_ENABLED "espaudioecho2/enabled"
#define TOPIC_ENABLE "espaudioecho2/enable"
#define TOPIC_VOLUME "espaudioecho2/volume"
#define MAXVOL 19
#endif

#ifdef D1DAC32
#define I2S_DOUT 3
#define I2S_BCLK 2
#define I2S_LRC 15

#define HOSTNAME "ESPD1DAC32"
#define TOPIC_STATUS "espaudiod1dac32/status"
#define TOPIC_IP "espaudiod1dac32/ip"
#define TOPIC_ENABLED "espaudiod1dac32/enabled"
#define TOPIC_ENABLE "espaudiod1dac32/enable"
#define TOPIC_VOLUME "espaudiod1dac32/volume"
#define MAXVOL 19
#endif

#ifdef D1DAC
#define I2S_DOUT 3
#define I2S_BCLK 2
#define I2S_LRC 15

#define HOSTNAME "ESPD1DAC"
#define TOPIC_STATUS "espaudiod1dac/status"
#define TOPIC_IP "espaudiod1dac/ip"
#define TOPIC_ENABLED "espaudiod1dac/enabled"
#define TOPIC_ENABLE "espaudiod1dac/enable"
#define TOPIC_VOLUME "espaudiod1dac/volume"
#define MAXVOL 19
#endif

#ifdef I2S_ESPONE
#define I2S_DOUT 25
#define I2S_BCLK 26
#define I2S_LRC 32
#endif

#ifdef M5ECHO
#define I2S_DOUT 22
#define I2S_BCLK 19
#define I2S_LRC 33
#define M5ATOM
#endif

#ifdef M5SPEAKER

#define HOSTNAME "ESPSPEAKER"
#define TOPIC_STATUS "espaudiospeaker/status"
#define TOPIC_IP "espaudiospeaker/ip"
#define TOPIC_ENABLED "espaudiospeaker/enabled"
#define TOPIC_ENABLE "espaudiospeaker/enable"
#define TOPIC_VOLUME "espaudiospeaker/volume"
#define MAXVOL 19

#define I2S_DOUT 25
#define I2S_BCLK 22
#define I2S_LRC 21
#define M5ATOM
#endif

#ifdef M5SPK
#include <M5StickC.h>

#define HAZDISPLAY

#define HOSTNAME "ESPSPK"
#define TOPIC_STATUS "espaudiospk/status"
#define TOPIC_IP "espaudiospk/ip"
#define TOPIC_ENABLED "espaudiospk/enabled"
#define TOPIC_ENABLE "espaudiospk/enable"
#define TOPIC_VOLUME "espaudiospk/volume"
#define MAXVOL 19

#define M5ATOM
#endif

#ifdef M5ATOM
#define RGBLED 27
#define BTN 39
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(1, RGBLED, NEO_GRB + NEO_KHZ800);
#endif


#ifdef SD_ESPONE
//SD Card // from waveshare esp one
#define SD_CS 22
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18
#endif

#define VOLMIN 0
#define VOLMAX 21

#define ssid "***"
#define password "***"
#define mqtt_server "1.1.1.1"

#define TOPIC_ANNOUNCE_GLOBAL "espaudio/announce"
#define TOPIC_URL_GLOBAL "espaudio/url"


unsigned long previousMillis = 0;
unsigned long interval = 30000;

WiFiClient espClient;
PubSubClient client(espClient);

#define NUM 5
char topics[NUM][255] = {TOPIC_ANNOUNCE_GLOBAL,
                         TOPIC_URL_GLOBAL,
                         "espaudio/cmnd",
                         TOPIC_ENABLE,
                         TOPIC_VOLUME
                        };

char cmnd[2][5] = { "help", "stop"};
int cmndSize = sizeof ( cmnd) / sizeof ( cmnd[0]);

boolean silence = false;

boolean changeUrl = false;
boolean announcing = false;
String playingNow;
#define MAX_ANNOUNCEMENTS 5
String announcements[MAX_ANNOUNCEMENTS] = {};
int announcementCount = 0;
int nextAnnouncement = 0;

#ifdef PSP
const byte ledMappings[] = {0x01, 0x02, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11};

void wireWrite(uint8_t reg, uint8_t val) ;
void beginI2c() ;
void endI2c();
void initI2c();
void updateI2c();
void cleanupI2c();
void ledI2c(uint8_t addr, uint8_t i);
#endif

#ifdef MFA
//Button
const int Pin_vol_up = 39;
const int Pin_vol_down = 36;
const int Pin_mute = 35;

const int Pin_previous = 15;
const int Pin_pause = 33;
const int Pin_next = 2;
#define DEF INPUT_PULLUP
#endif
#ifdef PBT
//Button
const int Pin_vol_up = 37;// - 36 - 16
const int Pin_vol_down = 36;// - 37 - 26
const int Pin_mute = 38;// - 32 - 12

const int Pin_previous = 21;// - 33 - 13  //LED!!!
const int Pin_pause = 35;// - 31 - 6
const int Pin_next = 34;// - 29 - 5
#define DEF INPUT
#endif

#ifdef M5ATOM
//single button

#endif

String streamTitle;
String station;

#ifdef ESP32
#ifndef M5SPK
Audio audio;
#else
Audio audio(true, I2S_DAC_CHANNEL_LEFT_EN);
#endif
#else
AudioGenerator *decoder = NULL;
AudioFileSourceICYStream *file = NULL;
AudioFileSourceBuffer *buff = NULL;
AudioOutputI2S *out = NULL;

void StopPlaying()
{
  if (decoder) {
    decoder->stop();
    delete decoder;
    decoder = NULL;
  }
  if (buff) {
    buff->close();
    delete buff;
    buff = NULL;
  }
  if (file) {
    file->close();
    delete file;
    file = NULL;
  }
}
const int preallocateBufferSize = 5 * 1024;
const int preallocateCodecSize = 29192; // MP3 codec max mem needed
void *preallocateBuffer = NULL;
void *preallocateCodec = NULL;

void MDCallback(void *cbData, const char *type, bool isUnicode, const char *str)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  (void) ptr;
  if (strstr_P(type, PSTR("Title"))) {
    //    strncpy(title, str, sizeof(title));
    //    title[sizeof(title)-1] = 0;
    streamTitle = String(type);
  } else {
    // Who knows what to do?  Not me!
  }
}
void StatusCallback(void *cbData, int code, const char *string)
{ /*
    const char *ptr = reinterpret_cast<const char *>(cbData);
    (void) code;
    (void) ptr;
    strncpy_P(status, string, sizeof(status)-1);
    status[sizeof(status)-1] = 0;*/
}
int volume = 100;



void startPlaying() {
  Serial.printf_P(PSTR("created icystream\n"));
  file->RegisterMetadataCB(MDCallback, NULL);
  buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
  Serial.printf_P(PSTR("created buffer\n"));
  buff->RegisterStatusCB(StatusCallback, NULL);
  decoder =/* isAAC ? (AudioGenerator*) new AudioGeneratorAAC(preallocateCodec, preallocateCodecSize) :*/ (AudioGenerator*) new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
  Serial.printf_P(PSTR("created decoder\n"));
  decoder->RegisterStatusCB(StatusCallback, NULL);
  Serial.printf_P("Decoder start...\n");
  decoder->begin(buff, out);
  out->SetGain(((float)volume) / 100.0);
  if (!decoder->isRunning()) {
    Serial.printf_P(PSTR("Can't connect to URL"));
    StopPlaying();

    //strcpy_P(status, PSTR("Unable to connect to URL"));
    //retryms = millis() + 2000;
  }
}

void PumpDecoder()
{
  if (decoder && decoder->isRunning()) {
    //strcpy_P(status, PSTR("Playing")); // By default we're OK unless the decoder says otherwise
    if (!decoder->loop()) {
      Serial.printf_P(PSTR("Stopping decoder\n"));
      StopPlaying();
      //retryms = millis() + 2000;
    }
  }
}
#endif


void callback(char* topic, byte * payload, unsigned int length);
#ifdef ESP32
void setupOta();
#endif
void setupWifi();
void setupMqtt();
void reconnect();


#ifdef PSP

#define NUMLED 10

void setupLeds() {
  Wire.begin(18, 23);
  initI2c();
  Serial.println("SETUP LEDs");

  for (int i = 1; i < NUMLED + 1; i++) {
    for (int k = 1; k < NUMLED + 1; k++) {
      if (i == k) {
        ledI2c(ledMappings[i - 1], 125);
      } else {
        ledI2c(ledMappings[i - 1], 0);
      }
    }
  }
  for (int i = NUMLED - 1; i > -1; i--) {
    for (int k = 1; k < NUMLED + 1; k++) {
      if (i == k) {
        // eines anmachen
        ledI2c(ledMappings[i - 1], 125);
      } else if (i == 0) {
        // i == 0, alle ausmachen
        ledI2c(ledMappings[i - 1], 0);
      } else {
        // den rest ausmachen
        ledI2c(ledMappings[i - 1], 0);
      }
    }
  }
}
#endif


#ifdef HAZDISPLAY
#ifdef MFA
void lcd_text(String text)
{
  display.clearDisplay();

  //  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(text);
  display.display();
  //delay(500);
}
void lcd_text(String text, String text2)
{
  display.clearDisplay();

  //  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(text);
  display.println(text2);
  display.display();
  //delay(500);
}
#endif
#ifdef M5SPK
void lcd_text(String text)
{
  M5.Lcd.fillScreen(TFT_BLACK);

  M5.Lcd.setTextSize(1);              // Normal 1:1 pixel scale
  M5.Lcd.setTextColor(TFT_WHITE); // Draw white text
  M5.Lcd.setCursor(0, 0);             // Start at top-left corner
  M5.Lcd.println(text);
  //M5.Lcd.display();
  //delay(500);
}
void lcd_text(String text, String text2)
{
  M5.Lcd.fillScreen(TFT_BLACK);

  M5.Lcd.setTextSize(1);              // Normal 1:1 pixel scale
  M5.Lcd.setTextColor(TFT_WHITE); // Draw white text
  M5.Lcd.setCursor(0, 0);             // Start at top-left corner
  M5.Lcd.println(text);
  M5.Lcd.println(text2);
  //M5.Lcd.display();
  //delay(500);
}
#endif

const int pages = 2;
int currentPage = -1;
boolean dataUpdate = false;

void setDisplayPage(int p) {
  if ((!announcing) & (currentPage != p | dataUpdate)) {
    dataUpdate = false;
    switch (p) {
      case (3): break;
      case (2): break;
      case (1): lcd_text(streamTitle); break;
      case (0): lcd_text(station); break;
    }
  }
}

void setupHardwareLcd() {
  //LCD
#ifdef MFA
  Wire.begin(MAKEPYTHON_ESP32_SDA, MAKEPYTHON_ESP32_SCL);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setRotation(2);
  display.clearDisplay();
#endif
#ifdef M5SPK
  M5.begin();
  M5.Lcd.setRotation(3);

#endif
}
#endif

void setupHardwareButtons() {

#if defined(PBT) || defined(MFA)
  //IO mode init
  pinMode(Pin_vol_up, DEF);
  pinMode(Pin_vol_down, DEF);
  pinMode(Pin_mute, DEF);
  pinMode(Pin_previous, DEF);
  pinMode(Pin_pause, DEF);
  pinMode(Pin_next, DEF);
#endif
#if defined(M5ATOM)
  //IO mode init
  pinMode(BTN, INPUT);
#endif
}

#if defined(PSP) || defined(PBT) || defined(MFA) || defined(AUDIOSHIM)
File root;
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
void setupHardwareSd() {
  //SD(SPI)
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  if (!SD.begin(SD_CS, SPI))
  {
    Serial.println("Card Mount Failed");
#ifdef HAZDISPLAY
#ifdef MFA
    lcd_text("SD ERR");
#endif
#endif
    //    while (1)
    //      ;
  }
  else
  {
#ifdef HAZDISPLAY
#ifdef MFA
    lcd_text("SD OK");
#endif
#endif
  }
  root = SD.open("/");

  printDirectory(root, 0);

  Serial.println("done!");

}
#endif

void setup() {
  setupHardware();
  WiFi.disconnect();
  setupWifi();
#ifdef ESP32
  setupOta();
#endif
  setupMqtt();
#ifdef PSP
  Wire.begin(18, 23);
  setupLeds();
#endif
#ifdef MFA
#endif
}

void setupHardware() {
#ifndef D1DAC32
  setupHardwareSerial();
#endif
#if defined(PBT) || defined(MFA) || defined(M5ATOM)
  setupHardwareButtons();
#endif
#ifdef HAZDISPLAY
  setupHardwareLcd();
#endif
#if defined(PSP) || defined(PBT) || defined(MFA) || defined(AUDIOSHIM)
  setupHardwareSd();
#endif
  setupHardwareAudio();
#ifdef PBT
  //LED bars off
  pinMode(39, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(39, LOW);
  digitalWrite(4, LOW);
#endif
#ifdef M5ATOM
  //RGBLED
  pixels.begin();
  pixels.clear();
  pixels.show();
#endif
}

void setupHardwareSerial() {
  //Serial
  Serial.begin(115200);
}

void setupHardwareAudio() {
#ifdef ESP32
#ifndef M5SPK
  //Audio(I2S)
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
#endif
  audio.setVolume(MAXVOL); // 0...21
#else
  preallocateBuffer = malloc(preallocateBufferSize);
  preallocateCodec = malloc(preallocateCodecSize);
  if (!preallocateBuffer || !preallocateCodec) {
    Serial.printf_P(PSTR("FATAL ERROR:  Unable to preallocate %d bytes for app\n"), preallocateBufferSize + preallocateCodecSize);
    while (1) delay(1000); // Infinite halt
  }
  audioLogger = &Serial;
  file = NULL;
  buff = NULL;
  out = new AudioOutputI2S();
  decoder = NULL;
#endif
}
// put your setup code here, to run once:

uint run_time = 0;
uint8_t vol = 0;
#if defined(PBT) || defined(MFA) || defined (M5ATOM)
boolean paused = false;
uint button_time = 0;
#endif
#ifdef HAZDISPLAY
uint page_time = 0;
#endif

uint enable_time = 0;
uint title_time = 0;
boolean sendStation = false;
boolean sendTitle = false;


#ifdef PSP
uint16_t vu = 0;
#endif


void audioloop() {
#ifdef ESP32
  audio.loop();
#else
  PumpDecoder();
#endif
}
boolean audioIsRunning() {
#ifdef ESP32
  return audio.isRunning();
#else
  return (decoder && decoder->isRunning());
#endif
}

void audioStopSong() {
#ifdef ESP32
  audio.stopSong();
#else
  StopPlaying();
#endif
}


void audioStartSong() {
  if (strcmp(playingNow.c_str(), "") != 0) {
    Serial.println("Playing now... ");
    Serial.println(playingNow);
#ifdef ESP32
    audio.connecttohost(playingNow.c_str());
#else
    file = new AudioFileSourceICYStream(playingNow.c_str());
    startPlaying();
#endif
  }
}

void audioStartAnnouncement() {
#ifdef ESP32
  audio.connecttohost(announcements[nextAnnouncement].c_str());
#else
  file = new AudioFileSourceICYStream(announcements[nextAnnouncement].c_str());
  startPlaying();
#endif
}

#ifdef PSP
void processVU() {
  //=================================VU METER==========================================
  // put your main code here, to run repeatedly:
  if (audio.isRunning()) {
    //if (vu > 0) vu-=4;
    //vu = max(audio.getVUlevel(), vu);
    vu = audio.getVUlevel();
    uint8_t v = map (vu, 0, 255, 0, NUMLED + 1);
    /*
      if (v > 0) {
      Serial.print((char)power, HEX);
      Serial.print("  ==>>   ");
      Serial.print(v, HEX);
      Serial.println();
      }
    */
    for (int i = 1; i < NUMLED + 1; i++) {
      //or led0 is lit on no sound
      if (v == 0 || v < i) {
        ledI2c(ledMappings[i - 1], 0);
      } else {
        ledI2c(ledMappings[i - 1], 150);
      }
    }
  } else {
    for (int i = 0; i < NUMLED ; i++) {
      ledI2c(ledMappings[i], 0);
    }
  }
  updateI2c();
}
#endif

#ifdef M5ATOM
void processRGB() {
  //rgb led off
  pixels.clear();
  if (silence) {
    pixels.setPixelColor(0, pixels.Color(192, 0, 192));
  } else {
    if (audioIsRunning()) {
      //rgb led green = playing
      if (announcing) {
        pixels.setPixelColor(0, pixels.Color(192, 0, 0));
      } else {
#if defined(PSPM5ECHO) || defined(ECHO2)
        //green damaged??
        pixels.setPixelColor(0, pixels.Color(0, 32, 192));
#else
        pixels.setPixelColor(0, pixels.Color(0, 192, 0));
#endif
      }
    }
  }
  pixels.show();
}
#endif

void loop() {
  //=====================HANDLE WIFI========================================
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

#ifdef ESP32
  //=====================HANDLE OTA UPDATE==================================
  ArduinoOTA.handle();
#endif
  //=====================HANDLE MQTT========================================
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //=====================HANDLE AUDIO=======================================
  audioloop();
#ifdef M5ATOM
  //=====================HANDLE RGB LED=====================================
  processRGB();
#endif
  //=====================HANDLE AUDIO 2 ====================================
  //do we need to change something ?
  // a audio is not running
  // b we have announcements and are not announcing
  // c url change requested
  if (!silence) {
    if (announcementCount > 0 && !announcing ) {
      // regular stream running or not, change to announcements
      if (audioIsRunning()) audioStopSong();
      announcing = true;
    } else if (announcementCount > 0 && announcing ) {
      // next announcement if not playing
      if (!audioIsRunning()) {
        Serial.println("Announcing... ");
        Serial.println(announcements[nextAnnouncement]);
        audioStartAnnouncement();
        announcementCount--;
        nextAnnouncement++;
        nextAnnouncement %= MAX_ANNOUNCEMENTS;
      } else {
        //announcements are already playing
      }
    } else if (announcementCount == 0 && announcing ) {
      // all announcements played, return to regular stream
      if (!audioIsRunning()) {
        announcing = false;
      }
    } else {
      // no announcements and not announcing
      //      if (strcmp(playingNow.c_str(), "") == 0) {
      if (!audioIsRunning() || changeUrl) {
        if (audioIsRunning()) audioStopSong();
        changeUrl = false;
        // start playing
        audioStartSong();
      }
      //      }
    }
  } else {
    //silence
    //case happens if announcements are queued/playing and silence is activated
    //clear announcements
    announcementCount = 0;
    nextAnnouncement = 0 ;
    announcing = false;
  }
  //=====================HANDLE MQTT AGAIN==================================
  audioloop();
  client.loop();
  //=====================HANDLE CURRENTLY PLAYING ==========================
  if (sendTitle || millis() - title_time > 60000)
  {
    if (audioIsRunning() && !announcing) {
      sendTitle = false;
      title_time = millis();
      //do not use strcat, breaks the sound!
      client.publish("esp/text", (String("Now: " ) + streamTitle).c_str());
      client.publish("espaudio/onair", streamTitle.c_str());
    }
  }
  //=====================HANDLE MQTT AGAIN==================================
  audioloop();
  client.loop();
  //=====================HANDLE BUTTONS=====================================
  /*  if (millis() - run_time > 1000)
    {
      run_time = millis();
      //display logic
      //setDisplayPage(0)
    }*/
  //=====================HANDLE MQTT AGAIN==================================
  audioloop();
  client.loop();
  //=====================HANDLE ENABLED=====================================
  if (millis() - enable_time > 60000)
  {
    enable_time = millis();
    client.publish(TOPIC_ENABLED, (silence ? "0" : "1") , true);
  }
  //=====================HANDLE MQTT AGAIN==================================
  audioloop();
  client.loop();
  //#if defined(PBT) || defined(MFA)
  //=====================HANDLE BUTTONS=====================================
#if defined(PBT) || defined(MFA) || defined(M5ATOM)
  if (millis() - button_time > 300)
  {
#if defined(PBT)
#endif
#if defined(MFA)
    //Button logic
    if (digitalRead(Pin_next) == 0)
    {
      Serial.println("Pin_next");
      //playNext();
    }
    if (digitalRead(Pin_previous) == 0)
    {
      Serial.println("Pin_previous");
      //playPrevious();
    }
    if (digitalRead(Pin_vol_up) == 0)
    {
      Serial.print("Pin_vol_up ");
      uint8_t cv = audio.getVolume();
      if (cv < VOLMAX) cv++;
      audio.setVolume(cv);
      Serial.println(cv);
    }
    if (digitalRead(Pin_vol_down) == 0)
    {
      Serial.println("Pin_vol_down");
      uint8_t cv = audio.getVolume();
      if (cv > VOLMIN) cv--;
      audio.setVolume(cv);
      Serial.println(cv);
    }
    if (digitalRead(Pin_mute) == 0)
    {
      if (vol == VOLMIN) {
        Serial.println("Pin_mute mute");
        vol = audio.getVolume();
        audio.setVolume(VOLMIN);
      } else {
        Serial.println("Pin_mute unmute");
        audio.setVolume(vol);
        vol = VOLMIN;
      }
    }
    if (digitalRead(Pin_pause) == 0)
    {
      Serial.println("Pin_pause");
      paused = !paused;
      audio.pauseResume();
    }
#endif
#ifdef M5ATOM
    if (digitalRead(BTN) == 0)
    {
      Serial.println("Button");
      //start
      if (!audioIsRunning()) {

      } else {

      }
    }

#endif
    button_time = millis();
  }
#endif
  //=====================HANDLE MQTT AGAIN==================================
  client.loop();
  audioloop();
  //=====================HANDLE DISPLAY================================================
#ifdef HAZDISPLAY
  if (millis() - page_time > (3 * 1000))
  {
    currentPage++;
    if (currentPage == pages) {
      currentPage = 0;
    }
    page_time = millis();
    dataUpdate = true;
    setDisplayPage(currentPage);
  }
#endif
  //=====================HANDLE MQTT AGAIN==================================
  client.loop();
  audioloop();
#ifdef ESP32
  //=====================HANDLE SERIAL VOLUME CONTROL==================================
  if (Serial.available())
  {
    String r = Serial.readString();
    r.trim();
    audio.setVolume(r.toInt());
  }
#endif
#ifdef PSP
  processVU();
#endif
}

// optional
void audio_info(const char *info) {
  Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info) { //id3 metadata
  Serial.print("id3data     "); Serial.println(info);
}
void audio_eof_mp3(const char *info) { //end of file
  Serial.print("eof_mp3     "); Serial.println(info);
}
void audio_showstation(const char *info) {
  Serial.print("station     "); Serial.println(info);
  station = String(info);
#ifdef HAZDISPLAY
  dataUpdate = true;
#endif
  sendStation = true;
}
void audio_showstreaminfo(const char *info) {
  Serial.print("streaminfo  "); Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle "); Serial.println(info);
  streamTitle = String(info);
#ifdef HAZDISPLAY
  dataUpdate = true;
#endif
  sendTitle = true;
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     "); Serial.println(info);
}
void audio_commercial(const char *info) { //duration in sec
  Serial.print("commercial  "); Serial.println(info);
}
void audio_icyurl(const char *info) { //homepage
  Serial.print("icyurl      "); Serial.println(info);
}
void audio_lasthost(const char *info) { //stream URL played
  Serial.print("lasthost    "); Serial.println(info);
}
void audio_eof_speech(const char *info) {
  Serial.print("eof_speech  "); Serial.println(info);
}
