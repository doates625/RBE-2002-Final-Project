//**************************************************************/
// TITLE
//**************************************************************/

//!t SonarComms.cpp
//!a RBE-2002 B17 Team 10

#include "Sonar.h"
#include "RobotDims.h"
#include "HcSr04Array.h"
#include "PinChangeInt.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace Sonar {

	// Sonar distance variables (from VTC)
	float distF = 0;
	float distB = 0;
	float distL = 0;
	float distR = 0;

	// Sonar trigger and echo pins (in order: F,B,L,R)
	uint8_t* PIN_TRIGS = new uint8_t[4]{51,  53, 50,  52};
	uint8_t* PIN_ECHOS = new uint8_t[4]{A9, A11, A8, A10};

	// Sonar array object
	HcSr04Array sensors(4, PIN_TRIGS, PIN_ECHOS);
	bool sonarBegun = false;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes sonar sensors.
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
void Sonar::loop() {
	if(sonarBegun) {
		sensors.loop();
		distF = sensors.get(1);
		distB = sensors.get(2);
		distL = sensors.get(3);
		distR = sensors.get(4);
		if(distF != 0) distF += RobotDims::sonarRadiusF;
		if(distB != 0) distB += RobotDims::sonarRadiusB;
		if(distL != 0) distL += RobotDims::sonarRadiusL;
		if(distR != 0) distR += RobotDims::sonarRadiusR;
	} else {
		sensors.begin();
		sonarBegun = true;
	}
}

//!b Performs interrupt service routine for sonar.
void Sonar::isr() {
	sensors.isr();
}
