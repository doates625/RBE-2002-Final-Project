//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.cpp
//!a RBE-2002 B17 Team 10

#include "FireBot.h"
#include "IndicatorLed.h"
#include "Odometer.h"
#include "Sonar.h"
#include "DriveSystem.h"
#include "WallFollower.h"
#include "PanTilt.h"
#include "MatlabComms.h"

//*************************************************************//
// NAMESPACE FIELD DEFINITIONS
//*************************************************************//

namespace FireBot {

	// Arduino Pin Settings
	const uint8_t PIN_FLAME_SENSOR = A0;
	const uint8_t PIN_FAN = 8;

	// Flame Finding Variables
	const int FLAME_FOUND_THRESHOLD = 647;	// ADC
	const int FLAME_OUT_THRESHOLD = 900;	// ADC
	int minFlameRead = 0;
	float sweepHeading = 0;
	float flameHeading = 0;
	float flameTilt = 0;
	Vec flamePos(3);

	// Fan Variables
	const float FLAME_OUT_TIME = 5.0;
	BrushlessMotor fan(PIN_FAN, 1000, 2000);
	Timer flameTimer;

	// Front Sonar Variables
	const uint8_t NUM_FRONT_READINGS = 10;
	float frontDistances[NUM_FRONT_READINGS];
	int frontReadsTaken = 0;

	// Flame Localization Variables
	const float CANDLE_BASE_RADIUS = 0.07;	// (m)
	float candleSonarDistance = 0;			// (m)

	// State Machine
	enum {
		STATE_SEARCH_FOR_FLAME = 1,
		STATE_ZERO_PAN_SERVO = 2,
		STATE_GET_FLAME_HEADING = 3,
		STATE_TURN_TO_FLAME_HEADING = 4,
		STATE_GET_FLAME_HORIZONTAL_DISTANCE = 5,
		STATE_LOWER_TILT_SERVO = 6,
		STATE_GET_FLAME_TILT = 7,
		STATE_AIM_AT_FLAME = 8,
		STATE_EXTINGUISH_FLAME = 9,
		STATE_CHECK_FLAME = 10,
		STATE_TURN_TO_WALL = 11,
		STATE_GO_HOME = 12,
		STATE_AT_HOME = 13
	} state;

	// Other Parameters
	const float HOME_DISTANCE_THRESHOLD = 0.1; // (m)
}

//*************************************************************//
// NAMESPACE FUNCTION DEFINITIONS
//*************************************************************//

//!b Initializes FireBot.
//!d Call this method in the main setup function.
void FireBot::setup() {

	// Initialize Namespaces
	if(!Odometer::setup()) error(1);	// Indicate IMU failure
	if(!MatlabComms::setup()) error(2);	// Indicate Hc06 failure

	IndicatorLed::setup();
	DriveSystem::setup();
	Sonar::setup();
	WallFollower::setup();
	PanTilt::setup();

	// Initialize Flame Sensor and Fan
	pinMode(PIN_FLAME_SENSOR, INPUT);
	fan.setup();
	fan.arm();
	delay(5000);

	// Wait for Matlab begin message
	IndicatorLed::led.on();
	if(!MatlabComms::waitForBegin()) {
		error(3);	// Indicate Matlab sent wrong byte
	}

	// Initialize state machine
	state = STATE_SEARCH_FOR_FLAME;
}

//!b Executes repeatedly after Arduino reset.
//!d Call this method in the main loop function.
void FireBot::loop() {

	// Subsystem Updates
	Odometer::loop();	// Update robot position and heading
	PanTilt::loop();	// Update pan-tilt servos

	// State Machine
	switch(state) {

		// Wall-follow until flame detected
		case STATE_SEARCH_FOR_FLAME:
			WallFollower::loop();
			PanTilt::sweep();
			Sonar::loop();
			if(WallFollower::inForwardState() &&
				flameDetected())
			{
				PanTilt::stopTilt();
				WallFollower::stop();
				PanTilt::setPan(0);
				state = STATE_ZERO_PAN_SERVO;
			}
			break;

		// Sero the pan servo in prep for heading sweep
		case STATE_ZERO_PAN_SERVO:
			if(PanTilt::isAimed()) {
				sweepHeading = Odometer::heading + HALF_PI;
				flameHeading = Odometer::heading;
				minFlameRead = 1023;
				state = STATE_GET_FLAME_HEADING;
			}
			break;

		// Rotate robot 90 degrees to find flame heading
		case STATE_GET_FLAME_HEADING:
			if(!DriveSystem::drive(sweepHeading)) {
				int fr = analogRead(PIN_FLAME_SENSOR);
				if(fr < minFlameRead) {
					minFlameRead = fr;
					flameHeading = Odometer::heading;
				}
			} else {
				state = STATE_TURN_TO_FLAME_HEADING;
			}
			break;

		// Turn robot to calculated flame heading
		case STATE_TURN_TO_FLAME_HEADING:
			if(DriveSystem::drive(flameHeading)) {
				state = STATE_GET_FLAME_HORIZONTAL_DISTANCE;
			}
			break;

		// Compute horizontal distance to candle with sonar
		case STATE_GET_FLAME_HORIZONTAL_DISTANCE:
			if(frontReadsTaken != NUM_FRONT_READINGS) {
				float dist = Sonar::pingFront();
				if(dist != 0) {
					frontDistances[frontReadsTaken] = dist;
					frontReadsTaken++;
				}
			} else {
				for(int i=0; i<NUM_FRONT_READINGS; i++) {
					candleSonarDistance += frontDistances[i];
				}
				candleSonarDistance /= NUM_FRONT_READINGS;
				PanTilt::setTilt(PanTilt::TILT_MIN);
				state = STATE_LOWER_TILT_SERVO;
			}
			break;

		// Lower tilt servo in prep for tilt sweep
		case STATE_LOWER_TILT_SERVO:
			if(PanTilt::isAimed()) {
				PanTilt::setTilt(PanTilt::TILT_MAX);
				flameTilt = PanTilt::tilt;
				minFlameRead = 1023;
				state = STATE_GET_FLAME_TILT;
			}
			break;

		// Sweep tilt servo up to find flame tilt
		case STATE_GET_FLAME_TILT:
			if(!PanTilt::isAimed()) {
				int fr = analogRead(PIN_FLAME_SENSOR);
				if(fr < minFlameRead) {
					minFlameRead = fr;
					flameTilt = PanTilt::tilt;
				}
			} else {
				PanTilt::setTilt(flameTilt);
				state = STATE_AIM_AT_FLAME;
			}
			break;

		// Aim fan towards flame
		case STATE_AIM_AT_FLAME:
			if(PanTilt::isAimed()) {
				fan.setSpeed(1.0);
				flameTimer.tic();
				state = STATE_EXTINGUISH_FLAME;
			}
			break;

		// Extinguish the flame
		case STATE_EXTINGUISH_FLAME:
			flameTimer.tic();
			if(flameExtinguished()) {
				state = STATE_CHECK_FLAME;
			}
			break;

		// Run fan extra time to assur flame is out
		case STATE_CHECK_FLAME:
			if(!flameExtinguished()) {
				state = STATE_EXTINGUISH_FLAME;
			} else if(flameTimer.hasElapsed(FLAME_OUT_TIME)) {
				fan.setSpeed(0.0);
				state = STATE_TURN_TO_WALL;
			}
			break;

		// Turn robot back towards wall it was following
		case STATE_TURN_TO_WALL:
			if(DriveSystem::drive(
				WallFollower::targetHeading()))
			{
				WallFollower::start();
				state = STATE_GO_HOME;
			}
			break;

		// Wall-follow until near home position
		case STATE_GO_HOME:
			WallFollower::loop();
			Sonar::loop();
			if(Odometer::nearHome()) {
				WallFollower::stop();
				state = STATE_AT_HOME;
			}
			break;

		// Sit around. Good job, robot!
		case STATE_AT_HOME:
			break;
	}

	// Check Messages from Matlab
	switch(MatlabComms::loop()) {
		case 1: error(1); break;	// No messages timeout
		case 2: error(2); break;	// Invalid message type byte
		case 3: error(3); break;	// Teleop data timeout
		default: break;
	}
	if(MatlabComms::disconnected) {	// Matlab sent DC message
		DriveSystem::stop();
		IndicatorLed::led.off();
		while(1);
	}
}

//!b Returns byte enumerating current state
byte FireBot::getState() {
	return (byte)state;
}

//!b Returns true if flame is detected by flame sensor.
bool FireBot::flameDetected() {
	return analogRead(PIN_FLAME_SENSOR) < FLAME_FOUND_THRESHOLD;
}

//!b Returns true if flame is extinguished by fan.
//!d Assumes flame sensor is pointed directly at flame.
bool FireBot::flameExtinguished() {
	return analogRead(PIN_FLAME_SENSOR) > FLAME_OUT_THRESHOLD;
}

//!b Stops robot driving and flashes LED n times in a loop.
void FireBot::error(uint8_t n) {
	DriveSystem::stop();
	IndicatorLed::flash(n);
}
