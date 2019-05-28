#include "TM1637.h"
#define CLK
#define DIO 3

TM1637 tm1637(CLK, DIO);
int8_t NumTab[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //0~9,A,b,C,d,E,F
int8_t ListDisp[4];

void led_init() {
        tm1637.init();
        tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}
void led_print(int num_0, int num_1, int num_2, int num_3){
        tm1637.display(0, num_0);
        tm1637.display(1, num_1);
        tm1637.display(2, num_2);
        tm1637.display(3, num_3);
}
