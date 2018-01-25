/**
 Application configuration.

 Software License Agreement
 
 The software supplied herewith is owned by Modtronix Engineering, and is
 protected under applicable copyright laws. The software supplied herewith is
 intended and supplied to you, the Company customer, for use solely and
 exclusively on products manufactured by Modtronix Engineering. The code may
 be modified and can be used free of charge for commercial and non commercial
 applications. All rights are reserved. Any use in violation of the foregoing
 restrictions may subject the user to criminal sanctions under applicable laws,
 as well as to civil liability for the breach of the terms and conditions of this license.
 
 THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 File History
 
 2017-10-15, David H. (DH):
     - Initial version
 */
#ifndef APPCONF_H
#define APPCONF_H

#include <type_traits>

#if defined(ESP8266)
#include <pgmspace.h>   //Required for PSTR define on ESP8266
#endif

//Use MxConfig library
#include "MxConfig.h"

#ifdef MXCONFIG_HAS_8BIT
#undef MXCONFIG_HAS_8BIT
#endif
#ifdef MXCONFIG_HAS_STR
#undef MXCONFIG_HAS_STR
#endif


/**
* Application Settings that are saved to EEPROM/Flash.
* IMPORTANT!
* - Only add to end of Structure and "Bit Structures".
* - Must comply to format required for a "Modtronix Config" structure. See "MxConfig.h" for details.
*/
typedef struct AppConfig_ {
  // 32-Bit Members ///////////////////////////////////////////////////////////
  struct dw_ {
    uint32_t    testU32;
    uint32_t    testU32_2;
  } _ALIGNED_(4) _PACKED_ dw;

  // 16-Bit Members ///////////////////////////////////////////////////////////
  struct w_ {
    uint16_t    testU16;
    uint32_t    testU32;    // Test if CPU trigger alignment error if we put 32-Bit variable on 16-Bit boundary.
    uint16_t    testU16_2;
    uint16_t    testU16_3;
  } _ALIGNED_(2) _PACKED_ w;

  // 8-Bit Byte Members ///////////////////////////////////////////////////////
  #define MXCONFIG_HAS_8BIT
  struct b_ {
    uint8_t     testU8;
    uint8_t     testU8_2;
    uint8_t     testU8_3;
    uint8_t     testU8_4;

    //Uncomment this byte if required to make 'b' structure even size. MUST be called 'u8Padding' !!
    //uint8_t     u8Padding;
  } _PACKED_ b;
  
  // String Section ///////////////////////////////////////////////////////////
  // Strings lenghts must be multiple of 2
#define MXCONFIG_HAS_STR
  struct str_ {
    char        ssid[30];
    char        password[20];
  } _ALIGNED_(2) _PACKED_ str;

  // Flags Section ////////////////////////////////////////////////////////////
  // IMPORTANT: When adding new flags in future, ensure their default values are 0!
  uint16_t _ALIGNED_(2) numberOfFlags;
  union {
    struct {
      //Flags 1-8
      uint16_t    f_loaded : 1;        //Indicates if this structure was loaded from Flash/EEPROM/FileSystem
      uint16_t    f_test1Bit : 1;
      uint16_t    f_test2Bit : 2;
      uint16_t    f_test4Bit : 4;
    };
    uint8_t       Val[2];               //Must be multipe of 2 (2,4,6...)
  } _ALIGNED_(2) _PACKED_;

  // !!!!! IMPORTANT !!!!! Update numberOfFlags(n) with number of flags used above !!!!!
  AppConfig_(uint8_t dummy) : numberOfFlags(8) {}
  AppConfig_() = default;
} _ALIGNED_(2) _PACKED_ AppConfig;


static_assert(std::is_pod<AppConfig>::value, "AppConfig is NOT a POD");   //Is NOT a POD any more

#ifdef MXCONFIG_HAS_8BIT
static_assert(sizeof(AppConfig::b) % 2 == 0, "AppConfig 8-Bit section is NOT even. Add or Remove the 'u8Padding' padding byte");
#undef MXCONFIG_HAS_8BIT
#endif

//Ensure "String Section" starts on a word boundary (even address). 
#ifdef MXCONFIG_HAS_STR
static_assert(offsetof(AppConfig,str) % 2 == 0, "AppConfig String section does NOT start on 16-Bit boundray!");
#undef MXCONFIG_HAS_STR
#endif

static_assert(offsetof(AppConfig, numberOfFlags) % 2 == 0, "AppConfig::numberOfFlags section does NOT start on 16-Bit boundray!");



// External GLOBAL VARIABLES //////////////////////////////////////////////////
#if !defined(THIS_IS_APPCONFIG_C)
extern AppConfig        appConf;
#endif


//Function Prototypes /////////////////////////////////////////////////////////
void appConfSetDefaults();


#endif
