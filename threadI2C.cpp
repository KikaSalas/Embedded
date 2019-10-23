#include "mbed.h"
#include "rtos.h"
#include "MMA8451Q.h"

//ACCELEROMETRE
#define MMA8451_I2C_ADDRESS (0x1d<<1)
PinName const SDA = PB_9;
PinName const SCL = PB_8;
float x, y, z;

//TEMPERATURE HUMIDITY
I2C misensor(PB_9,PB_8); //sda, sdl
float hum, temp;

//RGB LED AND COLOUR SENSOR
I2C colour_sensor(PB_9, PB_8);
DigitalOut ledColour(PA_10); // TCS34725 led
DigitalOut ledR(PA_14); //RGB led - red light
DigitalOut ledG(PH_1);  //RGB led - green light 
DigitalOut ledB(PA_4);  //RGB led - blue light
int sensor_addr = 0x29 << 1; 
//Variable for ISR
bool readColour =  false;
DigitalOut green(LED1); //LED of B-L072Z-LRWAN1 board
Ticker t;

int clear_value, red_value, green_value, blue_value;
char max;


Thread threadI2C(osPriorityNormal, 1024); // 1K stack size
void I2C_thread();

//ISR code
void read_colour (void) {
	readColour =  true;
}

//Get max value (r,g,b) function
char getMax(int r, int g, int b) {
  char result;
  int max;
  if (r < g){
    max = g; 
    result = 'g';  
  } else {
    max= r;
    result = 'r';
  }
  if (max < b){
    result = 'b';
  }
  return result;
}



void I2C_thread(){
	
	
	//ACCELEROMETRE
	MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);
	
	//TEMPERATURE HUMIDITY
	misensor.frequency(100000);
	char tx_buff[2];
	char rx_buff[2];
	
	//COLOUR SENSOR
	t.attach(read_colour, 1.0); // Every second the ticker triggers an interruption
  green = 1; // LED of B-L072Z-LRWAN1 board on
	char id_regval[1] = {0x92}; //?1001 0010? (bin)
  char data[1] = {0}; //?0000 0000?
	
	//We obtain device ID from ID register (0x12)
	colour_sensor.write(sensor_addr,id_regval,1, true);
	colour_sensor.read(sensor_addr,data,1,false);
	
	//We check that the ID is the TCS34725 one. If it is, we switch off a LED on the board, wait for 2s, and switch on again
	if (data[0]==0x44) { //? 0100 0100? -> Value for the part number (0x44 for TCS34725)
        green = 0;
        wait (1);
        green = 1;
	} else {
        green = 0;
  }
	
	// Initialize color sensor
    
	// Timing register address 0x01 (0000 0001). We set 1st bit to 1 -> 1000 0001
    char timing_register[2] = {0x81,0x50}; //0x50 ~ 400ms
    colour_sensor.write(sensor_addr,timing_register,2,false); 
    
	// Control register address 0x0F (0000 1111). We set 1st bit to 1 -> 1000 1111
    char control_register[2] = {0x8F,0}; //{0x8F, 0x00}, {1000 1111, 0000 0000} -> 1x gain
    colour_sensor.write(sensor_addr,control_register,2,false);
    
	// Enable register address 0x00 (0000 0000). We set 1st bit to 1 -> 1000 0000
    char enable_register[2] = {0x80,0x03}; //{0x80, 0x03}, {1000 0000, 0000 0011} -> AEN = PON = 1
    colour_sensor.write(sensor_addr,enable_register,2,false);
    
    // Read data from color sensor (Clear/Red/Green/Blue)
        char clear_reg[1] = {0x94}; // {?1001 0100?} -> 0x14 and we set 1st bit to 1
        char clear_data[2] = {0,0};
        char red_reg[1] = {0x96}; // {?1001 0110?} -> 0x16 and we set 1st bit to 1
        char red_data[2] = {0,0};
        char green_reg[1] = {0x98}; // {?1001 1000?} -> 0x18 and we set 1st bit to 1
        char green_data[2] = {0,0};
        char blue_reg[1] = {0x9A}; // {?1001 1010?} -> 0x1A and we set 1st bit to 1
        char blue_data[2] = {0,0};
    
    // Turn on the led in the sensor
    ledColour = 1;
	
	while(1){
		
		wait(1);
		//ACCELEROMETRE
		x = abs(acc.getAccX());
		y = abs(acc.getAccY());
		z = abs(acc.getAccZ());
		
		//TEMPERATURE HUMIDITY
		//Measure Temperature
		tx_buff[0] = 0xF3;  // CMD: Measure Temperature, No Hold Master Mode
		misensor.write(0x80, (char *) tx_buff, 1);	// Device ADDR: 0x80 = SI7021 7-bits address shifted one bit left.
		Thread::wait(100);
		misensor.read(0x80, (char*) rx_buff, 2);		// Receive MSB = rx_buff[0], then LSB = rx_buff[1]
	
		temp = (((rx_buff[0] * 256 + rx_buff[1]) * 175.72) / 65536.0) - 46.85;	// Conversion based on Datasheet

		//Measure Humidity
		tx_buff[0] = 0xF5;  // CMD: Measure Relative Humidity, No Hold Master Mode
		misensor.write(0x80, (char *) tx_buff, 1);
		Thread::wait(100);
		misensor.read(0x80, (char*) rx_buff, 2);		// Receive MSB first, then LSB
	
		hum = (((rx_buff[0] * 256 + rx_buff[1]) * 125.0) / 65536.0) - 6;				// Conversion based on Datasheet

		//Thread::wait(200);
		
		//If ISR has been executed, we read clear & RGB values
      if (readColour) {
        readColour = 0; //readColour = false
		//Reads clear value
        colour_sensor.write(sensor_addr,clear_reg,1, true);
        colour_sensor.read(sensor_addr,clear_data,2, false);
        
		//We store in clear_value the concatenation of clear_data[1] and clear_data[0]
        clear_value = ((int)clear_data[1] << 8) | clear_data[0];
        
		//Reads red value
        colour_sensor.write(sensor_addr,red_reg,1, true);
        colour_sensor.read(sensor_addr,red_data,2, false);
        
		//We store in red_value the concatenation of red_data[1] and red_data[0]
        red_value = ((int)red_data[1] << 8) | red_data[0];
        
		//Reads green value
        colour_sensor.write(sensor_addr,green_reg,1, true);
        colour_sensor.read(sensor_addr,green_data,2, false);
        
		//We store in green_value the concatenation of green_data[1] and green_data[0]
        green_value = ((int)green_data[1] << 8) | green_data[0];
        
		//Reads blue value
        colour_sensor.write(sensor_addr,blue_reg,1, true);
        colour_sensor.read(sensor_addr,blue_data,2, false);
        
		//We store in blue_value the concatenation of blue_data[1] and blue_data[0]
        blue_value = ((int)blue_data[1] << 8) | blue_data[0];
        
		//Obtains which one is the greatest - red, green or blue
		max = getMax(red_value, green_value, blue_value);
		
		//Switchs the color of the greatest value. First, we switch off all of them
        ledR.write(1);
        ledG.write(1);
        ledB.write(1);
        if (max == 'r'){
          ledR.write(0);
        } else if(max == 'g'){
          ledG.write(0);
        } else{
          ledB.write(0);
        }
				
    }
		
	}
	
}