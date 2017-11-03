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

const uint8_t PIN_SONAR_F_T = 2; // FILL IN
const uint8_t PIN_SONAR_F_E = 3;
const uint8_t PIN_SONAR_B_T = 0;
const uint8_t PIN_SONAR_B_E = 0;
const uint8_t PIN_SONAR_L_T = 0;
const uint8_t PIN_SONAR_L_E = 0;
const uint8_t PIN_SONAR_R_T = 0;
const uint8_t PIN_SONAR_R_E = 0;
const uint8_t PIN_LED = 13;

//*************************************************************//
// SERIAL PROTOCOL DEFINITIONS
//*************************************************************//

HardwareSerial* serial = &Serial;
const unsigned long BAUD = 115200;

const byte BYTE_BEGIN = 0x42; // "B"
const byte BYTE_READY = 0x52; // "R"
const byte BYTE_DATA  = 0x44; // "D"

//*************************************************************//
// OBJECT DEFINITIONS
//*************************************************************//

Led led(PIN_LED);

HcSr04 sonarF(PIN_SONAR_F_T, PIN_SONAR_F_E);
HcSr04 sonarB(PIN_SONAR_B_T, PIN_SONAR_B_E);
HcSr04 sonarL(PIN_SONAR_L_T, PIN_SONAR_L_E);
HcSr04 sonarR(PIN_SONAR_R_T, PIN_SONAR_R_E);

BinarySerial bs(*serial, BAUD);

//*************************************************************//
// GLOBAL VARIABLE INSTANTIATIONS
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
//!d - Initialize ultrasonic sensors
//!d - Initialize serial communication
//!d - Wait for start byte ("B")
void setup() {
	led.setup();
	sonarF.setup();
	sonarB.setup();
	sonarL.setup();
	sonarR.setup();

	bs.setup();
	bs.wait();
	if(bs.readByte() == BYTE_BEGIN) {
		led.on();
		bs.writeByte(BYTE_READY);
		return;
	} else
		flashLed(1);
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - Gather ultrasonic distance data
//!d - Send ultrasonic data to serial.
void loop() {
	distF = sonarF.dist();
	distB = distF + 1; // sonarB.dist();
	distL = distF + 2; // sonarL.dist();
	distR = distF + 3; // sonarR.dist();

	if(bs.available()) {
		if(bs.readByte() == BYTE_READY) {
			bs.writeByte(BYTE_DATA);
			bs.writeFloat(distF);
			bs.writeFloat(distB);
			bs.writeFloat(distL);
			bs.writeFloat(distR);
		} else
			flashLed(2);
	}
}
