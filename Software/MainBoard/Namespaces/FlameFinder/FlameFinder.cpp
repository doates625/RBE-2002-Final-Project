//**************************************************************/
// TITLE
//**************************************************************/

//!t FlameFinder.cpp
//!a RBE-2002 B17 Team 10

#include "FlameFinder.h"
#include "Arduino.h"
#include "OpenLoopServo.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace FlameFinder {

	// Servo constants
	const int SERVO_SIGNAL_MIN = 400;		// microseconds
	const int SERVO_SIGNAL_MAX = 2500;		// microseconds
	const float SERVO_ANGLE_MIN = +2.400;	// rad
	const float SERVO_ANGLE_MAX = -2.574;	// rad
	const float SERVO_VELOCITY_MAX = 3.927;	// rad/s

	// Pan System
	const uint8_t PIN_PAN = 2;
	const float PAN_MIN = -HALF_PI;			// rad
	const float PAN_MAX = +HALF_PI;			// rad
	const float PAN_VELOCITY = 0.785; 		// rad/s

	float pan = 0;
	OpenLoopServo panServo(
		PIN_PAN,
		SERVO_SIGNAL_MIN,
		SERVO_SIGNAL_MAX,
		SERVO_ANGLE_MIN,
		SERVO_ANGLE_MAX,
		SERVO_VELOCITY_MAX);

	enum {
		PAN_RIGHT,
		PAN_LEFT,
	} panState;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes all motors and sensors for flame finder.
void FlameFinder::setup() {
	panServo.setup(PAN_MIN);
	delay(2000);
	panServo.setVelocity(PAN_VELOCITY);
	panServo.setAngle(PAN_MAX);
	panState = PAN_RIGHT;
}

//!b Iterates through flame finder state machine.
void FlameFinder::loop() {
	pan = panServo.loop();
	switch(panState) {

		// Panning right
		case PAN_RIGHT:
			if(panServo.atTargetAngle()) {
				panServo.setAngle(PAN_MIN);
				panState = PAN_LEFT;
			}
			break;

		// Panning left
		case PAN_LEFT:
			if(panServo.atTargetAngle()) {
				panServo.setAngle(PAN_MAX);
				panState = PAN_RIGHT;
			}
			break;
	}
}
