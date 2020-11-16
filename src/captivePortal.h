#include <DNSServer.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
#include "catFS.h"

void setupAP();
void setup_captivePortal();
void loop_captivePortal();
void notFound(AsyncWebServerRequest *request);