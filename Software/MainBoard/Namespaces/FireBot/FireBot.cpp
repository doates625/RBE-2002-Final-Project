//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.cpp
//!a Dan Oates (RBE-2002 B17 Team 10)

#include "FireBot.h"
#include "RobotDims.h"
#include "IndicatorLed.h"
#include "Odometer.h"
#include "Sonar.h"
#include "WallFollower.h"
#include "DriveSystem.h"
#include "PanTilt.h"
#include "MatlabComms.h"
#include "BrushlessMotor.h"

//*************************************************************//
// NAMESPACE FIELD DEFINITIONS
//*************************************************************//

namespace FireBot {

	// Arduino Pin Settings
	const uint8_t PIN_FLAME_SENSOR = A0;
	const uint8_t PIN_FAN = 8;

	// Flame Finding
	const int FLAME_FOUND_THRESHOLD = 750;	// (ADC)
	int minFlameRead = 0;	// (ADC)
	float flamePan = 0;		// (rad)
	float flameHeading = 0;	// (rad)
	float flameTilt = 0;	// (rad)
	Vec flamePos(3);		// In (x, y, z) format (m)

	// Driving to Candle
	const float CANDLE_DRIVE_DISTANCE = 0.25;	// (m)
	const float CANDLE_DRIVE_SPEED = 0.15;		// (m/s)
	const float CANDLE_BASE_RADIUS = 0.06;		// (m)
	float candleDriveTime = 0;	// (s)
	Timer candleDriveTimer;

	// Flame Extinguishing
	const int FLAME_OUT_THRESHOLD = 850;	// (ADC)
	const float FLAME_OUT_TIME = 5.0;		// (s)
	BrushlessMotor fan(PIN_FAN, 1000, 2000);
	Timer flameTimer;

	// State Machine
	enum {
		STATE_SEARCH_FOR_FLAME = 1,
		STATE_ZERO_PAN_SERVO,
		STATE_GET_FLAME_HEADING,
		STATE_TURN_TO_FLAME_HEADING,
		STATE_DRIVE_TO_CANDLE,
		STATE_LOWER_TILT_SERVO,
		STATE_GET_FLAME_TILT,
		STATE_AIM_AT_FLAME,
		STATE_EXTINGUISH_FLAME,
		STATE_CHECK_FLAME,
		STATE_BACK_FROM_CANDLE,
		STATE_TURN_TO_WALL,
		STATE_GO_HOME,
		STATE_AT_HOME
	} state;
}

//*************************************************************//
// NAMESPACE FUNCTION DEFINITIONS
//*************************************************************//

//!b Initializes FireBot.
//!d Call this method in the main setup function.
void FireBot::setup() {
	IndicatorLed::setup();

	// Initialize Namespaces
	if(!Odometer::setup()) error(1);	// Indicate IMU failure
	if(!MatlabComms::setup()) error(2);	// Indicate Hc06 failure
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
			if(flameDetected() &&
				WallFollower::inPausableState())
			{
				WallFollower::stop();
				PanTilt::stopTilt();
				PanTilt::setPan(0);
				state = STATE_ZERO_PAN_SERVO;
			}
			break;

		// Zero the pan servo angle in prep for pan sweep.
		case STATE_ZERO_PAN_SERVO:
			if(PanTilt::isAimed()) {
				minFlameRead = 1023;
				PanTilt::setPan(PanTilt::PAN_MAX);
				state = STATE_GET_FLAME_HEADING;
			}
			break;

		// Sweep pan servo to determine flame heading
		case STATE_GET_FLAME_HEADING:
			if(!PanTilt::isAimed()) {
				int fr = analogRead(PIN_FLAME_SENSOR);
				if(fr < minFlameRead) {
					minFlameRead = fr;
					flamePan = PanTilt::pan;
				}
			} else {
				flameHeading = Odometer::heading + flamePan;
				PanTilt::setPan(0);
				state = STATE_TURN_TO_FLAME_HEADING;
			}
			break;

		// Zero pan servo angle and turn robot towards flame
		case STATE_TURN_TO_FLAME_HEADING:
			if(DriveSystem::drive(flameHeading) &&
				PanTilt::isAimed())
			{
				DriveSystem::stop();
				candleDriveTimer.tic();
				state = STATE_DRIVE_TO_CANDLE;
			}
			break;

		// Drive up close to candle
		case STATE_DRIVE_TO_CANDLE: {
			float candleDist = Sonar::pingFront();
			if(candleDist != 0 && candleDist <
				CANDLE_DRIVE_DISTANCE)
			{
				DriveSystem::stop();
				candleDriveTime = candleDriveTimer.toc();
				state = STATE_LOWER_TILT_SERVO;
			} else {
				DriveSystem::drive(flameHeading,
					CANDLE_DRIVE_SPEED);
			}
			break;
		}

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
				computeFlamePosition();
				PanTilt::setTilt(flameTilt);
				state = STATE_AIM_AT_FLAME;
			}
			break;

		// Aim fan at flame
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
				candleDriveTimer.tic();
				state = STATE_BACK_FROM_CANDLE;
			}
			break;

		// Drive back from candle to wall-follow position
		case STATE_BACK_FROM_CANDLE:
			if(candleDriveTimer.hasElapsed(candleDriveTime)) {
				DriveSystem::stop();
				state = STATE_TURN_TO_WALL;
			} else {
				DriveSystem::drive(flameHeading,
					-CANDLE_DRIVE_SPEED);
			}
			break;

		// Turn robot back towards wall-following heading
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
		default: break;
	}
	if(MatlabComms::disconnected) {	// Matlab sent DC message
		error(0);
	}
}

//!b Returns byte enumerating current robot state
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

//!b Computes flame position (x, y, z) relative to field origin.
//!d Stores result in 3-dimensional 'flamePos' vector.
void FireBot::computeFlamePosition() {
	float cy = CANDLE_DRIVE_DISTANCE + CANDLE_BASE_RADIUS;
	float d1 = cy + RobotDims::dBTy
				+ (RobotDims::dTS * sin(flameTilt));
	float d2 = RobotDims::dBTz
				+ (RobotDims::dTS * cos(flameTilt));
	float d3 = d1 * tan(flameTilt);
	flamePos(1) = Odometer::position(1) + cy * sin(flameHeading);
	flamePos(2) = Odometer::position(2) + cy * cos(flameHeading);
	flamePos(3) = d2 + d3;
}

//!b Stops robot driving and flashes LED n times in a loop.
//!d This is used to indicate Serial or I2C communication errors.
void FireBot::error(uint8_t n) {
	fan.setSpeed(0.0);
	DriveSystem::stop();
	IndicatorLed::flash(n);
}
