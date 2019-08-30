#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);


DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1 = {0x28, 0x61, 0x64, 0x11, 0x83, 0xC3, 0xF7, 0xC3};  // адрес датчика DS18B20 280054B604000092

// float my_vcc_const = 0.976;
float my_vcc_const = 1.08;
float min_bat_treshold = 2.9;
float min_bat_treshold_or = 2.9;
float drawdown_factor = 0.88;
float temp_treshold = 60;



int request_pin = 4;
int mosfet_pin = 6;
int bat_pin = A5;
bool status = false;
int relay_pin = 9;

#include <GyverTimer.h>

GTimer_ms work_time(4000);
GTimer_ms wait_time(5000);

void setup(){
		sensors.begin();
		sensors.setResolution(Thermometer1, 10);
		pinMode(mosfet_pin, OUTPUT);
		pinMode(relay_pin, OUTPUT);
		digitalWrite(relay_pin, LOW);
		pinMode(request_pin, INPUT);
		Serial.begin(9600);
}

void loop() {
		sensors.requestTemperatures();
		// Serial.print(digitalRead(request_pin));
		// Serial.print((volts()));
		// Serial.println((sensors.getTempC(Thermometer1) < temp_treshold));
		delay(500);
		if (digitalRead(request_pin) and (volts() > min_bat_treshold) and (sensors.getTempC(Thermometer1) < temp_treshold))
		{
			Serial.println("on");
			digitalWrite(mosfet_pin, HIGH);
			work_time.reset();
			while (!work_time.isReady()) {delay(100);}
			digitalWrite(mosfet_pin, LOW);
			Serial.println("off");
			wait_time.reset();
			while (!wait_time.isReady()) {delay(100);}
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
