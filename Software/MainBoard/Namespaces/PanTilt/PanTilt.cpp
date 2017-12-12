//**************************************************************/
// TITLE
//**************************************************************/

//!t PanTilt.cpp
//!a RBE-2002 B17 Team 10

#include "PanTilt.h"
#include "OpenLoopServo.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace PanTilt {

	// Arduino Pin Settings
	const uint8_t PIN_PAN = 6;
	const uint8_t PIN_TILT = 7;

	// Servo Constants
	const int SERVO_SIGNAL_MIN = 400;		// microseconds
	const int SERVO_SIGNAL_MAX = 2500;		// microseconds
	const float SERVO_VELOCITY_MAX = 3.927;	// rad/s

	// Pan System
	const float PAN_MIN = 0.000;	// rad
	const float PAN_MAX = PI/2.0;	// rad
	const float PAN_VEL = PI/2.0;	// rad/s

	float pan = 0;
	OpenLoopServo panServo(
		PIN_PAN,
		SERVO_SIGNAL_MIN,
		SERVO_SIGNAL_MAX,
		+2.460914,
		-2.591814,
		SERVO_VELOCITY_MAX);

	enum {
		PAN_RIGHT,
		PAN_LEFT,
	} panState;

	// Tilt System
	const float TILT_MIN = -PI/6.0;		// rad
	const float TILT_MAX = +PI/4.0;		// rad
	const float TILT_VEL = 3.5;	// rad/s

	float tilt = 0;
	OpenLoopServo tiltServo(
		PIN_TILT,
		SERVO_SIGNAL_MIN,
		SERVO_SIGNAL_MAX,
		+2.565634,
		-2.434734,
		SERVO_VELOCITY_MAX);

	enum {
		TILT_UP,
		TILT_DOWN,
	} tiltState;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes sweep state machines and servos to 0 angles.
void PanTilt::setup() {

	// Initialize servos and fan
	panServo.setup(0);
	tiltServo.setup(0);

	// Set up pan state machine
	panServo.setVelocity(PAN_VEL);
	panServo.setAngle(PAN_MAX);
	panState = PAN_RIGHT;

	// Set up tilt state machine
	tiltServo.setVelocity(TILT_VEL);
	tiltServo.setAngle(TILT_MAX);
	tiltState = TILT_UP;
}

//!b Runs open-loop servo loop functions.
//!d Also updates pan and tilt variables from servo angles.
//!d Call this method in the main loop always.
void PanTilt::loop() {
	pan = panServo.loop();
	tilt = tiltServo.loop();
}

//!b Iterates through flame finder sweep state machine.
void PanTilt::sweep() {

	// Pan state machine
	switch(panState) {
		case PAN_RIGHT:	// Panning right
			if(panServo.atTargetAngle()) {
				panServo.setAngle(PAN_MIN);
				panState = PAN_LEFT;
			}
			break;

		case PAN_LEFT:	// Panning left
			if(panServo.atTargetAngle()) {
				panServo.setAngle(PAN_MAX);
				panState = PAN_RIGHT;
			}
			break;
	}

	// Tilt state machine
	switch(tiltState) {
		case TILT_UP:	// Tilting up
			if(tiltServo.atTargetAngle()) {
				tiltServo.setAngle(TILT_MIN);
				tiltState = TILT_DOWN;
			}
			break;

		case TILT_DOWN:	// Tilting down
			if(tiltServo.atTargetAngle()) {
				tiltServo.setAngle(TILT_MAX);
				tiltState = TILT_UP;
			}
			break;
	}
}

//!b Directs pan servo to rotate to given angle (rad).
void PanTilt::setPan(float p) {
	panServo.setAngle(p);
}

//!b Directs tilt serv to rotate to given angle (rad).
void PanTilt::setTilt(float t) {
	tiltServo.setAngle(t);
}

//!b Stops tilt servo at its current angle.
void PanTilt::stopTilt() {
	tiltServo.stop();
}

//!b Returns true if pan and tilt servos are at target angles.
bool PanTilt::isAimed() {
	return
		panServo.atTargetAngle() &&
		tiltServo.atTargetAngle();
}
