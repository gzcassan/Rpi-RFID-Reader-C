#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <time.h>
#include <errno.h>
#include "CardReader.c"
#include "main.h"
#include "pins.c"

#define UNUSED_PIN -1
#define PINS_COUNT 26
#define FRAME_SIZE 26
#define READERS_COUNT_MAX 8
#define BIT_TIMEOUT 4000000

//debug
int counter = 0;

int pins[PINS_COUNT] = {PIN_0, 
		PIN_1, 
		UNUSABLE_PIN, 
		UNUSABLE_PIN,  
		PIN_4, 
		UNUSABLE_PIN, 
		UNUSABLE_PIN, 
		PIN_7, 
		PIN_8, 
		PIN_9, 
		PIN_10, 
		PIN_11, 
		UNUSABLE_PIN,
		UNUSABLE_PIN,
		PIN_14, 
		PIN_15, 
		UNUSABLE_PIN,
		PIN_17, 
		PIN_18,
		UNUSABLE_PIN,
		UNUSABLE_PIN,
		PIN_21, 
		PIN_22, 
		PIN_23, 
		PIN_24, 
		PIN_25};

int values[PINS_COUNT] = {};
int READERS_COUNT = 0;

CardReader** readers;
CardReader* _readers;

#include "callbacks.c"

void initProgram(){
	wiringPiSetupGpio();
	readers = (CardReader**)malloc(sizeof(CardReader*)*PINS_COUNT);	
}

void initReaders(){
	createCardReader("Lecteur", PIN_23, PIN_24, &callback23, &callback24);
}

int main(void) {
	
	initProgram();
	initReaders();
	
	
	
	// Waste time but not CPU
	for (;;) {
		sleep(1);
	}
}

void createCardReader(char* pname, int pGPIO_0, int pGPIO_1, void(*callback0), void(*callback1)){
	CardReader temp;
	
	temp.name = pname;
	temp.GPIO_0 = pGPIO_0;
	temp.GPIO_1 = pGPIO_1;
	temp.tag = malloc(sizeof(char)*FRAME_SIZE);
	temp.bitCount = 0;

	readers[pGPIO_0] = &temp;
	readers[pGPIO_1] = &temp;
	values[pGPIO_0] = 0;
	values[pGPIO_1] = 1;
	
	
	// Set pin to input in case it's not
	pinMode(pGPIO_0, INPUT);
	pinMode(pGPIO_1, INPUT);
	
	pullUpDnControl(pGPIO_0, PUD_UP);
	pullUpDnControl(pGPIO_1, PUD_UP);

	// Bind to interrupt
	wiringPiISR(PIN_23, INT_EDGE_FALLING, callback0);
	wiringPiISR(PIN_24, INT_EDGE_FALLING, callback1);
	
	updateArrays(temp);

}

void updateArrays(CardReader reader){
	_readers = realloc(_readers, sizeof(CardReader)*(READERS_COUNT+1));
	_readers[READERS_COUNT] = reader;
	READERS_COUNT++;
}
