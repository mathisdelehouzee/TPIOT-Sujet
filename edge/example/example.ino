/*
 * Squelette du programme de l'ESP32 (partie « edge »), voir docs/tp/2-edge.md.
 * Copiez ce dossier (ex. edge/myCode/, en renommant example.ino en myCode.ino), puis :
 *   - complétez credentials.h (Wi-Fi) et conf.h (adresse IP du broker MQTT du fog) ;
 *   - complétez loop() pour publier les mesures du DHT22 au format JSON (schema + payload).
 * En l'état, le programme se connecte puis publie "Hello World !" toutes les 5 secondes
 * sur le topic de test {"test":"string"}.
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <time.h>
#include <stdint.h>
#include "credentials.h"
#include "conf.h"
#include "edge.h"

#define DHTPIN 21     // Pin connecté au pin data du Capteur
#define DHTTYPE DHT22 // DHT 22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

uint64_t T0NTP;
uint64_t T0Milis;

// NTP config
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;     // Adjust for your timezone (e.g., -3600 for GMT-1)
const int daylightOffset_sec = 3600; // Adjust for daylight saving time

void setup() {
  Serial.begin(115200);
  setup_wifi(ssid, password);
  client.setServer(mqtt_server, 1883);
  client.setBufferSize(512); // taille max d'un message MQTT (256 octets par défaut)
  setupTime();
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect_Mqtt();
  }
  client.loop();

  static unsigned long lastPublish = 0;
  if (millis() - lastPublish >= 5000) {
    lastPublish = millis();
    // TODO : lire le capteur et publier UN message par mesure, par exemple :
    //   topic {"temperature":"double","timestamp":"int64"}
    //   message {"schema":{"type":"struct","fields":[
    //              {"type":"double","optional":false,"field":"temperature"},
    //              {"type":"int64","optional":false,"field":"timestamp"}]},
    //            "payload":{"temperature":22.8,"timestamp":<unixMilis()>}}
    //   et de même pour "humidity". Le type « int » n'existe pas dans Kafka Connect.
    Serial.println("Hello World !");
    client.publish("{\"test\":\"string\"}", "Hello World !");
  }
}
