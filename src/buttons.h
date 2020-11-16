#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include "AudioFileSourceBuffer.h"



int getButtonIndex(const char* name);
bool playPath(const char* path);
void setup_buttons();
void loop_buttons();