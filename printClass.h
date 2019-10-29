#include "mbed.h"
#include "rtos.h"
#include "main_config.h"


class printClass{
	
	public:
		void printClass();
		void printNormalModeResults(char type, float avg, float minResult,  float maxResult);
		void calculateAndPrint(char type, float allResults []);
		void printDominantColour();
		void printMode();
		
	
	private:
	
};



