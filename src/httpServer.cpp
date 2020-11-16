//#include <httpServer.h>
//#include "controller.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <Hash.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include <AsyncElegantOTA.h>
#include "captivePortal.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <ArduinoOTA.h>
// #include "SD.h"
#include "SDCard.h"
#include "buttons.h"
// #include "WiFiClient.h"

// const char* ssid = "KDG-ABA04";
// const char* password = "ZBGN0e1hc5UW";

AsyncWebServer sserver(80);
bool captivePortal = false;

struct WiFiConfigData
{
    bool capPortal = true;
    char serverToken[31];
    char serverUrl[63];
    char SSID[25];
    char password[25];
    char ConfigValid[3]; //If Config is Vaild, Tag "TK" is required"
};

WiFiConfigData WiFiConfig;

bool loadCredentials() {
  bool RetValue;
  EEPROM.begin(512);
  EEPROM.get(0, WiFiConfig);
  EEPROM.end();
  if (String(WiFiConfig.ConfigValid) = String("TK")) {
    RetValue = true;
  } else {
    RetValue = false; // WLAN Settings not found.
  }
  return RetValue;
}

bool saveCredentials() {
    EEPROM.begin(512);
    for (int i = 0 ; i < sizeof(WiFiConfig) ; i++) {
        EEPROM.write(i, 0);
    }
    strncpy( WiFiConfig.ConfigValid , "TK", sizeof(WiFiConfig.ConfigValid) );
    EEPROM.put(0, WiFiConfig);
    EEPROM.commit();
    EEPROM.end();
    return true;
}

void setCredentials(String ssid, String pass) {
    ssid.toCharArray(WiFiConfig.SSID, 25);
    pass.toCharArray(WiFiConfig.password, 25);
    WiFiConfig.capPortal = false;
    saveCredentials();
}

void setupOTA() {

    ArduinoOTA.onStart([]() {
        SPIFFS.end();
        //@todo close sd card handle
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if(!index) {
        // open the file on first call and store the file handle in the request object
        String path = "/" + filename;
        request->_tempFile = SD.open(path, "w");
    }
    if(len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
    }
    if(final) {
        // close the file handle as the upload is now done
        request->_tempFile.close();
        // request->send(200, "text/plain", "File Uploaded !");
        request->send(SPIFFS, "/files.html");
    }
}

void setup_httpsServer(void) {
    //load config
    Serial.println("start server");
    
    bool configOK = loadCredentials();
    byte len;
    
    Serial.println(WiFiConfig.SSID);
    Serial.println(WiFiConfig.password);
    
    WiFi.setHostname("SpeakBoard");
    if ((configOK && WiFiConfig.capPortal == true) || !configOK ) {
        captivePortal = true;
        Serial.println("Start captivePortal");
        setup_captivePortal();
    } else {
        captivePortal = false;
        len = strlen(WiFiConfig.SSID);
        WiFiConfig.SSID[len + 1] = '\0';

        len = strlen(WiFiConfig.password);
        WiFiConfig.password[len + 1] = '\0';

        len = strlen(WiFiConfig.serverToken);
        WiFiConfig.serverToken[len + 1] = '\0';

        len = strlen(WiFiConfig.serverUrl);
        WiFiConfig.serverUrl[len + 1] = '\0';


        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        
        WiFi.begin(WiFiConfig.SSID, WiFiConfig.password);
        wl_status_t status = WiFi.status();

        Serial.println(status);

        Serial.println("");
        Serial.println("Try Connecting");
        unsigned long startTime = millis();
        while (status != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            status = WiFi.status();
            if (millis() - startTime > 1000 * 20 || status == WL_NO_SSID_AVAIL || status == WL_CONNECT_FAILED) {
                switch (status) {
                    case WL_NO_SSID_AVAIL:
                        Serial.println("SSID not correct!");
                    case WL_CONNECT_FAILED:
                        Serial.println("Can't connect to WIFI!");
                }
                //restart in ap mode
                captivePortal = true;
                Serial.println("Start captivePortal");
                WiFi.disconnect();
                setup_captivePortal();
                return;
            }
            Serial.println(status);
        }
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(WiFiConfig.SSID);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());


        sserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/files.html");
        });
        
        sserver.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
            // request->send(SPIFFS, "/files.html");
            AsyncResponseStream *response = request->beginResponseStream("text/json");
            response->print("{\"files\": [");

            File root = SD.open("/");
            if(!root){
                Serial.println("Failed to open directory");
                return;
            }

            File file = root.openNextFile();
            bool first = true;
            while(file){
                if(file.isDirectory()){

                } else {
                    if (!first) {
                        response->printf(",");
                    }
                    response->printf("{\"name\": \"%s\", \"button\": %d}", file.name(), getButtonIndex(file.name()));
                }
                file = root.openNextFile();
                first = false;
            }


            // response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
            response->print("]}");
            request->send(response);
        });

        sserver.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
            if (request->hasParam("button") && request->hasParam("data")) {
                AsyncWebParameter* data = request->getParam("data");
                AsyncWebParameter* p = request->getParam("button");
                // p->value();//button number
                // data->value();//file name
                Serial.printf("new File [%s] to button [%s] \n", p->value(), data->value());
            }
        }, handleUpload);

        sserver.on("/play", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (request->hasParam("path")) {
                AsyncWebParameter* path = request->getParam("path");
                if (playPath(path->value().c_str()) ) {
                    // Serial.printf("play [%s] \n", path->value());
                    request->send(200, "text/plain", "OK");
                } else {
                    request->send(404, "text/plain", "File not found!");
                }
            }
        });

        // sserver.on("/shortStatus", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
        //     StaticJsonDocument<100> doc;
        //     DeserializationError err = deserializeJson(doc, data);
        //     if (err) {
        //         Serial.print(F("deserializeJson() failed with code "));
        //         Serial.println( err.c_str() );
        //     }
        //     int lockMode = doc["lockMode"];
        //     int cat1 = doc["cat1"]["where"];
        //     int cat2 = doc["cat2"]["where"];
        //     updateStatus(cat1, cat2, lockMode);
        //     request->send(200, "text/plain", "OK");
        // });

        // AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/shortStatus", [](AsyncWebServerRequest *request, JsonVariant &json) {
        //     JsonObject& jsonObj = json.as<JsonObject>();
        //     // ...
        //     request->send(200, "text/plain", "OK");
        // });
        // sserver.addHandler(handler);


        sserver.begin();
        Serial.println("HTTP server started");

        setupOTA();
    }
}

void loop_httpsServer(void) {
    if (captivePortal) {
        loop_captivePortal();
    } else {
        ArduinoOTA.handle();
        //AsyncElegantOTA.loop();
    }
}