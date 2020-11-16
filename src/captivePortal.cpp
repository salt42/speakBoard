#include "captivePortal.h"
#include "httpServer.h"

DNSServer dnsServer;
AsyncWebServer apServer(80);
IPAddress apIP(172, 20, 0, 1);
IPAddress netMsk(255, 255, 255, 0);
bool SoftAccOK  = false;


// class CaptiveRequestHandler : public AsyncWebHandler {
// public:
//   CaptiveRequestHandler() {}
//   virtual ~CaptiveRequestHandler() {}

//   bool canHandle(AsyncWebServerRequest *request){
//     //request->addInterestingHeader("ANY");
//     return true;
//   }

//   void handleRequest(AsyncWebServerRequest *request) {
//     // AsyncResponseStream *response = request->beginResponseStream("text/html");
//     Serial.println("Send index.html");
//     request->send(SPIFFS, "/index.html", "text/html");
//     // response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
//     // response->print("<p>This is out captive portal front page.</p>");
//     // response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
//     // response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
//     // response->print("</body></html>");
//     // request->send(response);
//   }
// };


void setupAP(){
    //start AP with name
    WiFi.disconnect();

//   WiFi.mode(WIFI_AP);
    SoftAccOK = WiFi.softAP("Cat Controller", "catsaregreat");
    delay(1000);
    WiFi.softAPConfig(apIP, apIP, netMsk);

    delay(1000);

    if (SoftAccOK) {
        /* Setup the DNS server redirecting all the domains to the apIP */
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(53, "*", WiFi.softAPIP());
        Serial.println(WiFi.softAPIP());


        apServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            Serial.println("load /");
            //request->send(200, "text/plain", "Hello, world");
            request->send(SPIFFS, "/index.html", "text/html");
        });



        // Send a POST request to <IP>/post with a form field message set to <message>
        apServer.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
            Serial.println("load /save");
            String new_ssid;
            String new_key;
            if (request->hasParam("ssid", true) && request->hasParam("key", true)) {
                new_ssid = request->getParam("ssid", true)->value();
                new_key = request->getParam("key", true)->value();
                //save to eprom
                setCredentials(new_ssid, new_key);
                request->send(200, "text/plain", "Saved! Resart device");
                delay(1000);
                ESP.restart();
            } else {
                request->send(400, "text/plain", "recived wrong");
            }
        });

        //First request will return 0 results unless you start scan from somewhere else (loop/setup)
        //Do not request more often than 3-5 seconds
        apServer.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
            Serial.println("load /scann");
            String json = "[";
            WiFi.scanNetworks();
            int n = WiFi.scanComplete();
            if(n == -2){
                WiFi.scanNetworks(true);
            } else if(n) {
                for (int i = 0; i < n; ++i){
                    if(i) json += ",";
                    json += "{";
                    json += "\"rssi\":"+String(WiFi.RSSI(i));
                    json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
                    json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
                    json += ",\"channel\":"+String(WiFi.channel(i));
                    json += ",\"secure\":"+String(WiFi.encryptionType(i));
                    // json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
                    json += "}";
                }
                WiFi.scanDelete();
                if(WiFi.scanComplete() == -2){
                    WiFi.scanNetworks(true);
                }
            }
            json += "]";
            request->send(200, "application/json", json);
            json = String();
        });

        apServer.onNotFound([] (AsyncWebServerRequest *request) {
            Serial.println("404");
            // request->redirect("/start");
            request->send(404, "text/plain", "Not found");
        });

        apServer.begin();
    } else {
        Serial.println("err: Could not start AP!");
        //@todo handler error -> restart or something stupid
    }

}



void setup_captivePortal() {
    delay(100);
    setupAP();
}

void loop_captivePortal(){
    dnsServer.processNextRequest();
}
