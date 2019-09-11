////////////////////////////////////////////////////////////////////////////////
//mpu6050
#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	#include "Wire.h"
#endif

int mpu_first_treshold;
int mpu_second_treshold;

MPU6050 accel;

void mpu_setup(int first_treshold,int second_treshold) {
  mpu_first_treshold = first_treshold;
  mpu_second_treshold = second_treshold;
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
float varProcess = 0.05; // скорость реакции на изменение (подбирается вручную)
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


int mpu_check(/* arguments */) {
		int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
		unsigned int sum;

		accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);
		sum = abs(_mpu_filter(constrain((gx_raw + gy_raw + gz_raw), -48000, 48000)));
		// Serial.println(sum);

		if (sum > mpu_second_treshold) {
				return 2;
		}else if (sum > mpu_first_treshold) {
				return 1;
		}else{
				return 0;
		}
}