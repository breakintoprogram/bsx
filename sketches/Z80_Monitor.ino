// Z80 Monitor Program for Breadboard Z80 Project
// 
// Author:  `       Dean Belfield (@BreakIntoProg)
// Project Website: breakintoprogram.co.uk
// Schematic:				Z80 Breadboard Computer #3.2
// Target Board:    STM32 (Blue Pill)
// Created:         05/05/2020
// Last Updated:    20/05/2020
//
// Updates:
//
// 20/05/2020:			Now uses hd44780 library for LCD control
//									Serial comms now in timerInterrupt and bit 0 of ioDataRD[7] now indicates serial data available
//									Z80 pins reordered and all Z80 IO read/written directly to STM32 GPIO registers for speed

#include <Arduino.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h> // Arduino pin i/o class header

// Constants
//
const int pinLed =  LED_BUILTIN;			// The built-in LED pin
//
// LCD Interface
//
const int pinLCD_RS = PB1;
const int pinLCD_Enable = PB0;
const int pinLCD_D4 = PA3;
const int pinLCD_D5 = PA2;
const int pinLCD_D6 = PA1;
const int pinLCD_D7 = PA0;

// Z80 Interface

// Control pins
//
const int pinIOREQ = PA15;	// Was PB12
const int pinRD = PB4; 			// Was PB13 
const int pinWR = PB3; 			// Was PB14 

// Address pins
//
const int pinA2 = PA10; 		// Was PB15
const int pinA1 = PA9; 			// Was PA8;
const int pinA0 = PA8; 			// Was PA9;

// Data pins
//
const int pinD7 = PB15; 		// Was PA10
const int pinD6 = PB14; 		// Was PA15
const int pinD5 = PB13; 		// Was PB3
const int pinD4 = PB12;			// Was PB4
const int pinD3 = PB9; 			// Was PB6
const int pinD2 = PB8; 			// Was PB7
const int pinD1 = PB7; 			// Was PB8
const int pinD0 = PB6; 			// Was PB9

// Variables used by interrupt routine
//
volatile int ioCount = 0;							// An IO counter
volatile int ioCountPrevious = 0;
volatile byte ioRegister = 0;					// The IO register (read from the Z80 address pins)
volatile byte ioDataRD[8] = {					// The IO data to send back to the Z80 (Z80 is reading the port)
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
volatile byte ioDataWR[8] = {					// The IO data sent from the Z80 (Z80 has written to a port)
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
volatile boolean ioRead = false;
volatile boolean ioWrite = false;

String output = "";

// Initialise libraries
//
hd44780_pinIO lcd(pinLCD_RS, pinLCD_Enable, pinLCD_D4, pinLCD_D5, pinLCD_D6, pinLCD_D7);

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
	GPIOB->regs->CRL = (GPIOB->regs->CRL & 0x00FFFFFF) | 0x88000000;
	GPIOB->regs->CRH = (GPIOB->regs->CRH & 0x0000FF00) | 0x88880088;
  //
  // Attach interrupts to the IOREQ pin
  //
  attachInterrupt(digitalPinToInterrupt(pinIOREQ), ioreqInterrupt, FALLING);
  //
  //
  // Initialise the LCD
	lcd.begin(16, 2);										// Set up the LCD
	lcd.setCursor(0, 0);
	lcd.print("ARW DIO");
  //
  // Setup a hardware timer for the LCD
  //
  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(100); // In microseconds
  Timer2.setCompare(TIMER_CH1, 1);
  Timer2.attachInterrupt(TIMER_CH1, timerInterrupt);
}

// The main loop
//
void loop() {
	char buffer[17];
 	sprintf(buffer, "%u%c%c &%02X%s", ioRegister, ioRead ? '*' : '-', ioWrite ? '*' : '-', ioDataRD[ioRegister], output.c_str());
	lcd.setCursor(0,1);
	lcd.print(buffer);
	digitalWrite(pinLed, (ioCount & 0x80));
	delay(10);
}

// The timer interrupt
//
void timerInterrupt() {
	if(ioCount != ioCountPrevious) {
		ioCountPrevious = ioCount;
		switch(ioRegister) {
			case 0: ioMode0(); break;
			case 7: ioMode7(); break;
		}
		ioRead = false;
		ioWrite = false;
  }
}

// IO mode 0
//
void ioMode0() {
	if(ioRead) {
		if(Serial.available() > 0) {
			ioDataRD[ioRegister]  = Serial.read();
		}
		else {
			ioDataRD[ioRegister]  = 0;
		}
	}
	if(ioWrite) {
		byte b = ioDataWR[ioRegister];
		Serial.write(b);
		output += char(b < 32 ? 0xA5 : b);
		if(output.length() > 9) {
			output.remove(0, 1);
		}
	}
}

void ioMode7() {
	if(ioRead) {
		if(Serial.available() > 0) {
			ioDataRD[ioRegister] |= 0x01;
		}
		else {			
			ioDataRD[ioRegister] &= 0xFE;
		}
	}
}

// The IOREQ interrupt handler
//
void ioreqInterrupt() {
	unsigned int PortA = GPIOA->regs->IDR;
	unsigned int PortB = GPIOB->regs->IDR;
	
	ioRegister = (PortA & 0x0700) >> 8;	// PA10,PA9,PA8
	ioWrite = (PortB & 0x0008) == 0; 		// PB3 
	ioRead =  (PortB & 0x0010) == 0; 		// PB4
	//
	// Data pins D7-D0 are PB15,PB14,PB13,PB12 and PB9,PB8,PB7,PB6
	//
	if(ioWrite /*&& (ioDataFL[ioRegister] & 0x02)*/) {
		ioDataWR[ioRegister] = ((PortB & 0x03C0) >> 6) | ((PortB & 0xF000) >> 8);
	}
	if(ioRead) {
		byte data = ioDataRD[ioRegister]; 
		GPIOB->regs->CRL = (GPIOB->regs->CRL & 0x00FFFFFF) | 0x33000000;
		GPIOB->regs->CRH = (GPIOB->regs->CRH & 0x0000FF00) | 0x33330033;
		GPIOB->regs->BSRR = 0xF3C00000 | ((data & 0x0F) << 6) | ((data & 0xF0) << 8);
		GPIOB->regs->CRL = (GPIOB->regs->CRL & 0x00FFFFFF) | 0x88000000;
		GPIOB->regs->CRH = (GPIOB->regs->CRH & 0x0000FF00) | 0x88880088;
	}
	ioCount++;
}
