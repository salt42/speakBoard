#include "httpRequest.h"


char baseUrl[14] = "192.168.0.234"; // port 8989


void auth() {}
bool registerHook() {
    HTTPClient http;

    http.begin("http://192.168.0.234:8989/hook/register/short/kesgnesgn");
    http.addHeader("hook-address", WiFi.localIP().toString() );
    // Send HTTP GET request
    int httpResponseCode = http.GET();

    String payload = "{}"; 

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    if (payload == "OK") return true;
    return false;
}

bool toggleCat1() {
    HTTPClient http;

    http.begin("http://192.168.0.234:8989/control/pet/117241/toggle");
    // http.addHeader("hook-address", WiFi.localIP().toString() );
    // Send HTTP GET request
    int httpResponseCode = http.GET();

    String payload = "{}"; 

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    if (payload == "OK") return true;
    return false;
}
bool toggleCat2() {
    HTTPClient http;

    http.begin("http://192.168.0.234:8989/control/pet/117242/toggle");
    // http.addHeader("hook-address", WiFi.localIP().toString() );
    // Send HTTP GET request
    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    if (payload == "OK") return true;
    return false;
}
// /lockmode/set/:mode
bool setLockMode(int mode) {
    HTTPClient http;
    char url[55] = "";
    sprintf(url, "http://192.168.0.234:8989/control/lockmode/set/%d", mode);
    http.begin(url);
    http.addHeader("hook-address", WiFi.localIP().toString() );
    // Send HTTP GET request
    int httpResponseCode = http.GET();

    String payload = "{}"; 

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    if (payload == "OK") return true;
    return false;
}

void setup_httpRequest() {
    //auth
}
void loop_httpRequest() {}




void asyncExample() {
    // AsyncClient * aClient = new AsyncClient();
    // if(!aClient)//could not allocate client
    //     return;
        
    // char url[14] = "/control/test";

    // aClient->onError([&aClient](void * arg, AsyncClient * client, err_t error){
    //     Serial.println("Connect Error");
    //     aClient = NULL;
    //     delete client;
    // }, NULL);

    // aClient->onConnect([&aClient, &url](void * arg, AsyncClient * client) {
    //     Serial.println("Connected");
    //     aClient->onError(NULL, NULL);

    //     client->onDisconnect([&aClient](void * arg, AsyncClient * c) {
    //         Serial.println("Disconnected");
    //         aClient = NULL;
    //         delete c;
    //     }, NULL);

    //     client->onData([&aClient](void * arg, AsyncClient * c, void * data, size_t len) {
    //         Serial.print("\r\nData: ");
    //         Serial.println(len);
    //         uint8_t * d = (uint8_t*)data;
    //         for(size_t i=0; i<len;i++)
    //         Serial.write(d[i]);
    //     }, NULL);

        // //send the request
        // client->add("GET ", 5);
        // client->add(url, sizeof(url));
        // client->add(" HTTP/1.0\r\nHost: ", 18);
        // client->add(baseUrl, sizeof(baseUrl));
        // client->add("\r\n\r\n", 5);
        // client->send();


		// char message[130];
		// sprintf(message, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", url, baseUrl);
		// client->add(message, strlen(message));
		// client->send();



        // char request[80];
        // sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", url, baseUrl);
        // String request = String();
        // request += "GET ";
        // request += url;
        // request += " HTTP/1.0\r\nHost: ";
        // request += baseUrl;
        // request += "\r\n\r\n";
        // client->write(request.c_str());
        // client->write(request);
    //     client->write("GET /control/test HTTP/1.0\r\nHost: 192.168.0.234\r\n\r\n");
    // }, NULL);


    // // if(!aClient->connect(baseUrl.c_str(), 80)){
    // if(!aClient->connect(baseUrl, 80)){
    //     Serial.println("Connect Fail");
    //     AsyncClient * client = aClient;
    //     aClient = NULL;
    //     delete client;
    // }
}