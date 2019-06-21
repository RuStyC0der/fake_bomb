#include "TM1637.h"
#define CLK 2
#define DIO 3

TM1637 tm1637(CLK, DIO);
int8_t NumTab[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //0~9,A,b,C,d,E,F
int8_t ListDisp[4];

void led_setup() {
        tm1637.init();
}

void led_enable(/* arguments */) {
  tm1637.set(7);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}

void led_print_time(long time){

		int minuts = time / (1000 * 60);
		int seconds = time % (1000 * 60);
		tm1637.display(0, minuts /10);
		tm1637.display(1, minuts % 10);
		tm1637.display(2, seconds/10);
		tm1637.display(3, seconds%10);
}
