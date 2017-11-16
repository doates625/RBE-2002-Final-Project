//*************************************************************//
// TITLE
//*************************************************************//

//!t SonarBoard.ino
//!b Software for RBE-2002 final project sonar board.
//!a RBE-2002 B17 Team 10

//!d This board pings 4 HcSr04 sonar sensors and sends the
//!d distance data to the main board via serial.

#include "Arduino.h"
#include "HcSr04.h"
#include "BinarySerial.h"
#include "Led.h"

//*************************************************************//
// ARDUINO WIRING FORMAT
//*************************************************************//

const uint8_t PIN_SONAR_F_T = 10;
const uint8_t PIN_SONAR_F_E = 9;
const uint8_t PIN_SONAR_B_T = 8;
const uint8_t PIN_SONAR_B_E = 7;
const uint8_t PIN_SONAR_L_T = 6;
const uint8_t PIN_SONAR_L_E = 5;
const uint8_t PIN_SONAR_R_T = 12;
const uint8_t PIN_SONAR_R_E = 11;
const uint8_t PIN_LED = 13;

//*************************************************************//
// SERIAL PROTOCOL
//*************************************************************//

// Port Settings
HardwareSerial*
	PORT = &Serial;
const unsigned long
	BAUD = 115200;

// Communication Settings
const byte
	BYTE_SONAR = 0x01;

// Communication Objects
BinarySerial bs(*PORT, BAUD);

//*************************************************************//
// OBJECT DEFINITIONS
//*************************************************************//

Led led(PIN_LED);

HcSr04 sonarF(PIN_SONAR_F_T, PIN_SONAR_F_E);
HcSr04 sonarB(PIN_SONAR_B_T, PIN_SONAR_B_E);
HcSr04 sonarL(PIN_SONAR_L_T, PIN_SONAR_L_E);
HcSr04 sonarR(PIN_SONAR_R_T, PIN_SONAR_R_E);

//*************************************************************//
// GLOBAL SONAR DISTANCE VARIABLES
//*************************************************************//

float distF = 0;
float distB = 0;
float distL = 0;
float distR = 0;

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Flashes LED to indicate an error
void flashLed(uint8_t n) {
	while(1) {
		for(uint8_t i=1; i<=n; i++) {
			led.on(); delay(100);
			led.off(); delay(250);
		}
		delay(1000);
	}
}

//!b Executes once on Arduino reset.
//!d Tasks:
//!d - Initialize LED and sonar sensors
//!d - Initialize serial communication
//!d - Wait for start byte from main board
void setup() {

	// Initialize LED and sonar sensors
	led.setup();
	sonarF.setup();
	sonarB.setup();
	sonarL.setup();
	sonarR.setup();

	// Initialize serial communication
	bs.setup();

	// Wait for sonar byte from main board
	bs.wait();
	if(bs.readByte() == BYTE_SONAR) {
		led.on();
		bs.writeByte(BYTE_SONAR);
		return;
	} else
		flashLed(1);
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - Gather ultrasonic distance data
//!d - Notify main board that data is ready
//!d - Send ultrasonic data when prompted by main board
void loop() {

	// Collect sonar data
	distF = sonarF.dist();
	distB = sonarB.dist();
	distL = sonarL.dist();
	distR = sonarR.dist();

	// Notify main board of readiness
	bs.writeByte(BYTE_SONAR);

	// Wait for response to send data
	bs.wait();
	if(bs.readByte() == BYTE_SONAR) {
		bs.writeFloat(distF);
		bs.writeFloat(distB);
		bs.writeFloat(distL);
		bs.writeFloat(distR);
	} else
		flashLed(2);
}
