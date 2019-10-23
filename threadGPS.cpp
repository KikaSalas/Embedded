#include "mbed.h"
#include "rtos.h"
#include "MBed_Adafruit_GPS.h"



Serial * gps_Serial;

Thread threadGPS(osPriorityNormal, 1024); 
void GPS_thread();


float latitude, longitude, altitude;
double hours, minutes, seconds, fix, satellites;



void GPS_thread() {
	gps_Serial = new Serial(D8,PA_10); //Rx, Tx
	Adafruit_GPS myGPS(gps_Serial); //object of Adafruit's GPS class
	char c; //when read via Adafruit_GPS::read(), the class returns single character stored here
	Timer refresh_Timer; //sets up a timer for use in loop; how often do we print GPS info?
	const int refresh_Time = 2000; //refresh time in ms

	myGPS.begin(9600);  
	myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); 
	myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
	myGPS.sendCommand(PGCMD_ANTENNA);


	wait(1);
	refresh_Timer.start();  //starts the clock on the timer
	
	Serial pc (USBTX, USBRX);
	pc.baud(115200);
	
		while (true) {
			
			//GPS
	 c = myGPS.read();   
	 
	 //if (c) { pc.printf("%c", c); } //this line will echo the GPS data if not paused
	 
	 //check if we recieved a new message from GPS, if so, attempt to parse it,
	 if ( myGPS.newNMEAreceived() ) {
			 if ( !myGPS.parse(myGPS.lastNMEA()) ) {
					 continue;   
			 }    
	 }
		 
	 
	 if (refresh_Timer.read_ms() >= refresh_Time) {
		 refresh_Timer.reset();
		 pc.printf("\n\rFix: %d", (int) myGPS.fix);
		 
		 fix = myGPS.fix;
		 
		if (myGPS.fix) {
		 //pc.printf("GPS: #Sats: %d, Lat(UTC): %5.2f, Long(UTC): %5.2f, Altitude: %5.2f m, GPS_time: %d:%d:%d.%u\n\r" , myGPS.satellites, myGPS.latitude, myGPS.longitude, myGPS.hour, myGPS.minute, myGPS.seconds, myGPS.milliseconds );
		 
			latitude = myGPS.latitude;
			longitude = myGPS.longitude;
			altitude = myGPS.altitude;
			hours = myGPS.hour;
			minutes = myGPS.minute;
			seconds = myGPS.seconds;
			satellites = myGPS.satellites;
		
		}
	}
	

				Thread::wait(200);
				
				  
    }
}