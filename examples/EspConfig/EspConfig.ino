/*
 Test for config files for a SBC with a File System.
 
 This demo uses the MxConfig library.
*/

//Use MxConfig library, also includes SPIFFS files
#include "MxConfig.h"

//Include Files
#include "AppConfig.h"
#include "OtherConfig.h"


// Defines ////////////////////////////////////////////////////////////////////


// Function Prototypes ////////////////////////////////////////////////////////


// Globals ////////////////////////////////////////////////////////////////////


void setup() {
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

  //Flash File System Test
  //ffsInfo();

  //Test configutation data
  testConf();

  File f2;
  f2.close();
}



/**
* Print Config Info
*/
void testConf() {
  uint32_t tMillis1, tMillis2;

  Serial.println(F("\n===== AppConfig Info ====="));
  appConfSetDefaults();
  MxConfig<AppConfig> appConfObj(&appConf);
#if(1) //Print Offsets
  Serial.print(F("Offset of 32-Bit Section: "));
  Serial.println(appConfObj.get32BitOffset());
  Serial.print(F("Offset of 16-Bit Section: "));
  Serial.println(appConfObj.get16BitOffset());
  Serial.print(F("Offset of 8-Bit Section: "));
  Serial.println(appConfObj.get8BitOffset());
  Serial.print(F("Offset of Strings Section: "));
  Serial.println(appConfObj.getStrOffset());
  Serial.print(F("Offset of Flags: "));
  Serial.println(appConfObj.getFlagsOffset());
#endif
#if(1) //Print Setion Sizes
  Serial.print(F("Size of 32-Bit Section: "));
  Serial.println(appConfObj.get32BitSize());
  Serial.print(F("Size of 16-Bit Section: "));
  Serial.println(appConfObj.get16BitSize());
  Serial.print(F("Size of 8-Bit Section: "));
  Serial.println(appConfObj.get8BitSize());
  Serial.print(F("Size of Strings Section: "));
  Serial.println(appConfObj.getStrSize());
  Serial.print(F("Number of Flags: "));
  Serial.println(appConfObj.getFlagsSize());
#endif


#if(0)
  Serial.println(F("\n===== Test1Conf Info ====="));
  test1ConfSetDefaults();
  MxConfig<Test1Config> test1ConfObj(&test1Conf);
  test1ConfObj.save("test1");
  test1ConfObj.load("test1");
  //Serial.print(F("Number of Flags: "));
  //Serial.println(test1ConfObj.getFlagsSize());
  //Serial.print(F("Size of Strings Section: "));
  //Serial.println(test1ConfObj.getStrSize());
  //Serial.print(F("Size of 32-Bit Section: "));
  //Serial.println(test1ConfObj.get32BitSize());
  //Serial.print(F("Size of 16-Bit Section: "));
  //Serial.println(test1ConfObj.get16BitSize());
  //Serial.print(F("Size of 8-Bit Section: "));
  //Serial.println(test1ConfObj.get8BitSize());
#endif


#if(0)
  Serial.println(F("\n===== Test2Conf Info ====="));
  test2ConfSetDefaults();
  MxConfig<Test2Config> test2ConfObj(&test2Conf);
  test2ConfObj.save("test2");
  test2ConfObj.load("test2");
  //Serial.print(F("Number of Flags: "));
  //Serial.println(test2ConfObj.getFlagsSize());
  //Serial.print(F("Size of Strings Section: "));
  //Serial.println(test2ConfObj.getStrSize());
  //Serial.print(F("Size of 32-Bit Section: "));
  //Serial.println(test2ConfObj.get32BitSize());
  //Serial.print(F("Size of 16-Bit Section: "));
  //Serial.println(test2ConfObj.get16BitSize());
  //Serial.print(F("Size of 8-Bit Section: "));
  //Serial.println(test2ConfObj.get8BitSize());
#endif

  
  #if(1)
  Serial.println(F("\n===== AppConfig Vars ====="));
  Serial.print(F("testU8: "));
  Serial.println(appConf.b.testU8);
  Serial.print(F("testU16: "));
  Serial.println(appConf.w.testU16);
  Serial.print(F("testU32: "));
  Serial.println(appConf.dw.testU32);
  Serial.print(F("ssid: "));
  Serial.println(appConf.str.ssid);
  Serial.print(F("password: "));
  Serial.println(appConf.str.password);
#endif


  Serial.println(F("\n===== AppConfig Change and Save Vars ====="));
  appConf.b.testU8 = 81;
  appConf.w.testU16 = 161;
  appConf.dw.testU32 = 321;
  Serial.print(F("testU8: "));
  Serial.println(appConf.b.testU8);
  Serial.print(F("testU16: "));
  Serial.println(appConf.w.testU16);
  Serial.print(F("testU32: "));
  Serial.println(appConf.dw.testU32);
  Serial.print(F("ssid: "));
  Serial.println(appConf.str.ssid);
  Serial.print(F("password: "));
  Serial.println(appConf.str.password);
  tMillis1 = millis();
  if (appConfObj.save("appConf")) {
    tMillis2 = millis();
    Serial.print(F("Successfully saved config in "));
    Serial.print(tMillis2 - tMillis1);
    Serial.println(F("ms"));
  }
  else {
    Serial.println(F("ERROR saving config!"));
  }


  Serial.println(F("\n===== AppConfig Restore Vars to Defaults ====="));
  appConfSetDefaults();
  Serial.print(F("testU8: "));
  Serial.println(appConf.b.testU8);
  Serial.print(F("testU16: "));
  Serial.println(appConf.w.testU16);
  Serial.print(F("testU32: "));
  Serial.println(appConf.dw.testU32);
  Serial.print(F("ssid: "));
  Serial.println(appConf.str.ssid);
  Serial.print(F("password: "));
  Serial.println(appConf.str.password);


  Serial.println(F("\n===== AppConfig load saved Vars ====="));
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
  Serial.print(F("testU8: "));
  Serial.println(appConf.b.testU8);
  Serial.print(F("testU16: "));
  Serial.println(appConf.w.testU16);
  Serial.print(F("testU32: "));
  Serial.println(appConf.dw.testU32);
  Serial.print(F("ssid: "));
  Serial.println(appConf.str.ssid);
  Serial.print(F("password: "));
  Serial.println(appConf.str.password);
}

/**
* Print FFS Info
*/
void ffsInfo() {
  //For ESP8266, AppConfig is stored in SPIFFS File System
#if defined(ESP8266)
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  Serial.println(F("\n===== FFS Info ====="));
  Serial.print(F("totalBytes: "));
  Serial.println(fs_info.totalBytes);
  Serial.print(F("usedBytes: "));
  Serial.println(fs_info.usedBytes);
  Serial.print(F("blockSize: "));
  Serial.println(fs_info.blockSize);
  Serial.print(F("pageSize: "));
  Serial.println(fs_info.pageSize);
  Serial.print(F("maxOpenFiles: "));
  Serial.println(fs_info.maxOpenFiles);
  Serial.print(F("maxPathLength: "));
  Serial.println(fs_info.maxPathLength);
#endif
}


/**
* Main Loop
*/
void loop() {
  static uint32_t previousMillis = 0;
  static uint8_t ledState;
  uint32_t currentMillis;

  // Blink LED
  currentMillis = millis();
  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(LED_BUILTIN, ledState);
  }

}
