#include <TM1637.h>

const int CLK = 8; //Set the CLK pin connection to the display
const int DIO = 9; //Set the DIO pin connection to the display

// float my_vcc_const = 0.976;
float my_vcc_const = 1.08;
float min_bat_treshold = 2.8;
float min_bat_treshold_or = 2.8;
float drawdown_factor = 1.125;



int request_pin = 4;
int mosfet_pin = 6;
int bat_pin = A5;
bool status = false;

int NumStep = 0;  //Variable to interate

TM1637 display(CLK, DIO);  //set up the 4-Digit Display.

void setup()
{
		display.set(BRIGHTEST); //set the diplay to maximum brightness
} 


void loop()
{
numDisp(digitalRead(bat_pin));
}
void numDisp(int num){
 for (int i = 3; i < 0; i--){
      display.display(i, num % 10);
      num = num/10;
 }
}


float readVcc() { //функция чтения внутреннего опорного напряжения, универсальная (для всех ардуин)
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
  long result = (high << 8) | low;

  result = my_vcc_const * 1023 * 1000 / result; // расчёт реального VCC
  // return result; // возвращает VCC
	float vs = (float)result / 1000;
	return vs;
}

float volts(){
	int value = analogRead(bat_pin);
	float volt = (value / 1023.0) * readVcc();
	return volt;
}
