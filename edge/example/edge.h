// edge.h - Fonctions fournies pour la partie « edge » (ESP32) du TP. Ne pas modifier.
#ifndef EDGE_H
#define EDGE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <stdint.h>

void setup_wifi(const char *ssid, const char *password); // Initialisation du Wi-Fi
void reconnect_Mqtt();                                   // Connexion ou reconnexion à MQTT
void setupTime();                                        // Initialisation de l'heure (NTP ou saisie manuelle)
uint64_t unixMilis();                                    // Timestamp UNIX actuel en millisecondes
uint64_t parseUint64(String s);                          // Transforme une chaîne de caractères en uint64_t

// Variables à définir dans votre fichier .ino (voir le début de code imposé dans la documentation)
extern PubSubClient client;
extern const char *ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;
extern uint64_t T0NTP;    // timestamp UNIX (ms) au moment de la synchronisation
extern uint64_t T0Milis;  // valeur de millis() au moment de la synchronisation

#endif // EDGE_H
