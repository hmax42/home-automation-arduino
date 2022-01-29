#ifdef ESP32
void setupOta() {
  //start OTA
  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(HOSTNAME);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("OTA: Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nOTA: End");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    ;
    Serial.printf("Error[%u]: ", error);
    //if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    //else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    //else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    //else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    //else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}
#endif

void setupWifi()
{
  // We start by connecting to a WiFi network
  Serial.print("WIFI: Connecting to ");
  Serial.println(ssid);
  WiFi.mode( WIFI_STA );
#ifndef ESP32
  WiFi.hostname(HOSTNAME);
#endif
  WiFi.begin(ssid, password);
#ifdef ESP32
  WiFi.setHostname(HOSTNAME);
#endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi: connected");
  Serial.print("WiFi: IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("MQTT: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(HOSTNAME, TOPIC_STATUS , 0, true, "0")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC_STATUS , "1", true);
      client.publish(TOPIC_IP  , WiFi.localIP().toString().c_str(), true);
      for (int i = 0; i < NUM; i++) {
        Serial.print("MQTT: Subscribe to ... ");
        Serial.println(topics[i]);
        client.subscribe(topics[i]);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte * payload, unsigned int length) {
  //#ifdef DETAIL
  Serial.print("MQTT: Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  //#endif

  for (int i = 0; i < NUM; i++) {
    if (strcmp(topic, topics[i]) == 0 ) {
      if (i == 0) {
        if (silence) return;
        // announcement
        if ( MAX_ANNOUNCEMENTS == announcementCount ) {
          //full
          return;
        }
        announcements[(nextAnnouncement + announcementCount) % MAX_ANNOUNCEMENTS] = String((char*)payload).substring(0, length);
        announcementCount++;
        //do not abort current running stuff, switch to announcements happens in main loop
      } else if (i == 1) {
        //if (silence) return;
        // url
        playingNow = String((char*)payload).substring(0, length);
        if (strcmp(playingNow.c_str(), "null") == 0) {
          playingNow = "";
        }
        //changeUrl = true;
        changeUrl = !silence;
        //no need to stop current stream, main loop will start the new one
        break;
      } else if (i == 2) {
        // show help
        if (length > 3) {
          for (int h = 0; h < cmndSize; h++) {
            if (memcmp(payload, cmnd[h], 4) == 0) {
              if (h == 0) {
                //write help
                for (int s = 0; s < cmndSize; s++) {
                  client.publish("espaudio/cmnd/help", cmnd[s]);
                }
              } else if (h == 1) {
                if (!announcing) {
                  audioStopSong();
                }
                playingNow = "";
              }
            } else {
              //unknown command
            }
          }
        }
        break;
      } else if (i == 3) {
        // enable = !silence
        if (payload[0] == '0') {
          silence = true;
          audioStopSong();
          //playingNow = "";
        } else if (payload[0] == '1') {
          audioStartSong();
          announcements[(nextAnnouncement + announcementCount) % MAX_ANNOUNCEMENTS] = String("http://192.168.7.195:8123/local/fzero_start.mp3");
          announcementCount++;
          silence = false;
        } else {
          Serial.println("Payload ignored");
        }
        //send update in next loop
        enable_time = millis() - 60000;
        break;
      } else if (i == 4) {
        // volume
        // 6 global
        // 7 local

        break;
      }
    }
  }
}
