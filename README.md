# MxConfig
Modtronix Library for saving and restoring configuration data to File System. Currently supports ESP8266 and ESP32. Currently uses SPIFFS to save configuration data to a file in SPIFFS.

Designed to be very fast, and use as little as possible RAM. Configuration data is stored in a packed structure.

Features:
- Power failure during saving of data will not corrupt data. 
- Very low RAM usage.
- Configuration data stored in packed strucutres
- Newer firmware can upgrade old congifuration file to new format (new firmware might have new configuration variables).
