#include "mbed.h"
#include "rtos.h"


AnalogIn soilmois(A0);
AnalogIn lightSensor(A2);

float valueSM=0.0;
float lightIntensity;

Thread threadANALOG(osPriorityNormal, 512); // 1K stack size

void ANALOG_thread(); 

void ANALOG_thread() {
		while (true) {

				Thread::wait(200);
				valueSM=soilmois*100;
				lightIntensity = lightSensor * 100;
			
				  
    }
}
