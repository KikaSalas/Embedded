#include "mbed.h"
#include "rtos.h"
#include "MBed_Adafruit_GPS.h"


const int timeDuration = 3;
const float limitMinTemp = 20, limitMaxTemp = 27;
const float limitMinHum = 20, limitMaxHum = 60;
const float limitMinLight = 10, limitMaxLight = 75;
const float limitMinSoil = 25, limitMaxSoil = 80;
const float limitX = 0.2, limitY = 0.2;

const char TEMPERATURE = 'a', HUMIDITY = 'b';
const char LIGHT = 'c', SOIL = 'd';
const char X_AXIS = 'e', Y_AXIS = 'f', Z_AXIS ='g'; 


Serial pc (USBTX, USBRX);
//GPS
extern Thread threadGPS;
extern void GPS_thread();
extern float altitude, longitude, latitude;
extern int hours, minutes, seconds, milliseconds, satellites;


//I2C
extern Thread threadI2C;
extern void I2C_thread();
extern float x, y, z, temp, hum;
extern int clear_value, red_value, green_value, blue_value;
extern char maxColour;

//ANALOG SENSORS
extern Thread threadANALOG;
extern void ANALOG_thread();
extern float valueSM, lightIntensity;

InterruptIn button(PB_2);
DigitalOut Led1(LED1);
DigitalOut Led2(LED2);
DigitalOut redLED(PH_0);
DigitalOut greenLED(PH_1);
DigitalOut blueLED(PB_13);
char mode = 't';
int auxTime = 2;
int counter = 0;
int countRed = 0, countGreen = 0, countBlue = 0;

void buttonInt() {
	
	switch (mode){
		case 'n':
			Led1.write(1);
			Led2.write(0);
			auxTime = 2;
			counter = 0;
			countRed = 0;
			countGreen = 0;
			countBlue = 0;
			redLED.write(1);
			blueLED.write(1);
			greenLED.write(1);		
			mode = 't';
		
			break;
		
		case 't':
			Led2.write(1);
			Led1.write(0);
			auxTime = 10;
			redLED.write(1);
			blueLED.write(1);
			greenLED.write(1);
			mode = 'n';
			break;
		
		}
}

void printNormalModeResults(char type, float avg, float minResult,  float maxResult){
	
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
		case X_AXIS:
			pc.printf("\n\rx_min: %1.2f, x_max: %1.2f", minResult, maxResult);
			break;
		case Y_AXIS:
			pc.printf("\n\ry_min: %1.2f, y_max: %1.2f", minResult, maxResult);
			break;
		case Z_AXIS:
			pc.printf("\n\rz_min: %1.2f, z_max: %1.2f", minResult, maxResult);
			break;
		
	}
}

void printDominantColour(){
	if (countRed > countGreen && countRed > countBlue){
		pc.printf("\n\rDominant Colour: RED  %i times\n\r", countRed);
	}
	else if (countBlue > countGreen && countBlue > countRed){
		pc.printf("\n\rDominant Colour: BLUE  %i times\n\r", countBlue);
	}
	else if (countGreen > countRed && countGreen > countBlue){
		pc.printf("\n\rDominant Colour: GREEN  %i times\n\r", countGreen);
	}else{
		pc.printf("\n\r No dominant colour\n\r");
	}

	
}

void printMode(){
		
		pc.printf("\n\rSOIL MOISTURE: %2.2f%%", valueSM); 
		pc.printf("\n\rLIGHT: %2.1f%%", lightIntensity);
		pc.printf("\n\rGPS: #Sats: %i, Lat(UTC): %5.2f, Long(UTC): %5.2f, Altitude: %5.2f m ",satellites, latitude, longitude,altitude );
		if (mode == 't'){
			pc.printf("GPS_time: %i:%i:%i", hours, minutes,seconds);
		}else if (mode == 'n'){
			pc.printf("GPS_time: %i:%i:%i LOCAL TIME", hours + 1, minutes,seconds);
		}
		
	
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

void evaluateMode(){
	if (mode == 't'){
		wait(auxTime);
	}else if (mode == 'n'){
		for (int i = 0; i < auxTime; i++) { wait(1);}
	}	
}

struct avgMinMax { 
    float avg, min, max; 
}; 

struct avgMinMax Struct;


struct avgMinMax calculateMinMaxAvg(float allResults []){
	struct avgMinMax s;
	float suma = 0;
	float minResult = 100;
	float maxResult = 0;
	for(int i = 0; i < timeDuration; i++){
				suma += allResults[i];
				if( allResults[i] < minResult){
					minResult = allResults[i];
					s.min = minResult; 
				}
				if( allResults[i] > maxResult){
					maxResult = allResults[i];
					s.max = maxResult; 
				}
			}
			s.avg = suma / timeDuration;
	
	return s;
	
}

void calculateAndPrint(char type, float allResults []){
	struct avgMinMax S;
	float min, max, avg;
	
	S = calculateMinMaxAvg(allResults);
	min =  S.min;
	max =  S.max;
	avg =  S.avg;
	printNormalModeResults(type, avg, min, max);
	
}

void checkEmergency(){
	
	if (x > limitX || y > limitY){
		redLED.write(0);
		greenLED.write(0);
		blueLED.write(1);
	}else if(maxColour != 'g'){
		greenLED.write(0);
		blueLED.write(0);
		redLED.write(1);
	}else if(valueSM > limitMaxSoil || valueSM < limitMinSoil){
		redLED.write(0);
		blueLED.write(0);
		greenLED.write(1);
	}else if(lightIntensity > limitMaxLight || lightIntensity < limitMinLight){
		greenLED.write(0);
		redLED.write(1);
		blueLED.write(1);
	}else if (temp > limitMaxTemp || temp < limitMinTemp){
		redLED.write(0);
		blueLED.write(1);
		greenLED.write(1);
	}else if( hum > limitMaxHum || hum < limitMinHum){
		blueLED.write(0);
		redLED.write(1);
		greenLED.write(1);
	}else{
		redLED.write(1);
		blueLED.write(1);
		greenLED.write(1);
	}
	
}



// main() runs in its own thread in the OS
int main() {

	float allTemps [timeDuration] = {};
	float allHumidity [timeDuration] = {};
	float allLight [timeDuration] = {};
	float allSoil [timeDuration] = {};
	float allX[timeDuration] = {};	
	float allY[timeDuration] = {};
	float allZ[timeDuration] = {};
		
	button.rise(&buttonInt);
	
	threadGPS.start(GPS_thread);
	threadI2C.start(I2C_thread);
	threadANALOG.start(ANALOG_thread);

	pc.baud(115200); //sets virtual COM serial communication to high rate; this is to allow more time to be spent on GPS retrieval
	pc.printf("Connection established at 115200 baud...\n\r");

	wait(1);
	Led1.write(1);
	
	while(true){
		
		
		if (counter >= timeDuration && mode =='n' ){
			calculateAndPrint(TEMPERATURE, allTemps);
			calculateAndPrint(HUMIDITY, allHumidity);
			calculateAndPrint(LIGHT, allLight);
			calculateAndPrint(SOIL, allSoil);
			calculateAndPrint(X_AXIS, allX);
			calculateAndPrint(Y_AXIS, allY);
			calculateAndPrint(Z_AXIS, allZ);
			
			printDominantColour();
		
			counter = 0; 
			countRed = 0;
			countGreen = 0;
			countBlue = 0;
			
		}else if (counter < timeDuration && mode =='n') {
			allTemps[counter] = temp;
			allHumidity[counter] = hum;
			allLight[counter] = lightIntensity;
			allSoil[counter] = valueSM;
			allX[counter] = x;
			allY[counter] = y;
			allZ[counter] = z;
			counter++;
			if (maxColour == 'r'){
				countRed ++;
			} else if(maxColour == 'g'){
				countGreen ++;
			} else if( maxColour == 'b'){
				countBlue ++;
			}
			checkEmergency();	
		}	
		evaluateMode();
		printMode();
		
	}
}

