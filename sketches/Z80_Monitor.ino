// Z80 Monitor Program for Breadboard Z80 Project
// 
// Author:  `       Dean Belfield (@BreakIntoProg)
// Project Website: breakintoprogram.co.uk
// Target Board:    STM32 (Blue Pill)
// Created:         05/05/2020
// Last Updated:    05/05/2020
//
// Updates:
//

#include <Arduino.h>
#include <LiquidCrystal.h>

// Constants
//
const int pinLed =  LED_BUILTIN;			// The built-in LED pin
//
// LCD Interface
//
const int pinLCD_RS = PB1;
const int pinLCD_Enable = PB0;
const int pinLCD_D0 = PA7;
const int pinLCD_D1 = PA6;
const int pinLCD_D2 = PA5;
const int pinLCD_D3 = PA4;
const int pinLCD_D4 = PA3;
const int pinLCD_D5 = PA2;
const int pinLCD_D6 = PA1;
const int pinLCD_D7 = PA0;

//
// Z80 Interface
//
const int pinIOREQ = PB12;						// The Z80 IOREQ pin
const int pinWR = PB13;
const int pinRD = PB14;
const int pinA2 = PB15;								// The address register`
const int pinA1 = PA8;
const int pinA0 = PA9;
const int pinD7 = PA10;								// The Z80 data lines
const int pinD6 = PA15;
const int pinD5 = PB3;
const int pinD4 = PB4;
const int pinD3 = PB6;
const int pinD2 = PB7;
const int pinD1 = PB8;
const int pinD0 = PB9;

volatile int ioCount = 0;							// An IO counter
volatile byte ioRegister = 0;					// The IO register (read from the Z80 address pins)
volatile byte ioDataRD[8] = {					// The IO data read from the Z80
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};				
volatile byte ioDataWR[8] = {					// The IO data to write back to the Z80
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
volatile boolean ioRead = false;
volatile boolean ioWrite = false;

char lcdBuffer0[17] = {};
char lcdBuffer1[17] = {};

String output = "";

// Variables
//
int previousCount = 0;

// Initialise libraries
//
LiquidCrystal lcd(pinLCD_RS, pinLCD_Enable, pinLCD_D0, pinLCD_D1, pinLCD_D2, pinLCD_D3, pinLCD_D4, pinLCD_D5, pinLCD_D6, pinLCD_D7);

// Setup the board
//
void setup() {
	Serial.begin(9600);									// Set up the serial comms
	//
	// Set up the digital pins
	//
  pinMode(pinLed, OUTPUT);						// The LED
  pinMode(pinIOREQ, INPUT);						// The IOREQ pin
  pinMode(pinWR, INPUT);							// The RD pin
  pinMode(pinRD, INPUT);							// The WR pin
  pinMode(pinA0, INPUT);							// The three register pins
  pinMode(pinA1, INPUT);						
  pinMode(pinA2, INPUT);							
  setDataDirection(INPUT);						// The eight data pins
  //
  // Attach interrupts to the IOREQ pin
  //
  attachInterrupt(digitalPinToInterrupt(pinIOREQ), ioreqInterrupt, FALLING);

	lcd.begin(16, 2);										// Set up the LCD
	lcd.setCursor(0, 0);
	lcd.print("ARW DIO");
}

// The main loop
//
void loop() {
	if(ioCount != previousCount) {
		previousCount = ioCount;
		switch(ioRegister) {
			case 0:	// Serial I/O
				ioMode0();
				break;
			case 7:	// Control
				ioMode7();
				break;
		}
  	sprintf(lcdBuffer1, "%u%c%c %02X=%s", ioRegister, ioRead ? '*' : '-', ioWrite ? '*' : '-', ioDataRD[ioRegister], output.c_str());
  	ioWrite = false;
  	ioRead = false;
  	digitalWrite(pinLed, (ioCount & 1));
  	lcd.setCursor(0,1);
  	lcd.print(lcdBuffer1);
  }
}

// IO mode 0
//
void ioMode0() {
	if(ioWrite) {
		if(Serial.available() > 0) { 
			ioDataWR[ioRegister] = Serial.read();
		}
		else {
			ioDataWR[ioRegister] = 0;
		}
	}
	if(ioRead) {
  	byte b = ioDataRD[ioRegister];
		Serial.write(b);
 		if(b < 32) {
  		b = 0xA5;
 		}
		output += char(b);
		if(output.length() > 9) {
  		output.remove(0, 1);
  	}
	}
}

// Set data pin direction
// 
void setDataDirection(WiringPinMode mode) {
  pinMode(pinD0, mode);
  pinMode(pinD1, mode);
  pinMode(pinD2, mode);
  pinMode(pinD3, mode);
  pinMode(pinD4, mode);
  pinMode(pinD5, mode);
  pinMode(pinD6, mode);
  pinMode(pinD7, mode);
}

// IO mode 7
//
void ioMode7() {
}

// The IOREQ interrupt handler
//
void ioreqInterrupt() {
	ioRegister = (digitalRead(pinA0)) | (digitalRead(pinA1) << 1) | (digitalRead(pinA2) << 2);
	ioWrite = (digitalRead(pinWR) == LOW);
	ioRead = (digitalRead(pinRD) == LOW);
	if(ioRead) {
		ioDataRD[ioRegister] = (digitalRead(pinD0)) | (digitalRead(pinD1) << 1) | (digitalRead(pinD2) << 2) | (digitalRead(pinD3) << 3) | (digitalRead(pinD4) << 4) | (digitalRead(pinD5) << 5) | (digitalRead(pinD6) << 6) | (digitalRead(pinD7) << 7);
	}
	if(ioWrite) {
		byte data = ioDataWR[ioRegister];
		setDataDirection(OUTPUT);
		digitalWrite(pinD0, data & 0x01 ? HIGH : LOW);
		digitalWrite(pinD1, data & 0x02 ? HIGH : LOW);
		digitalWrite(pinD2, data & 0x04 ? HIGH : LOW);
		digitalWrite(pinD3, data & 0x08 ? HIGH : LOW);
		digitalWrite(pinD4, data & 0x10 ? HIGH : LOW);
		digitalWrite(pinD5, data & 0x20 ? HIGH : LOW);
		digitalWrite(pinD6, data & 0x40 ? HIGH : LOW);
		digitalWrite(pinD7, data & 0x80 ? HIGH : LOW);
		setDataDirection(INPUT);
	}
	ioCount++;
}
