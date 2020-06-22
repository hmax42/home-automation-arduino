#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#define BAUD 9600

#define WAITTIME 3600000
#define HOSTNAME "ESP_IR_BRIDGE"
#define SENDTIMES 10

const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "192.168.1.1";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
#define PIN_SENDER D4
IRsend irsend(PIN_SENDER);  // An IR LED is controlled by GPIO pin 4 (D2)

#define BASENAME "esp/IRbridge"
#define BASENAME_SAMSUNG "esp/IRbridge/samsung"
#define BASENAME_SHERWOOD "esp/IRbridge/sherwood"
#define BASENAME_JVC "esp/IRbridge/jvc"
#define BASENAME_SHARP "esp/IRbridge/sharp"
#define BASENAME_SANYO "esp/IRbridge/sanyo"
#define BASENAME_SONY "esp/IRbridge/sony"
#define BASENAME_PANASONIC "esp/IRbridge/panasonic"
#define BASENAME_NEC "esp/IRbridge/nec"
#define BASENAME_MITSUBISHI "esp/IRbridge/mitsubishi"



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.hostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  digitalWrite(BUILTIN_LED, LOW);

  if (strcmp(BASENAME_SHERWOOD, topic) == 0) {
    Serial.println("Sherwood");
  } else if (strcmp(BASENAME_SAMSUNG, topic) == 0) {
    Serial.println("Samsung");
    irsend.sendSAMSUNG(getUInt64fromHex((char*)payload));
  } else if (strcmp(BASENAME_SONY, topic) == 0) {
    Serial.println("Sony");
    irsend.sendSony(getUInt64fromHex((char*)payload));
  } else if (strcmp(BASENAME_NEC, topic) == 0) {
    Serial.println("NEC");
    irsend.sendNEC(getUInt64fromHex((char*)payload),32);
  } else if (strcmp(BASENAME_MITSUBISHI, topic) == 0) {
    Serial.println("Mitsubishi");
    irsend.sendMitsubishi(getUInt64fromHex((char*)payload));
  } else {
    Serial.println("Else");
  }

  digitalWrite(BUILTIN_LED, HIGH);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
//    if (client.connect("esp/IRbridge", "clients/presence", 0, false, "esp/IRbridge ran away...")) {
    if (client.connect("esp/IRbridge", "irbridge/status", 0, true, "0")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
//      client.publish("clients/presence", "a wild esp/IRbridge appeared");
      client.publish("irbridge/status", "1", true);
      client.subscribe(BASENAME_SAMSUNG);
      client.subscribe(BASENAME_SHERWOOD);
      client.subscribe(BASENAME_NEC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(BAUD);
  Serial.println();
  Serial.println("IRbridge mqtt");
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  irsend.begin();
  digitalWrite(BUILTIN_LED, HIGH);
  setup_wifi();
  digitalWrite(BUILTIN_LED, LOW);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  digitalWrite(BUILTIN_LED, HIGH);
}

int n = 0;
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (n == 3599) {
    client.publish("clients/presence", "a wild esp/IRbridge is prowling the grounds");
    n = 0;
  } else {
    n++;
  }
  delay(500);
}

uint64_t getUInt64fromHex(char const * str)
{
  uint64_t accumulator = 0;
  for (size_t i = 0 ; isxdigit((unsigned char)str[i]) ; ++i)
  {
    char c = str[i];
    accumulator *= 16;
    if (isdigit(c)) /* '0' .. '9'*/
      accumulator += c - '0';
    else if (isupper(c)) /* 'A' .. 'F'*/
      accumulator += c - 'A' + 10;
    else /* 'a' .. 'f'*/
      accumulator += c - 'a' + 10;

  }

  return accumulator;
}
