/**
Other Configuration Examples.
*/
#define THIS_IS_OTHERCONFIG_C

//For ESP8266, Test1Config is stored in SPIFFS File System
#if defined(ESP8266)
#include <HardwareSerial.h> //Required for Serial
#elif defined(ARDUINO_ARCH_SAMD)
#include <variant.h> //Required for Serial
#endif

#include <stdint.h>         //Required for uintx_t defines
#include "OtherConfig.h"


/////////////////////////////////////////////////
//Variables
Test1Config     test1Conf(0);
Test2Config     test2Conf(0);

static const char FILENAME_Test1Config[] PROGMEM = "/test1.cfg";


void test1ConfSetDefaults() {
  test1Conf.b.testU8 = 80;
  test1Conf.dw.testU32 = 3200;
}

void test2ConfSetDefaults() {
}

