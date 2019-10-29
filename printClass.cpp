#include "mbed.h"
#include "rtos.h"
#include "main_config.h"
#include "printClass.h"


void printClass::printClass(){
}

void printClass::printNormalModeResults(char type, float avg, float minResult,  float maxResult){
	
	switch(type){
		case TEMPERATURE:
			pc.printf("\n\rTemperature_avg: %.2f C, Temperature_min: %.2f C, Temperature_max: %.2f C", avg, minResult, maxResult);
			break;
		case HUMIDITY:
			pc.printf("\n\rHumidity_avg: %.2f%%, Humidity_min: %.2f%%, Humidity_max: %.2f%%", avg, minResult, maxResult);
			break;
		case LIGHT:
			pc.printf("\n\rLight_avg: %.2f%%, Light_min: %.2f%%, Light_max: %.2f%%", avg, minResult, maxResult);
			break;
		case SOIL:
			pc.printf("\n\rSoil_avg: %.2f%%, Soil_min: %.2f%%, Soil_max: %.2f%%", avg, minResult, maxResult);
			break;
		
	}
}

void printClass::calculateAndPrint(char type, float allResults []){
	Struct S;
	float min, max, avg;
	
	S = calculateMinMaxAvg(allResults);
	min =  S.min;
	max =  S.max;
	avg =  S.avg;
	printNormalModeResults(type, avg, min, min);
	
}


void printClass::printDominantColour(void){
	if (countRed > countGreen && countRed > countBlue){
		pc.printf("\n\rDominant Colour: RED  %i times\n\r", countRed);
	}
	else if (countBlue > countGreen && countBlue > countRed){
		pc.printf("\n\rDominant Colour: BLUE  %i times\n\r", countBlue);
	}
	else if (countGreen > countRed && countGreen > countBlue){
		pc.printf("\n\rDominant Colour: GREEN  %i times\n\r", countGreen);
	}
	
}


void printClass::printMode(void){
		
		pc.printf("\n\rSOIL MOISTURE: %2.1f%%", valueSM); 
		pc.printf("\n\rLIGHT: %2.1f%%", lightIntensity);
		pc.printf("\n\rGPS: #Sats: %d, Lat(UTC): %5.2f, Long(UTC): %5.2f, Altitude: %5.2f m, GPS_time: %d:%d:%d.%u" , satellites, latitude, longitude,altitude, hours, minutes,seconds, milliseconds );
		pc.printf("\n\rCOLOUR SENSOR: Clear: %i, Red: %i, Green: %i, Blue: %i   -- ", clear_value, red_value, green_value, blue_value);
		if (mode == 't'){
			redLED.write(1);
			blueLED.write(1);
			greenLED.write(1);
		}
		if (maxColour == 'r'){
			pc.printf("Dominant Color: RED");
			if (mode == 't'){ redLED.write(0);}
		} else if(maxColour == 'g'){
			pc.printf("Dominant Color: GREEN");
			if (mode == 't'){ greenLED.write(0);}
		} else if( maxColour == 'b'){
			pc.printf("Dominant Color: BLUE");
			if (mode == 't'){ blueLED.write(0);}
		}else{
			redLED.write(1);
			blueLED.write(1);
			greenLED.write(1);
		}
		pc.printf("\n\rACCELEROMETRE: X_axis: %1.2f, Y_axis: %1.2f, Z_axis: %1.2f", x, y, z);
		pc.printf("\n\rTEMP/HUM: Temperature: %.2f C, Relative Humidity: %.2f%%\n\r", temp, hum);
	
}
