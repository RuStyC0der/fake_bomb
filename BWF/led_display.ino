////////////////////////////////////////////////////////////////////////////////
//LED display
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

void led_enable(/* arguments */) {
		tm1637_6D.set(BRIGHTEST);    //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
		tm1637_6D.display(ListDisp,ListDispPoint);
}

void led_print_time(long time){
		int minuts = time / 60000;
		int seconds = time % 60000 / 1000;
		int ms = (time % 60000) % 1000 / 10;
		ListDisp[5] = minuts /10;
		ListDisp[4] =  minuts % 10;
		ListDisp[3] =  seconds/10;
		ListDisp[2] =  seconds%10;
		ListDisp[1] = ms /10;
		ListDisp[0] = ms % 10;
		tm1637_6D.display(ListDisp, ListDispPoint);
}
