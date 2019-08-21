/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <ISMMOG007_1> <MHLLIN036_2>
 * Date
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

int HH,MM,SS;
int hoursList[8], minsList[7];

void initGPIO(void){
	
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. 
	
	cleanUp(); //resets pins to avoid writing to the same hardware multiple times

	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
	    pinMode(LEDS[i], OUTPUT);
	    
	}
	//Set Up the Seconds LED for PWM
	pinMode(SECS, PWM_OUTPUT);
	
	printf("LEDS done\n");
	
	//Set up the Buttons
	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);
	}
	

	printf("BTNS done\n");
	printf("Setup done\n");
}

void cleanUp (void){

	for(int i = 0; i<sizeof(LEDS); i++){
		digitalWrite(LEDS[i], 0);
	
	}
}

/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	
	initGPIO();

	//Set random time (3:04PM)
	wiringPiI2CWriteReg8(RTC, HOUR, 0x12+TIMEZONE);
	wiringPiI2CWriteReg8(RTC, MIN,0x4);
	wiringPiI2CWriteReg8(RTC, SEC, 0x80 ); // activates the RTC to starts counting
	
	

	if (wiringPiISR (BTNS[0], INT_EDGE_FALLING, &hourInc) < 0){ //increase hours
		fprintf(stderr, "Cannot increase hours" );
		return 1;
	}
	if (wiringPiISR (BTNS[1], INT_EDGE_FALLING, &minInc) < 0){ //increase minutes
		fprintf(stderr, "Cannot inrcrease minutes");
		return 1;
	}
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
		hours = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		mins = hexCompensation(wiringPiI2CReadReg8 (RTC, MIN));
		secs = hexCompensation(wiringPiI2CReadReg8 (RTC, SECS)-0x80);
		
		//Function calls to toggle LEDs
		lightHours(hours);
		lightMins(mins);
		secPWM(secs);
		
		// Print out the time we have stored on our RTC
		printf("The current time is: %dh:%dm",hours, mins);

		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}
/*
 * Function that converts the binary variable to an array of separate bits
 * 
 */
void int_to_bin_digit(unsigned int in, int count, int* out){
    /* assert: count <= sizeof(int)*CHAR_BIT */
    unsigned int mask = 1U << (count-1);
    int i;
    for (i = 0; i < count; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}

int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}

/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
	// Write your logic to light up the hour LEDs here	

    int_to_bin_digit(units, 8, hoursList);		//Pass hours to be converted to an array of 8 bits
      
      int loop;
      int i=0;
	while(i<4){							// only writes to the first 4 LEDs in the LED array
 		for(loop = 4; loop < 8; loop++){
	  		digitalWrite (LEDS[i], hoursList[loop]) ;	//Write logic to each LED pins 0,2,3,25
	  		i++;
		}
	}
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
	//Write your logic to light up the minute LEDs here
						
    int_to_bin_digit(units, 7, minsList);		//Pass number of minutes to be converted to an array of 8 bits
      
      int loop;
      int i=4;								// starts by writing to the 5th LED in the LED array
   for(loop = 1; loop < 8; loop++){
	  digitalWrite (LEDS[i], minsList[loop]) ;	//Write logic to each LED pins 7,22,21,27,4,6 
	  i++;
	}
}

void secPWM(int units){

	for (int b = units; b<1024; b++){
			pwmWrite(SECS, (b/60));
	}
	
}

void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 1 triggered, %x\n", hours);
		
		//Fetch RTC Time
		hours = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		printf("%d\n", hours);
		
		//Increase hours by 1, ensuring not to overflow
		if (hours >= 12) {
			hours = 0;
		} else {
			hours++;
		}
		printf("%d\n", hours);

		//Write hours back to the RTC
		wiringPiI2CWriteReg8(RTC, HOUR, decCompensation(hours));
	}
	lastInterruptTime = interruptTime;
}

void minInc(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 2 triggered, %x\n", mins);
		//Fetch RTC Time

		mins = hexCompensation(wiringPiI2CReadReg8 (RTC, MIN));
		printf("%d\n", mins);
		
		//Increase minutes by 1, ensuring not to overflow
		if (mins >= 60){
			mins = 0;
		} else{
			mins++;
		}

		//Write minutes back to the RTC
		wiringPiI2CWriteReg8(RTC, MIN, decCompensation(mins));
	}
	lastInterruptTime = interruptTime;
}

