#include <Arduino.h>

// #include <AsyncTCP.h>
#include <HTTPClient.h>


void auth();
bool registerHook();

bool toggleCat1();
bool toggleCat2();
bool setLockMode(int mode);

void setup_httpRequest();
void loop_httpRequest();