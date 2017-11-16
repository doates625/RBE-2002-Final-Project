//**************************************************************/
// TITLE
//**************************************************************/

//!t SonarComms.h
//!b Namespace for final project sonar board communication.
//!a RBE-2002 B17 Team 10

//!d This namespace handles communication with the secondary
//!d sonar board (an Arduino Nano) which pings the sonar sensors
//!d and collects distance data. This task is offset to a
//!d secondary board because it requires waiting up to 25ms
//!d For each ping to return while completing no other tasks
//!d for most accurate results. Instead, the sonar board
//!d collects distance data from all 4 sensors and sends it to
//!d the main board over serial.

#pragma once
#include "BinarySerial.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace SonarComms {
	extern float distF;
	extern float distB;
	extern float distL;
	extern float distR;

	uint8_t setup();
	uint8_t loop();
}
