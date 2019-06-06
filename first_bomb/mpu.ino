#include "I2Cdev.h"
#include "MPU6050.h"

// #define mila 123


int first_treshold = 1; //change me
int second_treshold = 2; //change me

MPU6050 accel;
void mpu_setup(/* arguments */) {
        accel.initialize(); // первичная настройка датчика
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
        Xe = G * (val - Zp) + Xp; // "фильтрованное" значение
        return (Xe);
}


int mpu_check(/* arguments */) {
        int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
        float sum;

        accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);
        sum = _mpu_filter(constrain((gx_raw + gy_raw + gz_raw), -16000, 16000));

        if (sum > second_treshold) {
                return 2;
        }else if (sum > first_treshold) {
                return 1;
        }else{
                return 0;
        }
}
