////////////////////////////////////////////////////////////////////////////////
//mpu6050
#include "I2Cdev.h"
#include "MPU6050.h"
#include "GyverTimer.h"

GTimer_ms touch_ignore_time(15000);

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	#include "Wire.h"
#endif

int mpu_treshold = 2000;
int reset_treshold = 1200; // 1200 tick by 500 ms = 10 minutes
int reset_counter = 0;

MPU6050 accel;

byte output_pin = 4;

void setup() {

	touch_ignore_time.setMode(0);

	Serial.begin(115200);

	pinMode(output_pin, OUTPUT);
  	
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
		Wire.begin();
	#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
		Fastwire::setup(400, true);
	#endif
		accel.initialize();     // первичная настройка датчика
		Serial.println(accel.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

// переменные для Калмана
float varVolt = 78.9;   // среднее отклонение (ищем в excel)
float varProcess = 1.0; // скорость реакции на изменение (подбирается вручную)
float Pc = 0.0, G = 0.0, P = 1.0, Xp = 0.0, Zp = 0.0, Xe = 0.0;

// Функция фильтрации
float _mpu_filter(float val) {
		Pc = P + varProcess;
		G = Pc / (Pc + varVolt);
		P = (1 - G) * Pc;
		Xp = Xe;
		Zp = Xp;
		Xe = G * (val - Zp) + Xp;     // "фильтрованное" значение
		return (Xe);
}

unsigned long previous_milis = millis();

long interval = 1000;

void(* resetFunc) (void) = 0; //declare reset function @ address 0


void loop(/* arguments */) {
		int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
		unsigned int sum;



		accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);
		sum = _mpu_filter(abs(constrain((gx_raw + gy_raw + gz_raw + ax_raw), -48000, 48000)));

		Serial.println(sum);

		
		if (sum > mpu_treshold && touch_ignore_time.isReady()) {
				digitalWrite(output_pin, HIGH);
				touch_ignore_time.reset();
		}else{
				digitalWrite(output_pin, LOW);
		}
		delay(500);


		reset_counter++;
		if (reset_counter >= reset_treshold){
			resetFunc();
		}
}
