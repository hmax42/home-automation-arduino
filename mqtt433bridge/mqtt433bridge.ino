#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#define BAUD 9600
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

#define WAITTIME 3600000

#define SENDTIMES 10

const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "192.168.1.1";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define BASENAME "/esp/433bridge/10101"
#define BASENAME2 "/esp/433bridge/10100"
#define NETZ1 "10101"
#define NETZ2 "10100"
#define SD_A "10000"
#define SD_B "01000"
#define SD_C "00100"
#define SD_D "00010"
#define PIN_SENDER D4

int netz = -1;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

  char* c = "";
  String topic2 = String(topic);
  if (topic2 == "/esp/433bridge/10101/A") {
    c = SD_A;
    netz = 1;
  }
  else if (topic2 == "/esp/433bridge/10101/B") {
    c = SD_B;
    netz = 1;
  }
  else if (topic2 == "/esp/433bridge/10101/C") {
    c = SD_C;
    netz = 1;
  }
  else if (topic2 == "/esp/433bridge/10101/D") {
    c = SD_D;
    netz = 1;
  }
  else if (topic2 == "/esp/433bridge/10100/A") {
    c = SD_A;
    netz = 2;
  }
  else if (topic2 == "/esp/433bridge/10100/B") {
    c = SD_B;
    netz = 2;
  }
  else if (topic2 == "/esp/433bridge/10100/C") {
    c = SD_C;
    netz = 2;
  }
  else if (topic2 == "/esp/433bridge/10100/D") {
    c = SD_D;
    netz = 2;
  }

  Serial.print("Netz ");
  Serial.print(getNetz(netz));
  Serial.print(" Button ");
  Serial.println(c);

  digitalWrite(BUILTIN_LED, LOW);
  if (c != "") {
    if (payload[1] == 'n') {
      for (int i = 0; i < SENDTIMES; i++) {
        mySwitch.switchOn(getNetz(netz), c);
        delay(10);
      }
      client.publish((topic2 + "/status").c_str(), "on");
    }
    else if (payload[1] == 'f') {
      for (int i = 0; i < SENDTIMES; i++) {
        mySwitch.switchOff(getNetz(netz), c);
        delay(10);
      }
      client.publish((topic2 + "/status").c_str(), "off");
    }
  digitalWrite(BUILTIN_LED, HIGH);
  }
}

char* getNetz(int netz) {
  if (netz == 1)
    return NETZ1;
  else if (netz == 2)
    return NETZ2;
  else
    return NETZ1;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp/433bridge", "clients/presence", 0, false, "esp/433bridge ran away...")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("clients/presence", "a wild esp/433bridge appeared");
      client.subscribe("/esp/433bridge/+/+");
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
  Serial.println("433bridge mqtt");
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  mySwitch.enableTransmit(D4);
  mySwitch.setPulseLength(422);
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
    client.publish("clients/presence", "a wild esp/433bridge is prowling the grounds");
    n = 0;
  } else {
    n++;
  }
  delay(500);
}


