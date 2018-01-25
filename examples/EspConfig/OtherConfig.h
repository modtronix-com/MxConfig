/**
Other Configuration Examples.

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
#ifndef OTHERCONF_H
#define OTHERCONF_H

#include <type_traits>

#include "MxConfig.h"

#ifdef MXCONFIG_HAS_8BIT
#undef MXCONFIG_HAS_8BIT
#endif
#ifdef MXCONFIG_HAS_STR
#undef MXCONFIG_HAS_STR
#endif



/**
* Test "Modtronix Config" structure.
* IMPORTANT!
* - Only add to end of Structure and "Bit Structures".
* - Must comply to format required for a "Modtronix Config" structure. See "MxConfig.h" for details.
*/
typedef struct Test1Config_ {

  // 32-Bit Members ///////////////////////////////////////////////////////////
  struct dw_ {
    uint32_t    testU32;        //Test UINT32
  } _ALIGNED_(4) _PACKED_ dw;

  // 8-Bit Byte Members ///////////////////////////////////////////////////////
  #define MXCONFIG_HAS_8BIT
  struct b_ {
    uint8_t     testU8;

    //Uncomment this byte if required to make 'b' structure even size. MUST be called 'u8Padding' !!
    uint8_t     u8Padding;
  } _PACKED_ b;

  // Flags Section ////////////////////////////////////////////////////////////
  // IMPORTANT: When adding new flags in future, ensure their default values are 0!
  uint16_t _ALIGNED_(2) numberOfFlags;
  union {
    struct {
      //Flags 1-8
      uint16_t    f_test1Bit : 1;
      uint16_t    f_test2Bit : 2;
    };
    uint8_t       Val[2];               //Must be 2,6,10,14.... bytes long
  } _ALIGNED_(2) _PACKED_;

  // !!!!! IMPORTANT !!!!! Update numberOfFlags(n) with number of flags used above !!!!!
  Test1Config_(uint8_t dummy) : numberOfFlags(3) {}
  Test1Config_() = default;
} _ALIGNED_(2) _PACKED_ Test1Config;

static_assert(std::is_pod<Test1Config>::value, "Test1Config is NOT a POD");   //Is NOT a POD any more

#ifdef MXCONFIG_HAS_8BIT
static_assert(sizeof(Test1Config::b) % 2 == 0, "Test1Config 8-Bit section is NOT even. Add or Remove the 'u8Padding' padding byte");
#undef MXCONFIG_HAS_8BIT
#endif

//Ensure "String Section" starts on a word boundary (even address). 
#ifdef MXCONFIG_HAS_STR
static_assert(offsetof(Test1Config, str) % 2 == 0, "Test1Config String section does NOT start on 16-Bit boundray!");
#undef MXCONFIG_HAS_STR
#endif

static_assert(offsetof(Test1Config, numberOfFlags) % 2 == 0, "Test1Config::numberOfFlags section does NOT start on 16-Bit boundray!");




/**
* Test "Modtronix Config" structure.
* IMPORTANT!
* - Only add to end of Structure and "Bit Structures".
* - Must comply to format required for a "Modtronix Config" structure. See "MxConfig.h" for details.
*/
typedef struct Test2Config_ {

  // 16-Bit Members ///////////////////////////////////////////////////////////
  struct w_ {
    uint16_t    testU16;       //Test UINT16
  } _ALIGNED_(2) _PACKED_ w;

  // Flags Section ////////////////////////////////////////////////////////////
  // IMPORTANT: When adding new flags in future, ensure their default values are 0!
  uint16_t      numberOfFlags;
  union {
    struct {
      //Flags 1-8
      uint16_t    f_loaded : 1;        //Indicates if this structure was loaded from Flash/EEPROM/FileSystem
      uint16_t    f_test1Bit : 1;
      uint16_t    f_test2Bit : 2;
    };
    uint8_t       Val[2];               //Must be 2,6,10,14.... bytes long
  } _ALIGNED_(2) _PACKED_;

  Test2Config_(uint8_t dummy) : numberOfFlags(4) {}
  Test2Config_() = default;
} _ALIGNED_(2) _PACKED_ Test2Config;

static_assert(std::is_pod<Test2Config>::value, "Test2Config is NOT a POD");   //Is NOT a POD any more

#ifdef MXCONFIG_HAS_8BIT
static_assert(sizeof(Test2Config::b) % 2 == 0, "Test2Config 8-Bit section is NOT even. Add or Remove the 'u8Padding' padding byte");
#undef MXCONFIG_HAS_8BIT
#endif

//Ensure "String Section" starts on a word boundary (even address). 
#ifdef MXCONFIG_HAS_STR
static_assert(offsetof(Test2Config, str) % 2 == 0, "Test2Config String section does NOT start on 16-Bit boundray!");
#undef MXCONFIG_HAS_STR
#endif



// External GLOBAL VARIABLES //////////////////////////////////////////////////
#if !defined(THIS_IS_OTHERCONFIG_C)
extern Test1Config      test1Conf;
extern Test2Config      test2Conf;
#endif


//Function Prototypes /////////////////////////////////////////////////////////
void test1ConfSetDefaults();
void test2ConfSetDefaults();


#endif
