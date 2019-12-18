////////////////////////////////////////////////////////////////////////////////
//LED display
// #define LED_DISABLED 0// for toggle this you need to comment/uncomment this line

// #ifndef LED_DISABLED == 0
#include "TM1637_6D.h"

#define CLK 2 //pins definitions for TM1637 and can be changed to other ports
#define DIO 3

TM1637_6D tm1637_6D(CLK,DIO);
int8_t ListDispPoint[6] = {POINT_OFF,POINT_OFF,POINT_ON,POINT_OFF,POINT_ON,POINT_OFF};
int8_t ListDisp[6] = {0,0,0,0,0,0};

void led_setup() {
		tm1637_6D.init();
		tm1637_6D.set(BRIGHTEST);    //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

}

void led_enable() {
		tm1637_6D.set(BRIGHTEST);    //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
		tm1637_6D.display(ListDisp,ListDispPoint);
}

void led_print_time(long timeInms){
		int minuts = timeInms / 60000;
		int seconds = timeInms % 60000 / 1000;
		int ms = (timeInms % 60000) % 1000 / 10;
		ListDisp[5] = minuts /10;
		ListDisp[4] =  minuts % 10;
		ListDisp[3] =  seconds/10;
		ListDisp[2] =  seconds%10;
		ListDisp[1] = ms /10;
		ListDisp[0] = ms % 10;
		tm1637_6D.display(ListDisp, ListDispPoint);
}


// #else
// void led_setup() {Serial.println(F("led_setup called, but it disabled!"));}
// void led_enable() {Serial.println(F("led_enable called, but it disabled!"));}
// void led_print_time(long timeInms){Serial.println(F("led_print_time called, but it disabled!"));}
// #endif