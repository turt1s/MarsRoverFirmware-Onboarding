#include "CANConfig.h"
#include "CANInterface.h"
#include "mbed.h"
#include "TutorialServo.h"
#include "hw_bridge.h"
#include <cstring>



TutorialServo servo(PA_1);
CANInterface can(CANConfig::config);
Timer timer;

int main(){
	timer.start();  // start timer
	printf("startup \r\n");


	can.setFilter(HWBRIDGE::CANFILTER::COMMON_FILTER, CANStandard, HWBRIDGE::ROVER_CANID_FILTER_MASK);
	HWBRIDGE::CANSignalValue_t rxSignal1Value = 0;
	

	while(1){
		can.getRXSignalValue(HWBRIDGE::CANID::COMMON_DEBUG_MESSAGE1, HWBRIDGE::CANSIGNAL::COMMON_DEBUG_SIGNAL1, rxSignal1Value);
		//should receive 0.0-100.0
		
		uint32_t signalInBits = static_cast<uint32_t>(rxSignal1Value);

		float positionAsPercent;
		float positionAsDegrees;

		memcpy(&positionAsPercent, &signalInBits, sizeof(positionAsPercent));

		if(positionAsPercent>100.0f){ //cap at 100.0%
			positionAsPercent = 100.0f;
		} else if (!(positionAsPercent>=0.0f)){ //floor at 0.0%
			positionAsPercent = 0.0f;
		}

		positionAsDegrees = positionAsPercent*(servo.getServoRangeInDegrees()/100);

		servo.setPositionInDegrees(positionAsDegrees);
		
		if (timer.elapsed_time() >= 5s){ //every 5 seconds print the signal recieved, servo position as percent and in degrees 
			printf("signalInBits: 0x%08lX   positionAsPercent: %.2f   positionInDegrees: %.1f\r\n", signalInBits, positionAsPercent, positionAsDegrees);
			timer.reset();
		}
	}


}