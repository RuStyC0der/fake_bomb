#include <GyverTimer.h>

// float my_vcc_const = 0.976;
float my_vcc_const = 1.08;
float min_bat_treshold = 2.9;
float min_bat_treshold_or = 2.9;
float drawdown_factor = 0.88;
float temp_treshold = 60;

int mosfet_pin = 53;
int bat_pin = A3;

bool status = false;

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
		uint8_t low  = ADCL;// must read ADCL first - it then locks ADCH
		uint8_t high = ADCH; // unlocks both
		long result = (high << 8) | low;

		result = my_vcc_const * 1023 * 1000 / result; // расчёт реального VCC
		// return result; // возвращает VCC
		float vs = (float)result / 1000;
		return vs;
}

float smoke_battary_voltage(){
		int value = analogRead(bat_pin);
		float volt = (value / 1023.0) * readVcc();
		return volt;
}


GTimer_ms wait_time(12000);

void smoke_setup(){
	pinMode(mosfet_pin, OUTPUT);
	digitalWrite(mosfet_pin, LOW);
}


bool smoke_run(){
		if ((smoke_battary_voltage() > min_bat_treshold) and (wait_time.isReady())){
			digitalWrite(mosfet_pin, HIGH);
			delay(10);
			digitalWrite(mosfet_pin, LOW);
			return true;
        }else{
			return false;
		}
}
