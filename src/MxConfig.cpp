
/**
Modtronix Configuration.
*/
#define THIS_IS_MXCONFIG_C

#include <stdint.h>     //Required for uintx_t defines
#include <Stream.h>

// TEST TEST For some reason Visual Micro does not detect ESP32 is declared
//#define ESP32
//#define ESP8266

/////////////////////////////////////////////////
//Global Variables - Must be before: #include "MxConfig.h"
uint8_t mxConfDebugLevel = 0; //Debuf level 0=Off, 1=Error, 2=Warning, 3=Info
Stream* mxConfDebug = NULL;   //If present, write debug information to it.

#include "MxConfig.h"

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



/////////////////////////////////////////////////
//Variables


/////////////////////////////////////////////////
//Function Prototypes
bool checkIfValidFile(const char * filename);


/**
* Test if given file exists
*
* @preconditon
* Ensure FS has already been initialized before calling this function!
* For SPIFFS, we have to call SPIFFS.begin()
*/
bool mxConfExists(const char* filename) {
  //For ESP8266/ESP32, AppConfig is stored in SPIFFS File System
#if defined(ESP8266) || defined(ESP32)
  //Check if file exists
  if (SPIFFS.exists(filename)) {
    return true;
  }
#endif
  return false;
}


/**
* Set debug level and Stream
*
* @param level Debug level. 0=Off, 1=Error, 2=Warning, 3=Info
* @param strm Stream to write debug information to
*/
void mxConfSetDebugLevel(uint8_t level, Stream* strm) {
  mxConfDebugLevel = level;
  mxConfDebug = strm;
}



/**
* Check if formatting is required. If so, will format flash, and write "mxfrmtdn.txt"
* market file to FS.
*/
bool mxConfCheckFormat() {
  if (!SPIFFS.exists(F("/mxfrmtdn.txt"))) {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if(mxConfDebugLevel) mxConfDebug->println(F("Please wait for SPIFFS to be formatted"));
    #endif

    SPIFFS.format();

    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel) mxConfDebug->println(F("SPIFFS formatted!"));
    #endif
    
    File f = SPIFFS.open("/mxfrmtdn.txt", "w");
    
    if (!f) {
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_WARN)
      if (mxConfDebugLevel) mxConfDebug->println(F("SPIFFS format marker file failed!"));
      #endif
    }
    else {
      f.println(F("Format Marker File. Do NOT delete!"));
    }

    f.close();
  }
  else {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel) mxConfDebug->println(F("SPIFFS is formatted. Moving along..."));
    #endif
  }
}



/**
* Confirm this is a valid "Modtronix Config" file
*
* @param filename Filename of file, including extension
*/
bool checkIfValidFile(const char * filepath) {
  MxConfigFsHdr   hdr;

  //For ESP8266/ESP32, "Modtronix Configuration" Stucture is stored in SPIFFS File System
#if defined(ESP8266) || defined(ESP32)
  File f = SPIFFS.open(filepath, "r");
  if (!f) {
    return false;
  }

  // Read File Header ///////////////////////////////////////////////////////
  f.read((uint8_t*)&hdr, sizeof(hdr));

  // TODO do check!

  f.close();

  return true;
#endif
}


bool mxConfSave(MxConfigInfo* pConfInfo, const __FlashStringHelper * filename) {
  char buf[32];
  if (!filename) return 0;                      // return if the pointer is void

  uint16_t strLen = strlen_P((PGM_P)filename);  // cast it to PGM_P, which is basically const char *, and measure it using the _P version of strlen.

  if (strLen == 0) return true;
  if (strLen >= (sizeof(buf) - 1)) {
    return false;
  }

  strcpy_P(buf, (PGM_P)filename);               //copy the string in using strcpy_P

  return mxConfSave(pConfInfo, (const char*)buf);
}


/**
* Save MxConfig data to given file.
*
* @preconditon
* Ensure FS has already been initialized before calling this function!
* For SPIFFS, we have to call SPIFFS.begin()
*/
bool mxConfSave(MxConfigInfo* pConfInfo, const char* filename) {
  uint16_t i, datasize, filenameLen;
  uint8_t foundFiles = 0;   // Bit 1 for *.xc1 file, bit 2 for *.xc2 file
  char oldFileExtNum = 0;   // Is '1' if old file is *.xc1, '2' if it is *.xc2, else 0
  char bufFilename[32];


  //For ESP8266/ESP32, "Modtronix Configuration" Structure is stored in SPIFFS File System
  #if defined(ESP8266) || defined(ESP32)

  ///////////////////////////////////////////////
  // Check if a file with this filename already exists. We save data to a
  // NEW file. Only after successfully saved, we deleted the existing file.

  //Check if *.xc1 or *.xc2 file exists
  bufFilename[0] = '/';
  strcpy(&bufFilename[1], filename);
  strcat(bufFilename, ".xc1");
  filenameLen = strlen(bufFilename);
  
  for (i = 0; i < 2; i++) {
    //Set extension to *.xc1 or *.xc2
    bufFilename[filenameLen - 1] = i + '1';

    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) {
      mxConfDebug->print(F("Searching for "));
      mxConfDebug->print(bufFilename);
    }
    #endif

    //Current file exists
    if (SPIFFS.exists(bufFilename)) {
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) mxConfDebug->println(F(" - Found!"));
      #endif

      //Confirm it is valid
      if (checkIfValidFile(bufFilename) == false) {
        //NOT VALID!! Delete it, and contineu to next file
        SPIFFS.remove(bufFilename);

        #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
        if (mxConfDebugLevel > 2) {
          mxConfDebug->println(F(" - Deleted INVALID file!"));
        }
        else {
          if (mxConfDebugLevel > 1) {
            mxConfDebug->println(F("Deleted INVALID configuration file!"));
          }
        }
        #endif

        continue;
      }

      //File IS valid
      foundFiles = foundFiles | (0x01 << i);
    }
    else {
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) mxConfDebug->println("");
      #endif
    }
  }

  ///////////////////////////////////////////////
  // If both files exist, delete oldest one
  if (foundFiles == 3) {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) mxConfDebug->println(F("Found 2 config file, deleting oldest one!"));
    #endif
    uint16_t f1StartRollingNumber, f2StartRollingNumber;

    //Open *.xc1 file, and read it's startRollingNumber
    bufFilename[filenameLen - 1] = '1';
    File f1 = SPIFFS.open(bufFilename, "r");
    if (f1) {
      f1.seek(offsetof(MxConfigFsHdr, startRollingNumber), fs::SeekSet);
      f1.read((uint8_t*)&f1StartRollingNumber, sizeof(f2StartRollingNumber));
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) {
        mxConfDebug->print(F("startRollingNumber for *.xc1 = "));
        mxConfDebug->println(f1StartRollingNumber);
      }
      #endif
    }

    //Open *.xc2 file, and read it's startRollingNumber
    bufFilename[filenameLen - 1] = '2';
    File f2 = SPIFFS.open(bufFilename, "r");
    if (f2) {
      f2.seek(offsetof(MxConfigFsHdr, startRollingNumber), fs::SeekSet);
      f2.read((uint8_t*)&f2StartRollingNumber, sizeof(f2StartRollingNumber));
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) {
        mxConfDebug->print(F("startRollingNumber for *.xc2 = "));
        mxConfDebug->println(f2StartRollingNumber);
      }
      #endif
    }

    //Delete oldest file (with smallest startRollingNumber
    if (f1StartRollingNumber > f2StartRollingNumber) {
      bufFilename[filenameLen - 1] = '2';         // Delete *.xc2 file
      foundFiles = foundFiles & ~(0x02);  // Clear bit for *.xc2 file
    }
    else {
      bufFilename[filenameLen - 1] = '1';         // Delete *.xc1 file
      foundFiles = foundFiles & ~(0x01);  // Clear bit for *.xc1 file
    }
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) {
      mxConfDebug->print(F("Deleting "));
      mxConfDebug->println(bufFilename);
    }
    #endif
    SPIFFS.remove(bufFilename);
  }


  
  ///////////////////////////////////////////////
  // Open file for writing

  // Get filename of file to create. Is found file was *.xc1, we create *.xc2. And visa-versa
  bufFilename[filenameLen - 1] = '1';
  if (foundFiles & 0x01) {
    bufFilename[filenameLen - 1] = '2';
  }
  File f = SPIFFS.open(bufFilename, "w");
  if (!f) {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_ERR)
    if (mxConfDebugLevel > 1) {
      mxConfDebug->println(F("Error creating file for writing config data to"));
    }
    #endif
    return false;
  }
  #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
  if (mxConfDebugLevel > 2) {
    mxConfDebug->print(F("Writing config to "));
    mxConfDebug->println(bufFilename);
  }
  #endif

  
  ///////////////////////////////////////////////
  // Calculate new start and end "Rolling Number". Read "Rolling Number" from old file, and increment it
  uint16_t oldStartRollingNumber = 0;
  if (foundFiles != 0) {
    oldFileExtNum = '1';
    if (foundFiles & 0x02) {
      oldFileExtNum = '2';
    }
    bufFilename[filenameLen - 1] = oldFileExtNum;
    File fOld = SPIFFS.open(bufFilename, "r");
    if (fOld) {
      fOld.seek(offsetof(MxConfigFsHdr, startRollingNumber), fs::SeekSet);
      fOld.read((uint8_t*)&oldStartRollingNumber, sizeof(oldStartRollingNumber));
    }
  }
  //Ensure "Start Rolling Number" is a even number
  if ((oldStartRollingNumber & 0x01) != 0) {
    oldStartRollingNumber++;
  }
  pConfInfo->hdr.startRollingNumber = oldStartRollingNumber + 2;
  pConfInfo->endRollingNumber = pConfInfo->hdr.startRollingNumber + 1;


  ///////////////////////////////////////////////
  // Calculate Checksum - TODO
  pConfInfo->hdr.checksum = 0;

  
  ///////////////////////////////////////////////
  // Write File Header
  f.write((const uint8_t *)&pConfInfo->hdr, sizeof(pConfInfo->hdr));


  ///////////////////////////////////////////////
  // Write File Data
  // - 32-Bit Data - 4 Byte Alligned
  // - 16-Bit Data - 2 Byte Alligned
  // - 8-Bit Data - 2 Byte Alligned
  // - Strings - 2 Byte Alligned
  // - Flags - 2 Byte Alligned
#if(0)
  //Write ALL data to file.
  f.write((const uint8_t *)pConfInfo->pMxConfStruct, pConfInfo->mxConfStructSize);
#else
  //Write 32-Bit Variables
  if (pConfInfo->hdr.offsetU16 > 0) {
    f.write((const uint8_t *)&pConfInfo->pMxConfStruct[0], pConfInfo->hdr.offsetU16);
  }
  //Write 16-Bit Variables
  i = pConfInfo->hdr.offsetU8 - pConfInfo->hdr.offsetU16;
  if (i > 0) {
    f.write((const uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.offsetU16], i);
  }
  //Write 8-Bit Variables
  if (pConfInfo->hdr.numberOfBytes > 0) {
    f.write((const uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.offsetU8], pConfInfo->hdr.numberOfBytes);
  }
  //Write strings
  i = pConfInfo->hdr.offsetFlags - pConfInfo->hdr.offsetStr;
  if (i > 0) {
    f.write((const uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.offsetStr], i);
  }

  // Write numberOfFlags variable, and actual flags. Calculate how many bytes contain data
  // - numberOfFlags is always present, and is always written to the config file
  // - The numberOfFlags variable indicates how many bits there are. Use following formula
  //   to calculate how many bytes are required to hold this many bits (0-8=1, 9-16=2, ...):
  //   = ((numberOfFlags - 1) / 8) + 1
  i = sizeof(pConfInfo->hdr.numberOfFlags);
  if (pConfInfo->hdr.numberOfFlags != 0) {
    i = i + ((pConfInfo->hdr.numberOfFlags-1) / 8) + 1;
  }
  f.write((const uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.numberOfFlags], i);
#endif


  // Write end rolling number ///////////////////////////////////////////////
  f.write((const uint8_t *)&pConfInfo->endRollingNumber, 2);

  //Close file when done
  f.close();


  ///////////////////////////////////////////////
  // Now that we have successfully written data to new file, delete old file
  if (oldFileExtNum != 0) {
    bufFilename[filenameLen - 1] = oldFileExtNum;
    SPIFFS.remove(bufFilename);
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) {
      mxConfDebug->print(F("Deleting old file "));
      mxConfDebug->println(bufFilename);
    }
    #endif
  }

#endif

  return true;
}



/**
* Load MxConfigInfo data from given file
*
* @param
*
* @preconditon
* Ensure FS has already been initialized before calling this function!
* For SPIFFS, we have to call SPIFFS.begin()
*/
bool mxConfLoad(MxConfigInfo* pConfInfo, const char* filename) {
  uint16_t i, datasize, offset1, offset2, filenameLen;
  uint8_t foundFiles = 0;   // Bit 1 for *.xc1 file, bit 2 for *.xc2 file
  char oldFileExtNum = 0;   // Is '1' if old file is *.xc1, '2' if it is *.xc2, else 0
  char bufFilename[32];

  #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
  if (mxConfDebugLevel > 2) {
    mxConfDebug->print(F("Loading config file: "));
    mxConfDebug->println(filename);
  }
  #endif

  //For ESP8266/ESP32, "Modtronix Configuration" Stucture is stored in SPIFFS File System
#if defined(ESP8266) || defined(ESP32)


  ///////////////////////////////////////////////
  // Check if a file(s) with this filename already exists.

  //Check if *.xc1 or *.xc2 file exists
  bufFilename[0] = '/';
  strcpy(&bufFilename[1], filename);
  strcat(bufFilename, ".xc1");
  filenameLen = strlen(bufFilename);

  for (i = 0; i < 2; i++) {
    //Set extension to *.xc1 or *.xc2
    bufFilename[filenameLen - 1] = i + '1';

    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) {
      mxConfDebug->print(F("Searching for "));
      mxConfDebug->print(bufFilename);
    }
    #endif

    //Current file exists
    if (SPIFFS.exists(bufFilename)) {
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) mxConfDebug->println(F(" - Found!"));
      #endif

      //Confirm it is valid
      if (checkIfValidFile(bufFilename) == false) {
        //NOT VALID!! Delete it, and contineu to next file
        SPIFFS.remove(bufFilename);

        #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
        if (mxConfDebugLevel > 2) {
          mxConfDebug->println(F(" - Deleted INVALID file!"));
        }
        else {
          if (mxConfDebugLevel > 1) {
            mxConfDebug->println(F("Deleted INVALID configuration file!"));
          }
        }
        #endif

        continue;
      }

      //File IS valid
      foundFiles = foundFiles | (0x01 << i);
    }
    else {
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) mxConfDebug->println("");
      #endif
    }
  }

  ///////////////////////////////////////////////
  // If both files exist, delete oldest one
  if (foundFiles == 3) {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) mxConfDebug->println(F("Found 2 config file, deleting oldest one!"));
    #endif
    uint16_t f1StartRollingNumber, f2StartRollingNumber;

    //Open *.xc1 file, and read it's startRollingNumber
    bufFilename[filenameLen - 1] = '1';
    File f1 = SPIFFS.open(bufFilename, "r");
    if (f1) {
      f1.seek(offsetof(MxConfigFsHdr, startRollingNumber), fs::SeekSet);
      f1.read((uint8_t*)&f1StartRollingNumber, sizeof(f2StartRollingNumber));
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) {
        mxConfDebug->print(F("startRollingNumber for *.xc1 = "));
        mxConfDebug->println(f1StartRollingNumber);
      }
      #endif
    }

    //Open *.xc2 file, and read it's startRollingNumber
    bufFilename[filenameLen - 1] = '2';
    File f2 = SPIFFS.open(bufFilename, "r");
    if (f2) {
      f2.seek(offsetof(MxConfigFsHdr, startRollingNumber), fs::SeekSet);
      f2.read((uint8_t*)&f2StartRollingNumber, sizeof(f2StartRollingNumber));
      #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
      if (mxConfDebugLevel > 2) {
        mxConfDebug->print(F("startRollingNumber for *.xc2 = "));
        mxConfDebug->println(f2StartRollingNumber);
      }
      #endif
    }

    //Delete oldest file (with smallest startRollingNumber
    if (f1StartRollingNumber > f2StartRollingNumber) {
      bufFilename[filenameLen - 1] = '2';         // Delete *.xc2 file
      foundFiles = foundFiles & ~(0x02);  // Clear bit for *.xc2 file
    }
    else {
      bufFilename[filenameLen - 1] = '1';         // Delete *.xc1 file
      foundFiles = foundFiles & ~(0x01);  // Clear bit for *.xc1 file
    }
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
    if (mxConfDebugLevel > 2) {
      mxConfDebug->print(F("Deleting "));
      mxConfDebug->println(bufFilename);
    }
    #endif
    SPIFFS.remove(bufFilename);
  }


  ///////////////////////////////////////////////
  // Open file for reading

  // Get filename of file to read.
  bufFilename[filenameLen - 1] = '1';
  if (foundFiles & 0x02) {
    bufFilename[filenameLen - 1] = '2';
  }
  File f = SPIFFS.open(bufFilename, "r");
  if (!f) {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_ERR)
    if (mxConfDebugLevel > 1) {
      mxConfDebug->println(F("Error opening config file for reading"));
    }
    #endif
    return false;
  }
  #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_INFO)
  if (mxConfDebugLevel > 2) {
    mxConfDebug->print(F("Reading config from "));
    mxConfDebug->println(bufFilename);
  }
  #endif


  // Read File Header ///////////////////////////////////////////////////////
  f.read((uint8_t*)&pConfInfo->hdr, sizeof(pConfInfo->hdr));


  // Read File Data /////////////////////////////////////////////////////////
  // - Strings - 2 Byte Alligned
  // - 32-Bit Data - 4 Byte Alligned
  // - 16-Bit Data - 2 Byte Alligned
  // - 8-Bit Data - 2 Byte Alligned
  // - Flags - 2 Byte Alligned
#if(0)
  // Read ALL data from file.
  f.read(pConfInfo->pMxConfStruct, pConfInfo->mxConfStructSize);
#else
  //Read 32-Bit Variables
  if (pConfInfo->hdr.offsetU16 > 0) {
    f.read((uint8_t *)&pConfInfo->pMxConfStruct[0], pConfInfo->hdr.offsetU16);
  }
  //Read 16-Bit Variables
  i = pConfInfo->hdr.offsetU8 - pConfInfo->hdr.offsetU16;
  if (i > 0) {
    f.read((uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.offsetU16], i);
  }
  //Read 8-Bit Variables
  if (pConfInfo->hdr.numberOfBytes > 0) {
    f.read((uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.offsetU8], pConfInfo->hdr.numberOfBytes);
  }
  //Read strings
  i = pConfInfo->hdr.offsetFlags - pConfInfo->hdr.offsetStr;
  if (i > 0) {
    f.read((uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.offsetStr], i);
  }
  // Write numberOfFlags variable, and actual flags. Calculate how many bytes contain data
  // - numberOfFlags is always present, and is always written to the config file
  // - The numberOfFlags variable indicates how many bits there are. Use following formula
  //   to calculate how many bytes are required to hold this many bits (0-8=1, 9-16=2, ...):
  //   = ((numberOfFlags - 1) / 8) + 1
  i = sizeof(pConfInfo->hdr.numberOfFlags);
  if (pConfInfo->hdr.numberOfFlags != 0) {
    i = i + ((pConfInfo->hdr.numberOfFlags - 1) / 8) + 1;
  }
  f.read((uint8_t *)&pConfInfo->pMxConfStruct[pConfInfo->hdr.numberOfFlags], i);
#endif
  

  // Read end rolling number ////////////////////////////////////////////////
  f.read((uint8_t*)&pConfInfo->endRollingNumber, 2);

  //Close file when done
  f.close();
#endif

  //Check if file was valid
  if ((pConfInfo->hdr.startRollingNumber + 1) != pConfInfo->endRollingNumber) {
    #if (MXCONF_DEBUG_LEVEL >= MXCONF_DEBUG_LVL_ERR)
    if (mxConfDebugLevel > 1) {
      mxConfDebug->println(F("Error in config file (rolling number)!"));
      return false;
    }
    #endif
  }
  return true;
}
