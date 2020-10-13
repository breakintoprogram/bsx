// Z80 Monitor Program for Breadboard Z80 Project
// 
// Author:  `       Dean Belfield (@BreakIntoProg)
// Project Website: breakintoprogram.co.uk
// Schematic:				Z80 Breadboard Computer #3.2
// Target Board:    STM32 (Blue Pill)
// Created:         05/05/2020
// Last Updated:    08/10/2020
//
// Updates:
//
// 20/05/2020:			Now uses hd44780 library for LCD control
//									Serial comms now in timerInterrupt and bit 0 of ioDataRD[7] now indicates serial data available
//									Z80 pins reordered and all Z80 IO read/written directly to STM32 GPIO registers for speed
// 28/05/2020:			Added support for SD card
// 08/10/2020:			Added support for the BSX 85C50A UART; Removed direct link to Z80

#include <Arduino.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h> // Arduino pin i/o class header
#include <SPI.h>
#include <SD.h>

// Constants
//
const int pinLed =  LED_BUILTIN;			// The built-in LED pin
const int pinCS = PA4;								// CS for the SD card

//
// LCD Interface
//
const int pinLCD_RS = PB1;
const int pinLCD_Enable = PB0;
const int pinLCD_D4 = PA3;
const int pinLCD_D5 = PA2;
const int pinLCD_D6 = PA1;
const int pinLCD_D7 = PA0;

// Variables used by interrupt routine
//
volatile int ioCount = 0;							// An IO counter
volatile boolean hasSD = false;				// An SD card has been inserted

String output = "";										// LCD output (bottom half of screen)
String sdPath = "/";									// The current directory

File sdFile;													// The current file

// Initialise libraries
//
hd44780_pinIO lcd(pinLCD_RS, pinLCD_Enable, pinLCD_D4, pinLCD_D5, pinLCD_D6, pinLCD_D7);

// Setup the board
//
void setup() {
	Serial.begin(9600);									// Set up the serial comms to the PC
	Serial3.begin(9600);					  		// Set up the serial comms to the 82C50A UART (PB10/PB11)
/*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
	}	
*/	
 	//
	// Set up the digital pins
	//
  pinMode(pinLed, OUTPUT);						// The LED
  pinMode(pinCS, OUTPUT);							// The SD Card CS
  //
  //
  // Initialise the LCD
	lcd.begin(16, 2);										// Set up the LCD
	//
	// Initialise the SD Card
	//
	hasSD = SD.begin(pinCS);						// Initialise the SD card
}

// The main loop
//
void loop() {
	byte b;
	boolean dataFlag = false;
	
	if(Serial.available() > 0) {
		b = Serial.read();
		Serial3.write(b);
		ioCount++;
		dataFlag = true;
	}
	while(Serial3.available() > 0) {
		b = Serial3.read();
		Serial.write(b);
  	output += char(b < 32 ? 0xA5 : b);
  	if(output.length() > 16) {
  		output.remove(0, 1);
  	}
		ioCount++;
		dataFlag = true;
	}
  digitalWrite(pinLed, dataFlag ? ioCount & 0x80 : false);
  lcd.setCursor(0,0);
  lcd.print(output);
  delay(1);
}

//
// List folder contents of SD card
//
void getDirectory(File dir, int numTabs, boolean recurse) {
  while(true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;											
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      if(recurse) {
      	getDirectory(entry, numTabs + 1, recurse);
      }
    }
    else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
