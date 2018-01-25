/*
 Simple test for MxConfig(Modtronix Configuration) library.
 Uses single "Modtronix Configuration" Structure, located in "AppConfig" files
*/


#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>         //Required for PSTR define on ESP8266
#endif

//Use MxConfig library, also includes SPIFFS files
#include "MxConfig.h"

//Include Files
#include "AppConfig.h"


void setup() {
  uint32_t tMillis1, tMillis2;
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

  //Enable debugging for "Modtronix Config"
  mxConfSetDebugLevel(3, &Serial);

  //Initialize SPIFFS
  //For ESP8266, AppConfig is stored in SPIFFS File System
#if defined(ESP8266) || defined(ESP32)
  SPIFFS.begin();

  //Test is Flash is formatted. Format if not!
  mxConfCheckFormat();
#endif

  //Set default values for AppConf structure
  Serial.println(F("Set to default values"));
  appConfSetDefaults();
  MxConfig<AppConfig> appConfObj(&appConf);
  tMillis1 = millis();
    if (appConfObj.load("appConf")) {
    tMillis2 = millis();
    Serial.print(F("Successfully loaded config in "));
    Serial.print(tMillis2 - tMillis1);
    Serial.println(F("ms"));
  }
  else {
    Serial.println(F("ERROR loading config!"));
  }

  // TEST TEST
  //appConfSetDefaults();

  //Print current AppConfig
  Serial.println(F("AppConfig Vars:"));
  Serial.print(F("b.testU8: "));
  Serial.println(appConf.b.testU8);
  Serial.print(F("w.testU16: "));
  Serial.println(appConf.w.testU16);
  Serial.print(F("w.testU32: "));
  Serial.println(appConf.w.testU32);
  Serial.print(F("dw.testU32: "));
  Serial.println(appConf.dw.testU32);
}


/**
* Main Loop
*/
void loop() {
  static uint32_t tPreviousMillis = 0;
  static uint8_t ledState;
  uint32_t tCurrentMillis;
  uint32_t tMillis1, tMillis2;

  // Blink LED
  tCurrentMillis = millis();
  if (tCurrentMillis - tPreviousMillis >= 500) {
    tPreviousMillis = tCurrentMillis;
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(LED_BUILTIN, ledState);

    //Increment testU16, and print it's value
    appConf.w.testU16++;
    Serial.println(""); //Blank line
    Serial.print(F("appConf.w.testU16 = "));
    Serial.println(appConf.w.testU16);

    //Save it to appConf file
    tMillis1 = millis();
    MxConfig<AppConfig> appConfObj(&appConf);
    if (appConfObj.save("appConf")) {
      tMillis2 = millis();
      Serial.print(F("Successfully saved config in "));
      Serial.print(tMillis2 - tMillis1);
      Serial.println(F("ms"));
    }
    else {
      Serial.println(F("ERROR saving config!"));
    }
  }
}
