// edge.cpp - Fonctions fournies pour la partie « edge » (ESP32) du TP. Ne pas modifier.
#include "edge.h"
#include <sys/time.h>

// ===============================
//      WIFI
// ===============================
void setup_wifi(const char *ssid, const char *password)
{
  delay(10);
  Serial.println();
  Serial.print("Connexion au Wi-Fi : ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int essais = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    essais++;
    if (essais > 40) // 20 secondes
    {
      Serial.println("\nEchec de la connexion Wi-Fi : verifiez credentials.h");
      return;
    }
  }

  Serial.println("\nWi-Fi connecte");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

// ===============================
//      MQTT
// ===============================
void reconnect_Mqtt()
{
  while (!client.connected())
  {
    Serial.print("Connexion au broker MQTT... ");

    // Identifiant unique : deux clients avec le même identifiant se déconnectent mutuellement
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str()))
    {
      Serial.println("OK");
    }
    else
    {
      Serial.print("ECHEC, code = ");
      Serial.print(client.state());
      Serial.println(". Nouvel essai dans 5 s");
      delay(5000);
    }
  }
}

// ===============================
//      HEURE (NTP)
// ===============================
void setupTime()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Synchronisation de l'heure avec le serveur NTP...");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 5000)) // attente maximale : 5 s
  {
    Serial.println("Echec de la synchronisation NTP");
    Serial.println("Entrez le timestamp UNIX actuel en secondes (ex. sur https://www.unixtimestamp.com) :");
    while (!Serial.available())
    {
      delay(100);
    }
    String input = Serial.readStringUntil('\n');

    struct timeval tv;
    tv.tv_sec = (time_t)parseUint64(input);
    tv.tv_usec = 0;
    settimeofday(&tv, NULL); // l'horloge de l'ESP32 est réglée à la main

    getLocalTime(&timeinfo, 0);
    Serial.print("Heure reglee manuellement : ");
  }
  else
  {
    Serial.print("Heure synchronisee : ");
  }
  Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");

  T0NTP = unixMilis();
  T0Milis = millis();
  Serial.print("T0NTP : ");
  Serial.println(T0NTP);
  Serial.print("T0Milis : ");
  Serial.println(T0Milis);
}

// Timestamp UNIX en millisecondes (UTC, indépendant du fuseau horaire)
uint64_t unixMilis()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

uint64_t parseUint64(String s)
{
  s.trim();
  uint64_t result = 0;
  for (unsigned int i = 0; i < s.length(); i++)
  {
    char c = s.charAt(i);
    if (c >= '0' && c <= '9')
    {
      result = result * 10 + (c - '0');
    }
    else
    {
      break;
    }
  }
  return result;
}
