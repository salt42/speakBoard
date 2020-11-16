#include "FS.h"
#include "SPIFFS.h"



bool initalizeCatFS();
void listDir(const char * dirname, uint8_t levels);
void end();
fs::File readFile(const char * path);