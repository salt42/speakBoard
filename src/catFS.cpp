#include "catFS.h"


#define FORMAT_SPIFFS_IF_FAILED true


bool initalizeCatFS() {
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return false;
    }

    Serial.println("List files");
    listDir("/", 0);
    //   readFile("/index.html");
    return true;
}

void listDir(const char * dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = SPIFFS.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

fs::File readFile(const char * path) {
    Serial.printf("Reading file: %s\r\n", path);

    File file = SPIFFS.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        // return NULL;
    }

    Serial.println("- read from file:");

    while(file.available()){
        Serial.write(file.read());
    }
    return file;
}

void end() {
    SPIFFS.end();
}


// void writeFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Writing file: %s\r\n", path);

//     File file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("- failed to open file for writing");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("- file written");
//     } else {
//         Serial.println("- frite failed");
//     }
// }

// void appendFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Appending to file: %s\r\n", path);

//     File file = fs.open(path, FILE_APPEND);
//     if(!file){
//         Serial.println("- failed to open file for appending");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("- message appended");
//     } else {
//         Serial.println("- append failed");
//     }
// }

// void renameFile(fs::FS &fs, const char * path1, const char * path2){
//     Serial.printf("Renaming file %s to %s\r\n", path1, path2);
//     if (fs.rename(path1, path2)) {
//         Serial.println("- file renamed");
//     } else {
//         Serial.println("- rename failed");
//     }
// }

// void deleteFile(fs::FS &fs, const char * path){
//     Serial.printf("Deleting file: %s\r\n", path);
//     if(fs.remove(path)){
//         Serial.println("- file deleted");
//     } else {
//         Serial.println("- delete failed");
//     }
// }
