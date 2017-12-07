//**************************************************************/
// TITLE
//**************************************************************/

//!t FlameFinder.cpp
//!a RBE-2002 B17 Team 10

#include "FlameFinder.h"
#include "Odometer.h"
#include "OpenLoopServo.h"
#include "BrushlessMotor.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace FlameFinder {

	// Arduino Pin Settings
	const uint8_t PIN_PAN = 6;
	const uint8_t PIN_TILT = 7;
	const uint8_t PIN_FLAME = A0;
	const uint8_t PIN_FAN = 8;

	// Servo Constants
	const int SERVO_SIGNAL_MIN = 400;		// microseconds
	const int SERVO_SIGNAL_MAX = 2500;		// microseconds
	const float SERVO_VELOCITY_MAX = 3.927;	// rad/s

	// Pan System
	const float PSERVO_ANGLE_MIN = +2.460914;	// rad
	const float PSERVO_ANGLE_MAX = -2.591814;	// rad
	const float PAN_MIN = 0.000;	// rad
	const float PAN_MAX = 1.571;	// rad
	const float PAN_VEL = 1.571;	// rad/s

	float pan = 0;
	OpenLoopServo panServo(
		PIN_PAN,
		SERVO_SIGNAL_MIN,
		SERVO_SIGNAL_MAX,
		PSERVO_ANGLE_MIN,
		PSERVO_ANGLE_MAX,
		SERVO_VELOCITY_MAX);

	enum {
		PAN_IDLE,
		PAN_RIGHT,
		PAN_LEFT,
	} panState;

	// Tilt System
	const float TSERVO_ANGLE_MIN = +2.565634;	// rad
	const float TSERVO_ANGLE_MAX = -2.434734;	// rad
	const float TILT_MIN = -0.349;	// rad
	const float TILT_MAX = +0.349;	// rad
	const float TILT_VEL = +1.700; 	// rad/s

	float tilt = 0;
	OpenLoopServo tiltServo(
		PIN_TILT,
		SERVO_SIGNAL_MIN,
		SERVO_SIGNAL_MAX,
		TSERVO_ANGLE_MIN,
		TSERVO_ANGLE_MAX,
		SERVO_VELOCITY_MAX);

	enum {
		TILT_IDLE,
		TILT_UP,
		TILT_DOWN,
	} tiltState;

	// Flame Sensor System
	const int FLAME_FOUND_THRESHOLD = 647;			// ADC
	const int FLAME_EXTINGUISHED_THRESHOLD = 900;	// ADC
	bool foundFlame = false;
	bool extinguishedFlame = false;
	float flamePan = 0;
	float flameTilt = 0;
	float flameDistance = 0;
	int minRead = 0;

	enum {
		FF_SCAN_FIELD = 1,
		FF_PREP_PAN = 2,
		FF_WAIT_FOR_STOP = 3,
		FF_FIND_PAN = 4,
		FF_PREP_TILT = 5,
		FF_FIND_TILT = 6,
		FF_AIM_AT_FLAME = 7,
		FF_EXTINGUISH_FLAME = 8,
		FF_IDLE = 9,
	} ffState;

	// Flame Extinguisher Fan
	const int FAN_SIGNAL_MIN = 1000;
	const int FAN_SIGNAL_MAX = 2000;
	const float FAN_SPEED = 0.1;
	BrushlessMotor fan(
		PIN_FAN,
		FAN_SIGNAL_MIN,
		FAN_SIGNAL_MAX);

	// Private Function Templates
	bool aimedAtTarget();
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes all motors and sensors for flame finder.
void FlameFinder::setup() {

	// Initialize servos and fan
	panServo.setup(PAN_MIN);
	tiltServo.setup(TILT_MIN);
	fan.setup();
	fan.arm();
	delay(5000);

	// Set up pan state machine
	panServo.setVelocity(PAN_VEL);
	panServo.setAngle(PAN_MAX);
	panState = PAN_RIGHT;

	// Set up tilt state machine
	tiltServo.setVelocity(TILT_VEL);
	tiltServo.setAngle(TILT_MAX);
	tiltState = TILT_UP;

	// Set up flame finder
	pinMode(PIN_FLAME, INPUT);
	ffState = FF_SCAN_FIELD;
}

//!b Iterates through flame finder state machine.
void FlameFinder::loop() {

	// Pan state machine
	pan = panServo.loop();
	switch(panState) {
		case PAN_IDLE:	// Idle or controlled elsewhere
			break;

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
	tilt = tiltServo.loop();
	switch(tiltState) {
		case TILT_IDLE:	// Idle or controlled elsewhere
			break;

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

	// Flame finder state machine
	switch(ffState) {

		// Scanning field for flame
		case FF_SCAN_FIELD:
			if(analogRead(PIN_FLAME) <=
				FLAME_FOUND_THRESHOLD)
			{
				foundFlame = true;
				panState = PAN_IDLE;
				tiltState = TILT_IDLE;
				panServo.setAngle(PAN_MIN);
				tiltServo.setAngle(0);
				ffState = FF_PREP_PAN;
			}
			break;

		// Set angles for pan sweep
		case FF_PREP_PAN:
			if(aimedAtTarget()) {
				panServo.setAngle(PAN_MAX);
				minRead = 1023;
				ffState = FF_WAIT_FOR_STOP;
			}
			break;

		// Wait for robot to stop moving
		case FF_WAIT_FOR_STOP:
			if(Odometer::velocity < 0.001) {
				ffState = FF_FIND_PAN;
			}
			break;

		// Sweep to determine pan angle of flame.
		case FF_FIND_PAN:
			if(panServo.atTargetAngle()) {
				panServo.setAngle(flamePan);
				tiltServo.setAngle(TILT_MIN);
				ffState = FF_PREP_TILT;
			} else {
				int r = analogRead(PIN_FLAME);
				if(r < minRead) {
					minRead = r;
					flamePan = pan;
				}
			}
			break;

		// Set angles for tilt sweep.
		case FF_PREP_TILT:
			if(aimedAtTarget()) {
				tiltServo.setAngle(TILT_MAX);
				minRead = 1023;
				ffState = FF_FIND_TILT;
			}
			break;

		// Sweep to determine tilt angle of flame
		case FF_FIND_TILT:
			if(tiltServo.atTargetAngle()) {
				tiltServo.setAngle(flameTilt);
				ffState = FF_AIM_AT_FLAME;
			} else {
				int r = analogRead(PIN_FLAME);
				if(r < minRead) {
					minRead = r;
					flameTilt = tilt;
				}
			}
			break;

		// Aim extinguisher at flame
		case FF_AIM_AT_FLAME:
			if(aimedAtTarget()) {
				flameDistance = getFlameDistance();
				fan.setSpeed(FAN_SPEED);
				ffState = FF_EXTINGUISH_FLAME;
			}
			break;

		// Extinguish flame
		case FF_EXTINGUISH_FLAME:
			if(analogRead(PIN_FLAME) >
				FLAME_EXTINGUISHED_THRESHOLD)
			{
				fan.setSpeed(0);
				extinguishedFlame = true;
				panServo.setAngle(PAN_MIN);
				tiltServo.setAngle(TILT_MIN);
				ffState = FF_IDLE;
			}
			break;

		// Reset pan-tilt angles and do nothing
		case FF_IDLE:
			break;
	}
}

//!b Returns byte representing state of flame finder (see above).
byte FlameFinder::getState() {
	return (byte)ffState;
}

//!b Calculates distance to flame assuming direct line of sight.
float FlameFinder::getFlameDistance() {
	return 0.000499035 * analogRead(PIN_FLAME) + 0.138621;
}

//!b Returns true if pan and tilt servos are at target angles.
bool FlameFinder::aimedAtTarget() {
	return panServo.atTargetAngle() &&
			tiltServo.atTargetAngle();
}