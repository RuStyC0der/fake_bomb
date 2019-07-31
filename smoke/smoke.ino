#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 6

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1 = { 0x28, 0x61, 0x64, 0x11, 0x8C, 0x65, 0xB4, 0x7C };  // адрес датчика DS18B20 280054B604000092

// float my_vcc_const = 0.976;
float my_vcc_const = 1.08;
float min_bat_treshold = 3;
float min_bat_treshold_or = 3;
float drawdown_factor = 0.88;
float temp_treshold = 60;



int request_pin = 4;
int mosfet_pin = 6;
int bat_pin = A5;
bool status = false;

void setup(){
	sensors.begin();
	sensors.setResolution(Thermometer1, 10);
		pinMode(mosfet_pin, OUTPUT);
		pinMode(2, OUTPUT);
		digitalWrite(2, LOW);
		pinMode(request_pin, INPUT);
		Serial.begin(9600);
}

void loop() {
	sensors.requestTemperatures();
		if (digitalRead(request_pin) and (volts() > min_bat_treshold) and (sensors.getTempC(Thermometer1) < temp_treshold)) {
		   if (!status) {
		       Serial.println("on");
		       digitalWrite(mosfet_pin, HIGH);
		       min_bat_treshold = min_bat_treshold * drawdown_factor;
		       status = true;
		   }
		}else{
		   if (status) {
		       Serial.println("off");
		       digitalWrite(mosfet_pin, LOW);
		       min_bat_treshold = min_bat_treshold_or;
		       status = false;
		   }

		}
		// Serial.println(digitalRead(request_pin));
		// Serial.println(readVcc());
		// Serial.println(analogRead(bat_pin));
		// Serial.println(volts());
		// delay(500);
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
		uint8_t low  = ADCL;// must read ADCL first - it then locks ADCH
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
