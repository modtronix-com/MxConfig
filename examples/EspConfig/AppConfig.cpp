
/**
Application configuration.
*/
#define THIS_IS_APPCONFIG_C

#include <stdint.h>         //Required for uintx_t defines
#include "AppConfig.h"


//Variables
AppConfig       appConf(0);


void appConfSetDefaults() {
  appConf.b.testU8 = 80;
  appConf.w.testU16 = 160;
  appConf.dw.testU32 = 3200;

  STR_CPY_P(appConf.str.ssid, "mySSID");
  STR_CPY_P(appConf.str.password, "myPASSWORD");
}
