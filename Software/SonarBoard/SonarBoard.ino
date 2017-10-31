//*************************************************************//
// TITLE
//*************************************************************//

//!t SonarBoard.ino
//!b Software for RBE-2002 final project sonar board.
//!a RBE-2002 B17 Team 10

#include "Arduino.h"
#include "HcSr04.h"
#include "BinarySerial.h"
#include "Led.h"

//*************************************************************//
// ARDUINO WIRING FORMAT
//*************************************************************//

const uint8_t PIN_SONAR_F_T = 11;
const uint8_t PIN_SONAR_F_E = 9;
const uint8_t PIN_SONAR_B_T = 10;
const uint8_t PIN_SONAR_B_E = 8;
const uint8_t PIN_LED = 13;

//*************************************************************//
// SERIAL PROTOCOL DEFINITIONS
//*************************************************************//

HardwareSerial* serial = &Serial;
const unsigned long BAUD = 115200;

const byte BYTE_BEGIN = 0x42; // "B"
const byte BYTE_READY = 0x52; // "R"

//*************************************************************//
// OBJECT DEFINITIONS
//*************************************************************//

Led led(PIN_LED);

HcSr04 sonarF(PIN_SONAR_F_T, PIN_SONAR_F_E);
HcSr04 sonarB(PIN_SONAR_B_T, PIN_SONAR_B_E);

BinarySerial binSerial(*serial, BAUD);

//*************************************************************//
// GLOBAL VARIABLE INSTANTIATIONS
//*************************************************************//

float distF = 0;
float distB = 0;

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
//!d Tasks:
//!d - Initialize ultrasonic sensors
//!d - Initialize serial communication
//!d - Wait for start byte ("B")
void setup() {
	led.setup();
	sonarF.setup();
	sonarB.setup();

	binSerial.setup();
	binSerial.wait();
	if(binSerial.readByte() != BYTE_BEGIN)
		while(1) {
			led.on(); delay(100);
			led.off(); delay(400);
		}
	serial->print("Beginning");
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - Gather ultrasonic distance data
//!d - Wait for request from serial ("R")
//!d - Print ultrasonic data to serial
void loop() {
	distF = sonarF.dist();
	distB = sonarB.dist();
	serial->println("F: " + String(distF));
	serial->println("B: " + String(distB));
	delay(250);
}
