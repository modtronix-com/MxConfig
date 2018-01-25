/**
 Modtronix configuration.


 ============================
 ========== SPIFFS ==========
 This library uses SPIFFS. For documenation, see:
 http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html#file-system-object-spiffs

 Some examples how to use it:
 https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/ConfigFile/ConfigFile.ino


 ======================================
 ============= File Format ============
 The file is stored in the File System with exension *.xc1 or *.xc2. There should always only be one file.
 When a new file gets created(by saving config), old file is deleted.
 
 Each fill will have a "startRollingNumber" and "endRollingNumber"(one more than "startRollingNumber") at begining
 and end of file. When a new file is created, it will be incremented. If two files exist, the one with lowest
 rollingNumber is deleted.

 The file format for V1 is:
 [2 - startRollingNumber]
 [2 - MxConfig Version]
 [2 - offset 32-Bit Section]
 [2 - offset 16-Bit Section]
 [2 - offset 8-Bit Section]
 [2 - offset Flags Section]
 [2 - number of flags in bits]
 [n - Strings]                {2 Byte Alligned}
 [n - 32-Bit Data]            {4 Byte Alligned}
 [n - 16-Bit Data]            {2 Byte Alligned}
 [n - 8-Bit Data]             {2 Byte Alligned}
 [n - Flags]                  {2 Byte Alligned}
 [2 - endRollingNumber]


 ======================================
 ========== Config Structure ==========
 For a structure to be a valid "Modtronix Config" structure, it must have the following member variables:

 typedef struct SampleConfig__ {
  uint16_t      numberOfFlags;
 };

 
 =============================
 ========== Example ==========
 This is an example "Modtronix Config" structure:
 typedef struct AppConfig_ {
 // 32-Bit Members ///////////////////////////////////////////////////////////
 struct dw_ {
 uint32_t    testU32;
 uint32_t    testU32_2;
 } _ALIGNED_(4) _PACKED_ dw;

 // 16-Bit Members ///////////////////////////////////////////////////////////
 struct w_ {
 uint16_t    testU16;
 //uint32_t    testU32;    // Placing 32-Bit variable on 16-Bit boundary seems to work, BUT created larger code on ESP8266! Use 'dw' structure!
 uint16_t    testU16_2;
 } _ALIGNED_(2) _PACKED_ w;

 // 8-Bit Byte Members ///////////////////////////////////////////////////////
 #define MXCONFIG_HAS_8BIT
 struct b_ {
 uint8_t     testU8;

 //Uncomment this byte if required to make 'b' structure even size. MUST be called 'u8Padding' !!
 uint8_t     u8Padding;
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

  
 =============================
 ========== License ==========
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
#ifndef MXCONF_H
#define MXCONF_H


// Includes ///////////////////////////////////////////////////////////////////

#if defined(ARDUINO_ARCH_SAMD) || defined(ESP8266) || defined(ESP32)
#include <type_traits>  //Required for static_assert
#endif

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>         //Required for PSTR define on ESP8266
#endif

//For ESP8266/ESP32, "Modtronix Configuration" Structure is stored in SPIFFS File System
#if defined(ESP8266)
#include <FS.h>
#elif defined(ESP32)
#include "FS.h"
#include "SPIFFS.h"
#endif


// Defines ////////////////////////////////////////////////////////////////////
//Debug level 0=Off, 1=Error, 2=Warning, 3=Info
#define MXCONF_DEBUG_LVL_OFF    0
#define MXCONF_DEBUG_LVL_ERR    1
#define MXCONF_DEBUG_LVL_WARN   2
#define MXCONF_DEBUG_LVL_INFO   3
#ifndef MXCONF_DEBUG_LEVEL
#define MXCONF_DEBUG_LEVEL      MXCONF_DEBUG_LVL_INFO
//#define MXCONF_DEBUG_LEVEL      MXCONF_DEBUG_LVL_ERR
#endif

#define MXCONF_VERSION 1

#ifndef _PACKED_
#define _PACKED_ __attribute__((packed))
#endif

#ifndef _ALIGNED_
#define _ALIGNED_(x) __attribute__((aligned(x)))
#endif

//Functions to use constant strings located in Flash.
//ESP8266 - Use "xxx_P" functions, and wrap constant string in PSTR().
//          see http://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
//        - For print and println, wrap string in F() function.
//        - !!! IMPORTANT !!! Seems like can NOT use any of these function in templated class. It causes errors.
//ESP32   - Seems like normal functions can be used. On ESP32, there is no need for PROGMEM because constant
//          data is automatically stored into Flash, and can be accessed from Flash without copying into RAM first. 
//SAMD    - Seems like normal functions can be used
#if defined(ARDUINO_ARCH_AVR)
//Normal functions can be used for AVR
/** Copy string from Flash to RAM.*/
//#define STR_CPY_P(dst,src) strcpy(dst,src)
#define STR_CPY_P(dst,src) strcpy_P(dst,PSTR(src))
#elif defined(ARDUINO_ARCH_SAMD)
//I think normal functions can be used for SAMD??? Confirm this??
/** Copy string from Flash to RAM.*/
//#define STR_CPY_P(dst,src) strcpy(dst,src)
#define STR_CPY_P(dst,src) strcpy_P(dst,PSTR(src))
#elif defined(ESP32)
//Normal functions can be used for ESP32
/** Copy string from Flash to RAM.*/
#define STR_CPY_P(dst,src) strcpy(dst, src)
#elif defined(ESP8266)
/** Copy string from Flash to RAM.*/
#define STR_CPY_P(dst,src) strcpy_P(dst,PSTR(src))
#endif


//Header used when storing MxConfigObj object File System.
//The file format for V1 is :
//[2 - startRollingNumber]
//[2 - MxConfig Version]
//[2 - offset 32 - Bit Section]
//[2 - offset 16 - Bit Section]
//[2 - offset 8 - Bit Section]
//[2 - offset Flags Section]
//[2 - number of flags in bits]
typedef struct MxConfigFsHdr_ {
  uint16_t  startRollingNumber;
  uint16_t  mxConfVersion;      // The "Modtronix Config" version number. Given in MXCONF_VERSION
  uint32_t  checksum;           // Checksum for all or some of the data in file?
  uint16_t  offsetU16;          // Offset of the first 16-Bit Variable
  uint16_t  offsetU8;           // Offset of the first 8-Bit Variable
  uint16_t  offsetStr;          // Offset of the first String
  uint16_t  offsetFlags;        // Offset of the flags section (offset of numberOfFlags variable, actual flags FOLLOW IT!)
  uint16_t  numberOfBytes;      // Number off 8-Bit variables in bytes(EXCLUDING padding). This can be smaller than U8 section if padding bytes are required to get section EVEN
  uint16_t  numberOfFlags;      // Number of flags, in bits
} _PACKED_ MxConfigFsHdr;
static_assert(std::is_pod<MxConfigFsHdr>::value, "MxConfigFsHdr is NOT a POD");


//Object used to store information of "Modtronix Config" structure.
typedef struct MxConfigInfo_ {
  MxConfigFsHdr   hdr;
  uint16_t        endRollingNumber;

  uint16_t        mxConfStructSize;   //Size of "Modtronix Config" Structure
  uint8_t*        pMxConfStruct;      //Pointer to actual "Modtronix Config" structure.
} _PACKED_ MxConfigInfo;
static_assert(std::is_pod<MxConfigInfo>::value, "MxConfig is NOT a POD");


// External GLOBAL VARIABLES //////////////////////////////////////////////////
#if !defined(THIS_IS_MXCONFIG_C)
extern uint8_t mxConfDebugLevel;
extern Stream* mxConfDebug;
#endif

// Function Prototype /////////////////////////////////////////////////////////

/**
* Test if given file exists
*/
bool mxConfExists(const char* filename);

/**
* Set debug level and Stream
*
* @param level Debug level. 0=Off, 1=Error, 2=Warning, 3=Info
* @param strm Stream to write debug information to
*/
void mxConfSetDebugLevel(uint8_t level, Stream* strm);

/**
* Check if formatting is required. If so, will format flash, and create "mxfrmtdn.txt"
* market file to FS.
*/
bool mxConfCheckFormat();

/**
* Load MxConfig data from given file. Given filename does NOT include extension. An extension
* 'xc1' or 'xc2' will be used.
*
* @preconditon
* Ensure FS has already been initialized before calling this function!
* For SPIFFS, we have to call SPIFFS.begin()
*/
bool mxConfLoad(MxConfigInfo* pConfInfo, const char* filename);

/**
* Save MxConfigInfo data to given file
*/
bool mxConfSave(MxConfigInfo* pConfInfo, const char* filename);

bool mxConfSave(MxConfigInfo* pConfInfo, const __FlashStringHelper * filename);



// Templated Functions ////////////////////////////////////////////////////////

// Template methods for 32-Bit sections in structure //////////////////////////
//Method for determining if a structure has a member variable. Got code from https://stackoverflow.com/a/16000226/8804200

//template<typename T, typename = void>
//struct mxConfHas32Bit : std::false_type { };
//template<typename T>
//struct mxConfHas32Bit<T, decltype(std::declval<T>().offsetOf32BitMembers, void())> : std::true_type { };
//
////Method for getting offset of member. Returns -1 if not present. Derived from code above.
//template<typename T, typename = void>
//struct mxConfOffsetOf32Bit : std::integral_constant<int, 0xffff> { };
//template<typename T>
//struct mxConfOffsetOf32Bit<T, decltype(std::declval<T>().offsetOf32BitMembers, void())> : std::integral_constant<int, (int)(&((T*)(0))->offsetOf32BitMembers) > { };

template<typename T, typename = void>
struct mxConfHas32Bit : std::false_type { };
template<typename T>
struct mxConfHas32Bit<T, decltype(std::declval<T>().dw, void())> : std::true_type { };

//Method for getting offset of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfOffsetOf32Bit : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfOffsetOf32Bit<T, decltype(std::declval<T>().dw, void())> : std::integral_constant<int, (int)(&((T*)(0))->dw) > { };






// Template methods for 16-Bit sections in structure //////////////////////////
//Method for determining if a structure has a member variable. Got code from https://stackoverflow.com/a/16000226/8804200

//template<typename T, typename = void>
//struct mxConfHas16Bit : std::false_type { };
//template<typename T>
//struct mxConfHas16Bit<T, decltype(std::declval<T>().offsetOf16BitMembers, void())> : std::true_type { };
//
////Method for getting offset of member. Returns -1 if not present. Derived from code above.
//template<typename T, typename = void>
//struct mxConfOffsetOf16Bit : std::integral_constant<int, 0xffff> { };
//template<typename T>
//struct mxConfOffsetOf16Bit<T, decltype(std::declval<T>().offsetOf16BitMembers, void())> : std::integral_constant<int, (int)(&((T*)(0))->offsetOf16BitMembers) > { };
//
template<typename T, typename = void>
struct mxConfHas16Bit : std::false_type { };
template<typename T>
struct mxConfHas16Bit<T, decltype(std::declval<T>().w, void())> : std::true_type { };

//Method for getting offset of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfOffsetOf16Bit : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfOffsetOf16Bit<T, decltype(std::declval<T>().w, void())> : std::integral_constant<int, (int)(&((T*)(0))->w) > { };




// Template methods for 8-Bit sections in structure ///////////////////////////
//Method for determining if a structure has a member variable. Got code from https://stackoverflow.com/a/16000226/8804200

//template<typename T, typename = void>
//struct mxConfHas8Bit : std::false_type { };
//template<typename T>
//struct mxConfHas8Bit<T, decltype(std::declval<T>().offsetOf8BitMembers, void())> : std::true_type { };
//
////Method for getting offset of member. Returns -1 if not present. Derived from code above.
//template<typename T, typename = void>
//struct mxConfOffsetOf8Bit : std::integral_constant<int, 0xffff> { };
//template<typename T>
//struct mxConfOffsetOf8Bit<T, decltype(std::declval<T>().offsetOf8BitMembers, void())> : std::integral_constant<int, (int)(&((T*)(0))->offsetOf8BitMembers) > { };


//template<typename T, typename = void>
//struct mxConfHas8Bit : std::false_type { };
//template<typename T>
//struct mxConfHas8Bit<T, decltype(std::declval<T>().b.offsetOf8BitMembers, void())> : std::true_type { };
//
////Method for getting offset of member. Returns -1 if not present. Derived from code above.
//template<typename T, typename = void>
//struct mxConfOffsetOf8Bit : std::integral_constant<int, 0xffff> { };
//template<typename T>
//struct mxConfOffsetOf8Bit<T, decltype(std::declval<T>().b.offsetOf8BitMembers, void())> : std::integral_constant<int, (int)(&((T*)(0))->b.offsetOf8BitMembers) > { };


template<typename T, typename = void>
struct mxConfHas8Bit : std::false_type { };
template<typename T>
struct mxConfHas8Bit<T, decltype(std::declval<T>().b, void())> : std::true_type { };

template<typename T, typename = void>
struct mxConfHas8BitPadding : std::false_type { };
template<typename T>
struct mxConfHas8BitPadding<T, decltype(std::declval<T>().b.u8Padding, void())> : std::true_type { };


//Method for getting offset of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfOffsetOf8Bit : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfOffsetOf8Bit<T, decltype(std::declval<T>().b, void())> : std::integral_constant<int, (int)(&((T*)(0))->b) > { };


//Method for getting sizeof of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfSizeOf8Bit : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfSizeOf8Bit<T, decltype(std::declval<T>().b, void())> : std::integral_constant<int, (int)sizeof(((T*)(0))->b) > { };




// Template methods for Strings sections in structure //////////////////////////
//Method for determining if a structure has a member variable. Got code from https://stackoverflow.com/a/16000226/8804200
template<typename T, typename = void>
struct mxConfHasStr : std::false_type { };
template<typename T>
struct mxConfHasStr<T, decltype(std::declval<T>().str, void())> : std::true_type { };

//Method for getting offset of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfOffsetOfStr : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfOffsetOfStr<T, decltype(std::declval<T>().str, void())> : std::integral_constant<int, (int)(&((T*)(0))->str) > { };











// Template methods for Flags sections in structure ///////////////////////////
//Method for determining if a structure has a member variable. Got code from https://stackoverflow.com/a/16000226/8804200
template<typename T, typename = void>
struct mxConfHasFlags : std::false_type { };
template<typename T>
struct mxConfHasFlags<T, decltype(std::declval<T>().numberOfFlags, void())> : std::true_type { };

//Method for getting offset of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfOffsetOfFlags : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfOffsetOfFlags<T, decltype(std::declval<T>().numberOfFlags, void())> : std::integral_constant<int, (int)(&((T*)(0))->numberOfFlags) > { };



//Method for getting sizeof of member. Returns -1 if not present. Derived from code above.
template<typename T, typename = void>
struct mxConfSizeOfStr : std::integral_constant<int, 0xffff> { };
template<typename T>
struct mxConfSizeOfStr<T, decltype(std::declval<T>().str, void())> : std::integral_constant<int, (int)sizeof(((T*)(0))->str) > { };



// MxConfig Class /////////////////////////////////////////////////////////////
template <typename MxConfigStruct>
class MxConfig {
  // Public Member variables
public:
  MxConfigStruct* pMxConfStruct;  // Pointer to actual "Modtronix Config" Structure
  MxConfigInfo    mxConfInfo;     // Info for the "Modtronix Config" Structure

  uint16_t startRollingNumber;

  // Public Member functions
public:

  /**
  * Constructor
  */
  MxConfig(MxConfigStruct* pMxConfigStruct) {
    uint16_t i, offset;

    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) mxConfDebug->println("MxConfig() Constructor");
    #endif

    pMxConfStruct = pMxConfigStruct;
    mxConfInfo.pMxConfStruct = (uint8_t*)pMxConfStruct;
    mxConfInfo.mxConfStructSize = sizeof(MxConfigStruct);
    mxConfInfo.hdr.numberOfFlags = pMxConfigStruct->numberOfFlags;
    mxConfInfo.hdr.mxConfVersion = MXCONF_VERSION;

    //Get number of 8-bit bytes, EXCLUDING any padding bytes. We DO NOT store the padding bytes. Very important
    //if we upgrade structure in future software version, that we only store actual number of bytes containing data(not padding).
    //mxConfInfo.hdr.numberOfBytes = mxConfSizeOf8Bit<MxConfigStruct>::value;
    mxConfInfo.hdr.numberOfBytes = sizeof(pMxConfigStruct->b);
    if (mxConfHas8BitPadding<MxConfigStruct>::value) {
      //mxConfDebug->println("MxConfig structure has a 'u8Padding' member");
      mxConfInfo.hdr.numberOfBytes--;
    }

    //Set to first of the 4 "offsetXxx" variables for the 4 sections (16-Bit,8-Bit,Str and flags)
    uint16_t* pOffsets = (uint16_t*)&mxConfInfo.hdr.offsetU16;

    //Get offsets of all 4 Sections. The mxConfOffsetOfXxx<> will return -1 if variable not present.
    mxConfInfo.hdr.offsetU16 = mxConfOffsetOf16Bit<MxConfigStruct>::value;
    mxConfInfo.hdr.offsetU8 = mxConfOffsetOf8Bit<MxConfigStruct>::value;
    mxConfInfo.hdr.offsetStr = mxConfOffsetOfStr<MxConfigStruct>::value;
    mxConfInfo.hdr.offsetFlags = mxConfOffsetOfFlags<MxConfigStruct>::value;

    

    //All sections not present will be -1. Set them equal to previous section. Process all 4 sections.
    offset = 0;
    for (i = 0; i < 4; i++) {
      if (*pOffsets == 0xffff) {
        *pOffsets = offset;
      }
      offset = *pOffsets;
      pOffsets++;
    }
  }


  /**
  * Test if given file exists
  */
  bool exists(const char* filename) {
    //For ESP8266/ESP32, "Modtronix Configuration" Structure is stored in SPIFFS File System
#if defined(ESP8266) || defined(ESP32)
    //Check if file exists
    if (SPIFFS.exists(filename)) {
      return true;
    }
#endif
    return false;
  }


  bool save(const __FlashStringHelper * filename) {
    return mxConfSave(&mxConfInfo, filename);
  }

  /**
  * Save MxConfigObj data to given file
  *
  * @param filename - Is filename, without extension! The extension *.xc1 or *.xc2 will be added.
  */
  bool save(const char* filename) {
    //Can use this to save program memory if this function gets large. This will help, because
    //template functions are always inlined. So, for each MxConfig, new code is generated.
    //Using a function, will mean code is only generated once.
    return mxConfSave(&mxConfInfo, filename);
  }

  /**
  * Load MxConfig data from given file
  *
  * @param filename - Is filename, without extension! The extension *.xc1 or *.xc2 will be added.
  *
  * @preconditon
  * Ensure FS has already been initialized before calling this function!
  * For SPIFFS, we have to call SPIFFS.begin()
  */
  bool load(const char* filename) {
    //Can use this to save program memory if this function gets large. This will help, because
    //template functions are always inlined. So, for each MxConfig, new code is generated.
    //Using a function, will mean code is only generated once.
    return mxConfLoad(&mxConfInfo, filename);
  }

  

  /**
  * Get offset of 32-Bit Variable section.
  */
  uint16_t get32BitOffset() {
    return 0;   //First element of structure
  }


  /**
  * Get offset of 16-Bit Variable section.
  */
  uint16_t get16BitOffset() {
    return mxConfInfo.hdr.offsetU16;
  }


  /**
  * Get offset of 8-Bit Variable section.
  */
  uint16_t get8BitOffset() {
    return mxConfInfo.hdr.offsetU8;
  }

  /**
  * Get offset of Strings section
  */
  uint16_t getStrOffset() {
    return mxConfInfo.hdr.offsetStr;
  }

  /**
  * Get offset of flags section..
  */
  uint16_t getFlagsOffset() {
    return mxConfInfo.hdr.offsetFlags;
  }




  /**
  * Get reserved size of all 32-Bit Variables contained in given MxConfigObj object.
  * Can contain some fill bytes.
  */
  uint16_t get32BitSize() {
    return mxConfInfo.hdr.offsetU16;
  }


  /**
  * Get reserved size of all 16-Bit Variables contained in given MxConfigObj object.
  * Can contain some fill bytes.
  */
  uint16_t get16BitSize() {
    return mxConfInfo.hdr.offsetU8 - mxConfInfo.hdr.offsetU16;
  }


  /**
  * Get reserved size of all 8-Bit Variables contained in given MxConfigObj object.
  * Can contain some fill bytes.
  */
  uint16_t get8BitSize() {
    //return mxConfInfo.hdr.offsetStr - mxConfInfo.hdr.offsetU8;
    return mxConfInfo.hdr.numberOfBytes;
  }

  /**
  * Get reserved size of all Strings contained in given MxConfigObj object.
  * Can contain some fill bytes.
  */
  uint16_t getStrSize() {
    return mxConfInfo.hdr.offsetFlags - mxConfInfo.hdr.offsetStr;
  }

  /**
  * Get number of flags(in bits) contained in given MxConfigObj object.
  */
  uint16_t getFlagsSize() {
    return mxConfInfo.hdr.numberOfFlags;
  }
};


#endif
