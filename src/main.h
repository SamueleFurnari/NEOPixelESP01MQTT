#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "Animator.h"
#include "Configuration.h"
#include "ConfigStorage.h"

void saveConfigCallback();
void byteToChar(byte *bytes, char *chars, unsigned int count);
void blinkLed();
void onOffNeoPixel(char message);
void setAnimation(char *message);
void reset(char *message);
void callback(char *topic, byte *payload, unsigned int length);
void saveConfig();
void setupWiFiManager();
bool isAuthrorized();
void setupServer();
void setupMqtt();
void reconnectMqtt();
void setupBoard();

#endif