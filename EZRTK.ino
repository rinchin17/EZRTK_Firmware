#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <HardwareSerial.h>
#include "FS.h"
#include <SD.h>
#include "OneButton.h"
#include <TinyGPS++.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define chipSelect 5

// For NMEA storage 
String globe = "Error: PX1122R Module not connected.";
String str0="";
String str1="";

// For Ip storage
String WiFiIp = "";
String hotspotrecv="";
String hotspotssid;

const unsigned long eventInterval = 2000;
const unsigned long wifiInterval = 7000;
const unsigned long newInterval = 3000;
const unsigned long baseInterval= 10000;
unsigned long previousTime = 0;
unsigned long wifipreviousTime = 0;
unsigned long newpreviousTime = 0;
unsigned long basepreviousTime = 0;

int ck=0;
int timer=0;
int hotspotcmd=0;
int i, SessionMode=0;
int temp;
int ipReturned = 0;
int device_mode;
int flag =0; 
int winflag4=0;
int winflag5=0;
int winflag6=0;
int winflag7=0;
int winflag8=0;
int winflag9=0;
int winflag10=0;

char* ssid;
char* password = "1234567890";
char ssid1[30], password1[30];


String ClickStatus="";
String fileName;

//  Object declarations
Adafruit_SSD1306 display(128, 64, &Wire, -1);
AsyncWebServer server(80);
HardwareSerial LoRa(1);
HardwareSerial RTK(2);
File myFile;
OneButton button(15, true);
TinyGPSPlus gps;
TinyGPSCustom dt(gps, "GNRMC", 9);
TinyGPSCustom fix(gps, "GNGSA", 2);
TinyGPSCustom fixStat(gps, "GPGGA", 6);

// Function declarations
void SD_Card_Setup();
void createJsonFile();
void logJson();
void singleclick();
void doubleclick();
void longclick();

// Bitmap of splash Image
const unsigned char MadeInINDIA [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xf9, 0xe7, 0x3c, 0x38, 0x39, 0xd9, 0x20, 0x4c, 0xe0, 0x01, 0xc9, 0xdf, 0x37, 0x21, 0xe7, 0x3f, 
  0xf8, 0xc7, 0x18, 0x18, 0x38, 0x88, 0x00, 0x0c, 0x60, 0x01, 0x88, 0x8f, 0x23, 0x20, 0x67, 0x1f, 
  0xf8, 0xc6, 0x18, 0x08, 0x78, 0x89, 0x10, 0x4c, 0x60, 0x01, 0x88, 0x0f, 0x20, 0x20, 0x66, 0x1f, 
  0xf8, 0x06, 0x08, 0xc8, 0x7c, 0x01, 0x18, 0xc0, 0x60, 0x01, 0x88, 0x0f, 0x20, 0x26, 0x26, 0x0f, 
  0xf8, 0x06, 0x08, 0xc8, 0x3c, 0x01, 0x18, 0xc0, 0x70, 0x03, 0x88, 0x0f, 0x20, 0x26, 0x26, 0x0f, 
  0xf8, 0x04, 0x08, 0xc9, 0xfc, 0x03, 0x18, 0xcc, 0x70, 0x03, 0x89, 0x0f, 0x24, 0x26, 0x24, 0x0f, 
  0xf9, 0x24, 0x00, 0x08, 0x3e, 0x23, 0x18, 0xcc, 0x78, 0x07, 0x89, 0x8f, 0x26, 0x20, 0x64, 0x07, 
  0xf9, 0xe4, 0xe4, 0x18, 0x3e, 0x23, 0x39, 0xcc, 0x7c, 0x0f, 0xc9, 0x8f, 0x26, 0x20, 0xe4, 0xe7, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// File System
String listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    String files = "";

    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return "failed to open directory";
    }
    if(!root.isDirectory()){
        //Serial.println(" - not a directory");
        return "not a directory";;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            files = String(files + file.name() + " ");
        }
        file = root.openNextFile();
    }

  return files;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, String path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

//  IP Address to String Converter

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +
         String(ipAddress[1]) + String(".") +
         String(ipAddress[2]) + String(".") +
         String(ipAddress[3]);
}

String commandPallete(String command){
  int strIndex=0;
  String SSID1, PASS;
  
  if(command.startsWith("$EZ_RTK,SET-WIFI,", 0))
  {
    Serial.println("Command received from hotspot : "+command);
    strIndex=command.lastIndexOf(',');
    SSID1=command.substring(17, strIndex);
    PASS=command.substring(strIndex+1);
    
    Serial.println("SSID="+SSID1);//String value parsed
    Serial.println("Password="+PASS);//String value parsed
    
    SSID1.toCharArray(ssid1, SSID1.length()+1);
    PASS.toCharArray(password1, PASS.length()+1);

    //SSID and Password broken into character array from string and then printed below
    
    Serial.println("Thanks for using EZ_RTK, your settings for Wifi has been configured as follows:\nSSID :"+String(ssid1)+"."+"\nPassword :"+String(password1)+".");
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Wifi already connected");
      WiFi.disconnect();
      delay(100);
    }

    while (WiFi.status() != WL_CONNECTED) 
    {
      Serial.println("'"+String(ssid1)+"'");
      WiFi.begin(ssid1, password1);
      delay(1000);
      timer++;

      Serial.print(".");
      
      if (timer==4)  //try reconnecting wifi for consecutively 5 attempts, 5 seconds in total
      {
        Serial.println("\nTimeout no, wifi connected!\n\n");
        timer=0;
        String("").toCharArray(ssid1, String("").length()+1);;
        break;
      }
    }
    
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi connection estabilished with "+String(ssid1));
      Serial.println("IP Address: ");
      Serial.println(WiFi.localIP());

      WiFiIp = IpAddress2String(WiFi.localIP());
      ipReturned = 1;
      
      winflag10=1;  //variable to wake the window in void loop
      hostServer();
      Serial.println("Server hosted");
      Serial.println();
    }
    command="";
    return WiFiIp;
  }
  else{
    Serial.println("Invalid command. Please Check your command : "+command);
  }
}


void setup() {
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.drawBitmap(0, 0,  MadeInINDIA, 128, 50, WHITE);
  display.display();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,55);
  display.println("Welcome to the EZ-RTK");
  display.setTextColor(BLACK, WHITE);
  display.print("   ");
  display.display();

  for (int i=0; i<3; i++)
  {
    display.setTextColor(BLACK, WHITE);
    display.print("    ");
    delay(1400);
    display.display();
  }
  
  display.setTextColor(WHITE);
  display.clearDisplay();
  
  SD_Card_Setup();

  readMode();
  
  if(device_mode == 1){
    RoverSetup();
  }
  else if(device_mode == 0){
    BaseSetup();
  }

  WiFi.mode(WIFI_MODE_APSTA);
  Serial.println("Setting AP However you can connect as Station as well…");

  //Wifi hotspot
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  hostServer();
  
  LoRa.begin(115200, SERIAL_8N1, 16, 17); //rx,tx
  RTK.begin(115200, SERIAL_8N1, 2, 4);  //rx,tx  
}

void loop() {
  button.tick();

  unsigned long wificurrentTime = millis();
  
  if(device_mode==1){

    if(LoRa.available()>0){
        RTK.write(LoRa.read());    
    }

    if(RTK.available()>0){
      temp = RTK.read();
      Serial.print((char)temp);
      
      gps.encode(temp);
  
      unsigned long currentTime = millis();
      
      if(currentTime - previousTime >= eventInterval){
          if(ck==0){
            ck = 1;
            globe = str0;
            str0 = "";
          }
          else if(ck==1){
            ck = 0;
            globe = str1;
            str1 = "";
          }
          previousTime = currentTime;
      }
      else{
        if(ck == 0){
          str0 += (char)temp;  
        }
        else{
          str1 += (char)temp;  
        }
      } 
    }
    
    unsigned long newcurrentTime = millis();
    
    if(newcurrentTime - newpreviousTime >= newInterval){
      RoverMain();
      newpreviousTime = newcurrentTime;
    }
    
  }
  else if(device_mode==0){
    globe="";
    if(RTK.available()>0){
      temp = RTK.read();
      Serial.print((char)temp);
      LoRa.write(temp);
    }
  }

  parser();  
}

void SD_Card_Setup()
{
  while (!Serial);

  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) 
  {
    Serial.println("SD card initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset or reopen this serial monitor after fixing your issue!");

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
  
    display.println("      CHECK SD CARD\n");
    display.setTextSize(1);

    display.println("  Failed to mount");
    display.println("  Memory device.");
    display.println("  Please check");
    display.println("  SD Card and");
    display.println("  restart the device.");
    
    display.display();
    while (true);
  }
  Serial.println("SD card initialised!");

  uint8_t cardType = SD.cardType();
  
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
}

void createJsonFile()
{
  fileName="/EZ_RTK_"+String(dt.value())+"_"+String(gps.time.value())+".json";
  Serial.println("Surveying session STARTED!\n");
  winflag6=1;  //variable to wake the window in for loop
  
  myFile = SD.open(fileName, FILE_WRITE);
  
  if (myFile) 
  {
    Serial.println("Succesfully created the file\n");
    Serial.println("Your file name is : "+String(fileName));
  } 
  
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("No file to open.....");
  }
  
  if (!SD.exists(fileName))
    Serial.println("NO SUCH FILE FOUND");
}

void logJson()
{
  //starting logging JSON
  Serial.println("Opening file for appendng new data!!");
  
  myFile = SD.open(fileName,FILE_APPEND);
  
  if(myFile)
  {
    Serial.println("File opened for appending new data");
    myFile.println ("{\"Time\" : " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second())+",");
    myFile.println ("\"Date\" : " + String(dt.value())+",");
    myFile.println ("\"Latitude\" : "+String(gps.location.lat())+",");
    myFile.println ("\"Longitude\" : "+String(gps.location.lng())+",");
    myFile.println ("\"Speed\" : "+String(gps.speed.kmph())+",");
    myFile.println ("\"Altitude\" : "+String(gps.altitude.meters())+",");
    myFile.println ("\"Sattelites\" : "+String(gps.satellites.value())+",");
    myFile.println ("\"HDOP\" : "+String(gps.hdop.hdop()) + "},");
    Serial.println("Data logged successfully...\n\n");
    myFile.close();
    winflag4=1; //variable to wake the window in void loop
  }
  else 
  {
    Serial.println("\nPlease start a session before logging the data!!!");
    winflag5=1; //variable to wake the window in void loop
  }
}

void doubleclick()                                  // what happens when button is double-pressed
{
  Serial.println("\n\nDouble click - Data to be logged"); 
  ClickStatus = "DoubleClick";
  logJson();
} 
 
void singleclick()                                  // what happens when the button is clicked
{                                 
  Serial.println("Single click - window to be changed");
  
  flag++;
  if (flag>=3)
  {
    flag=0;
  }                                 
}
 
void longclick()                                    // what happens when button is long-pressed
{                                   
  Serial.print("\n\nLong click - ");
  ClickStatus="LongClick";
  
  SessionMode=!SessionMode;
  
  if (SessionMode==1)
  {
    createJsonFile();                       
  }
  if (SessionMode==0)
  {
    if (SD.exists(fileName)){
      Serial.println("Surveying session TERMINATED\nYour data logged inside file : "+String(fileName));
      fileName="";
      winflag7=1;//variable to wake the window in void loop
    }
    else 
    {
      Serial.println("Surveying session Terminated \nNo file found."); 
    }
  }
}

void hostServer()
{
  server.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      request->send(200, "text/plain", "ESP 32 web server");
  });

  server.on("/files", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      String files = listDir(SD, "/", 0); //list files in directory
      request->send(200, "text/plain", files);
  });

  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      int fnsstart = request->url().lastIndexOf('/');
      String fn = request->url().substring(fnsstart);
      request->send(SD, fn, String(), true);
  });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      int fnsstart = request->url().lastIndexOf('/');
      String fn = request->url().substring(fnsstart);
      deleteFile(SD, fn);
      String files = listDir(SD, "/", 0);
      request->send(200, "text/plain", files); //send back remaining files
  }); 
  
  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request){
    int fnsstart = request->url().indexOf('$');
    hotspotrecv = request->url().substring(fnsstart, request->url().length());

    hotspotcmd=1;
    Serial.println("ipReturned = "+String(ipReturned));
    
    request->send(200, "text/plain", WiFiIp);
    WiFiIp = "";    
  });

  server.on("/connect", HTTP_GET, [](AsyncWebServerRequest *request){
    int fnsstart = request->url().indexOf('$');
    hotspotrecv = request->url().substring(fnsstart, request->url().length());
    WiFiIp = commandPallete(hotspotrecv);
    
    Serial.println("ipReturned = "+String(ipReturned));

    if(ipReturned == 0){
        request->send(200, "text/plain", "Incorrect Wi-FI Credentials. Please Check!");
    }
    
    else if(ipReturned == 1){
      ipReturned = 0;
      Serial.println("got ip: "+WiFiIp);
      request->send(200, "text/plain", WiFiIp);
      WiFiIp = "";    
    }
    
  });

  server.on("/init", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response;
    if (WiFi.status() == WL_CONNECTED)
    {
      response = String(ssid1) + ":" + IpAddress2String(WiFi.localIP());
    }
    else
    {
      response = "none";
    }
    Serial.println("Sending: "+ response);
    request->send(200, "text/plain", response);
  });

  server.on("/livedata", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", globe);
  });
}

void parser()
{
  char c;
  String command;

  while (Serial.available()) 
  {
    c = Serial.read();  //gets one byte from serial buffer
    command += c; //makes the String command
    delay(8);  //slow looping to allow buffer to fill with next character
  }
  while (hotspotcmd==1)
  {
    command=hotspotrecv;
    hotspotcmd=0;
  }

  if (command.length() > 0) 
  {
    if(command.equals("$EZ_RTK,SET-MODE,ROVER\n") || command.equals("$EZ_RTK,SET-MODE,ROVER"))
    {
      writeMode(1);
      flag = 30;
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a ROVER ");
      window12();
      command = "";
    }
    
    else if(command.equals("$EZ_RTK,SET-MODE,BASE\n") || command.equals("$EZ_RTK,SET-MODE,BASE"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a BASE");
      flag = 30;
      writeMode(0);
      winflag9=1;//variable to wake the window in void loop
      window13();
      winflag9=0;
      command = "";
    }
    
    else if(command.equals("$EZ_RTK,GNSS-VIEWER,RTKCOMMAND\n") || command.equals("$EZ_RTK,GNSS-VIEWER,RTKCOMMAND"))
    {
      Serial.println("CLOSE THE SOFTWARE AND CONNECT PORT AT 115200 BAUD IN GNSS VIEWER TO CONFIGURE RTK");
      command="";
//      gnssflag =!gnssflag;
      
//      GNSScomm(); //transfers gnss viewer command to 
    }
    
    else if(command.startsWith("$EZ_RTK,SET-WIFI,", 0)){
      commandPallete(command);
    }
    
//    else
//    {
//      Serial.println("Invalid command. Please Check your command : "+command);
//    }
  }
}

void readMode()
{
  File myFile1 = SD.open("/ConfigMode.txt", FILE_READ);
  device_mode = (int)(myFile1.read());
  myFile1.close();

  File hotspotFile = SD.open("/HotspotCreds.txt", FILE_READ);
  
  while(hotspotFile.available()){
    hotspotssid += (char)hotspotFile.read();
  }
  
  int j = hotspotssid.length()+1;
  ssid = (char *)malloc(sizeof(char)*j);

  for(int i = 0; i<hotspotssid.length(); i++)
    *(ssid+i) = (char)hotspotssid[i];
  
  *(ssid+(j-1)) = '\0';
  hotspotFile.close();
  
  Serial.println("Hotspot ssid = "+String(ssid));
}

void writeMode(int newMode)
{
  File myFile2 = SD.open("/ConfigMode.txt", FILE_WRITE);
  if(myFile2)
  {
    myFile2.write(newMode);
    Serial.println("ConfigFile created");
  }
  else
  {
    Serial.println("Couldn't create configFile");
  }  
  myFile2.close();
}

void window1() //sattelite window
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print(" ");
  display.setTextSize(2);
  display.println("SATTELITE");
  display.setTextSize(1);
  
  display.print("\n FIX : ");

  if (fixStat.isUpdated())
  {
    int fixStatInt = atoi(fixStat.value());
  
    switch (fixStatInt)
    {
    case 0:
      display.print("Fix Unavailabe");  
      break;
    case 1:
      display.print("SPS, 3D");
      break;
    case 2:
      display.print("DGPS");
      break;
    case 3:
      display.print("PPS");
      break;
    case 4:
      display.print("RTK");
      break;
    case 5:
      display.print("FLOAT");
      break;
    case 6:
      display.print("Est");
      break;
    case 7:
      display.print("Manual");
      break;
    case 8:
      display.print("Simul");
      break;
    default:
      display.print("N/A");
      break;
    }
  }
      
  display.print("\n LAT : ");
  display.println(gps.location.lat(), 6);
  display.print(" LON : ");
  display.println(gps.location.lng(), 6);
  display.print(" TIME : ");
  if (gps.time.hour() < 10) display.print("0");
  display.print(gps.time.hour());
  display.print(":");
  if (gps.time.minute() < 10) display.print("0");
  display.print(gps.time.minute());
  display.print(":");
  if (gps.time.second() < 10) display.print("0");
  display.println(gps.time.second());
  display.print(" DATE : ");
  display.print(gps.date.day());
  display.print("/");
  display.print(gps.date.month());
  display.print("/");
  display.println(gps.date.year());
  display.display();
}

void window2() //IMU  window
{
//    sensors_event_t a, g, temp;
//    mpu.getEvent(&a, &g, &temp);  

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.println("   IMU   ");
    
    display.setTextSize(1);
    
    display.print("\n    X : ");
//    display.println(g.gyro.x);
    
    display.print("    Y : ");
//    display.println(g.gyro.y);
    
    display.print("    Z : ");
//    display.println(g.gyro.z);
    
    display.display();
}
void window3() //wifi window
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("   WIFI   ");
  display.setTextSize(1);
  display.print("\nHotspot : ");
  display.println("ACTIVE");
  display.print("SSID : ");
  display.println(ssid1);
  display.print("IP : ");
  display.println(WiFi.localIP());
  
  display.display();
}
void window4() //to be flashed when surveying data logged
{
  display.clearDisplay();
  display.setCursor(0,20);
  display.setTextSize(2);
  display.println(" SUCCESS");
  display.setTextSize(1);
  display.print("\n   DATA LOGGED!");
  display.display();
}
void window5()// to be printed when invalid data logging is made
{
  display.clearDisplay();
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Please start session before logging data !");
  display.display();
}

void window6() //to be printed when session starts
{
  display.clearDisplay();
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Surveying session\nSTARTED!");
  display.display();
}
void window7() //to be printed when session terminated
{
  display.clearDisplay();
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Surveying session\nTERMINATED!");
  display.display();
}
void window8() //flashes configured as rover
{
  display.clearDisplay();
  display.setCursor(0,14);
  display.setTextSize(1);
  display.println("  Configured as");
  display.setTextSize(2);
  display.println(" ROVER");
  display.display();
}
void window9() //flashes configured as base
{
  display.clearDisplay();
  display.setCursor(0,14);
  display.setTextSize(1);
  display.println("  Configured as");
  display.setTextSize(2);
  display.println(" BASE ");
  display.display();
}
void window10()//ssid and password display
{
  display.clearDisplay();
  display.setCursor(0,14);
  display.setTextSize(1);
  display.println(" Connected to");
  display.println(ssid1);
  display.display();
}

void window11() //for gnss viewer command window
{
  display.clearDisplay();
  display.setCursor(0,28);
  display.setTextSize(1);
  display.println("  Looking for GNSS commands");
  display.setTextSize(2);
  display.println("  ... ");
  display.display();
}

void window12()   //  
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);

  display.println("      RESET NEEDED\n");
  display.setTextSize(1);
  
  display.println("  Mode Set as ROVER");
  display.println("  Please restart your");
  display.println("  device to get it");
  display.print("  into effect.");

  display.display();
}

void window13()   //  
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);

  display.println("      RESET NEEDED\n");
  display.setTextSize(1);
    
  display.println("  Mode Set as BASE");
  display.println("  Please restart your");
  display.println("  device to get it");
  display.print("  into effect.");


  display.display();
}
void BaseSetup()
{
  window9();
}

void RoverSetup(){
  if(!SD.begin(chipSelect))
  {
    SD.begin(chipSelect);
  }
  
  window8();
  
//  listDir(SD, "/", 0);

  button.attachDoubleClick(doubleclick);            // link the function to be called on a doubleclick event.
  button.attachClick(singleclick);                  // link the function to be called on a singleclick event.
  button.attachLongPressStop(longclick);            // link the function to be called on a longclick event.
}

void BaseMain()
{
  window9();
}

void RoverMain(){
  // windows
  if (flag==0)//opens satellite window
  {
    window1(); 
  }
  if (flag==1)//opens IMU window
  {
    window2();
  }
  if (flag==2)//opens wifi configuration window
  {
    window3();
  }
  if (winflag4==1)//splashes "data logged"
  {
    winflag4=0;
    window4();
//    delay(1000);
  }
  if (winflag5==1)//splashes "Please start a session before logging data"
  {
    window5();
//    delay(1000);
    winflag5=0;
  }
  if (winflag6==1)//splashes "session started"
  {
    window6();
//    delay(1000);
    winflag6=0;
  }
  if (winflag7==1)//splashes "session terminated"
  {
    window7();
//    delay(1000);
    winflag7=0;
  }
  if (winflag8==1)//splashes "session terminated"
  {
    window8();
//    delay(1000);
    winflag8=0;
  }
  if (winflag9==1)//splashes "session terminated"
  {
    window9();
//    delay(1000);
    winflag9=0;
  }
  if (winflag10==1)//splashes "session terminated"
  {
    window10();
//    delay(1000);
    winflag10=0;
  }
}
