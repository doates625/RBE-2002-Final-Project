//**************************************************************/
// TITLE
//**************************************************************/

//!t SonarComms.cpp
//!a RBE-2002 B17 Team 10

#include "Sonar.h"
#include "RobotDims.h"
#include "HcSr04Array.h"
#include "PinChangeInt.h"
#include "Timer.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace Sonar {

	// Arduino Pin Settings
	const uint8_t PIN_TRIG_F = 42;
	const uint8_t PIN_TRIG_B = 43;
	const uint8_t PIN_TRIG_L = 44;
	const uint8_t PIN_TRIG_R = 45;
	const uint8_t PIN_ECHO_F = A12;
	const uint8_t PIN_ECHO_B = A13;
	const uint8_t PIN_ECHO_L = A14;
	const uint8_t PIN_ECHO_R = A15;

	// Sonar Distance Variables
	float distF = 0;
	float distB = 0;
	float distL = 0;
	float distR = 0;

	// Sonar Array Object
	HcSr04Array sensors(4,
		new uint8_t[4]{
			PIN_TRIG_F,
			PIN_TRIG_B,
			PIN_TRIG_L,
			PIN_TRIG_R},
		new uint8_t[4]{
			PIN_ECHO_F,
			PIN_ECHO_B,
			PIN_ECHO_L,
			PIN_ECHO_R});
	bool sonarBegun = false;

	// Private Function Templates
	void isr();
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes sonar sensors.
//!d Call this method in the main setup function.
void Sonar::setup() {
	for(uint8_t i=1; i<=4; i++) {
		attachPinChangeInterrupt(
			sensors.getEchoPinId(i),
			isr,
			CHANGE);
	}
	sensors.setup();
}

//!b Updates sonar distance variables.
//!d Call this method in the main loop function.
void Sonar::loop() {
	if(sonarBegun) {
		switch(sensors.loop()) {

			// No sensors updated
			case 0: break;

			// Front sensor updated
			case 1:
				distF = sensors.get(1);
				if(distF != 0) {
					distF += RobotDims::sonarRadiusF;
				}
				break;

			// Back sensor updated
			case 2:
				distB = sensors.get(2);
				if(distB != 0) {
					distB += RobotDims::sonarRadiusB;
				}
				break;

			// Left sensor updated
			case 3:
				distL = sensors.get(3);
				if(distL != 0) {
					distL += RobotDims::sonarRadiusL;
				}
				break;

			// Right sensor updated
			case 4:
				distR = sensors.get(4);
				if(distR != 0) {
					distR += RobotDims::sonarRadiusR;
				}
				break;

		}
	} else {
		sensors.begin();
		sonarBegun = true;
	}
}

//!b Performs interrupt service routine for sonar.
void Sonar::isr() {
	sensors.isr();
}

//!b Sets up sonar and prints out values on Serial 115200.
void Sonar::serialTest() {
	setup();
	Serial.begin(115200);
	Serial.println("Sonar Test");

	Timer timer;
	timer.tic();

	while(1) {
		loop();
		if(timer.hasElapsed(0.2)) {
			timer.tic();
			Serial.println();
			Serial.println("F: " + String(distF));
			Serial.println("B: " + String(distB));
			Serial.println("L: " + String(distL));
			Serial.println("R: " + String(distR));
		}
	}
}
