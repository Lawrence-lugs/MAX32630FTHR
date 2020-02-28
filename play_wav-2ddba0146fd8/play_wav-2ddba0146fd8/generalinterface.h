#ifndef _GENERALINTERFACE_H_
#define _GENERALINTERFACE_H_

#include "USBSerial.h"
#include "SDFileSystem.h"
#include "USBMSD_BD.h"
#include "SDBlockDevice.h"
#include "HeapBlockDevice.h"
#include "FATFileSystem.h"

extern Serial daplink;
extern USBSerial microUSB;
extern SDBlockDevice bd;
extern FATFileSystem fs;
extern USBMSD_BD msd;

/*
    Cleans up any left over characters sent via the daplink serial stream
*/
void clearSerialStream();

/*
    Initializes the USB file system visible to the computer (routes files to SD card)
*/
void startFileSystem();

/*
    Obtains input from the DAPLINK TERMINAL via getc() and echoes it back to the puTTY terminal via putc()
    this blocks program flow until a CARRIAGE RETURN ( \r ) is recieved from the PC.
*/
bool getInput(int maxSize,char *inputArray);

#endif