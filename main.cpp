#include "mbed.h"
#include "rtos.h"

Serial pc (USBTX, USBRX);
	
//GPS
extern Thread threadGPS;
extern void GPS_thread();
extern float altitude, longitude, latitude;
extern double hours, minutes, seconds, fix, satellites;

//I2C
extern Thread threadI2C;
extern void I2C_thread();
extern float x, y, z, temp, hum;
extern int clear_value, red_value, green_value, blue_value;
extern char max;

//ANALOG SENSORS
extern Thread threadANALOG;
extern void ANALOG_thread();
extern float valueSM, lightIntensity; 



// main() runs in its own thread in the OS
int main() {

	threadGPS.start(GPS_thread);
	threadI2C.start(I2C_thread);
	threadANALOG.start(ANALOG_thread);

	pc.baud(115200); //sets virtual COM serial communication to high rate; this is to allow more time to be spent on GPS retrieval
	pc.printf("Connection established at 115200 baud...\n\r");

	wait(1);
	
	while(true){
		
		pc.printf("\n\rSOIL MOISTURE: %f %", valueSM); 
		pc.printf("\n\rLIGHT: %f %", lightIntensity);
		pc.printf("\n\rGPS: #Sats: %d, Lat(UTC): %5.2f, Long(UTC): %5.2f, Altitude: %5.2f m, GPS_time: %d:%d:%d" , satellites, latitude, longitude, hours, minutes,seconds );
		pc.printf("\n\rCOLOUR SENSOR: Clear: %i, Red: %i, Green: %i, Blue: %i   -- ", clear_value, red_value, green_value, blue_value);
		if (max == 'r'){
			pc.printf("Dominant Color: RED");
		} else if(max == 'g'){
			pc.printf("Dominant Color: GREEN");
		} else{
			pc.printf("Dominant Color: BLUE");
		}
		pc.printf("\n\rACCELEROMETRE: X_axis: %1.2f, Y_axis: %1.2f, Z_axis: %1.2f", x, y, z);
		pc.printf("\n\rTEMP/HUM: Temperature: %.2f C, Relative Humidity: %.2f%\n\r", temp, hum);
		//pc.printf("mbed-os-rev: %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);	

		wait(1);
		
	}
}

