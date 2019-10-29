#include "mbed.h"
#include "rtos.h"
#include "MBed_Adafruit_GPS.h"

Thread threadGPS(osPriorityNormal, 2048); 
void GPS_thread();


float latitude, longitude, altitude;
int hours, minutes, seconds, milliseconds, satellites;



void GPS_thread() {
	Serial * gps_Serial = new Serial(PA_9,PA_10); //Rx, Tx
	Adafruit_GPS myGPS(gps_Serial); //object of Adafruit's GPS class
	char c; //when read via Adafruit_GPS::read(), the class returns single character stored here
	Timer refresh_Timer; //sets up a timer for use in loop; how often do we print GPS info?
	const int refresh_Time = 2000; //refresh time in ms

	myGPS.begin(9600);  
	myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); 
	myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
	myGPS.sendCommand(PGCMD_ANTENNA);

	Thread::wait(1000);
	
	while (true) {
		
	 c = myGPS.read();   

	 //check if we recieved a new message from GPS, if so, attempt to parse it,
	 if ( myGPS.newNMEAreceived() ) {
			 if ( !myGPS.parse(myGPS.lastNMEA()) ) {
					 continue;   
			 }    
	 }
	 
	hours = myGPS.hour;
	 minutes = myGPS.minute;
	 seconds = myGPS.seconds;
	 milliseconds = myGPS.milliseconds;
	 latitude = myGPS.latitude;
	 longitude = myGPS.longitude;
	 altitude = myGPS.altitude;
	 satellites = myGPS.satellites;
	 
	}
}