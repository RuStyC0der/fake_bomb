#include "GyverTimer.h"

// config:
// 0: work time time
// 1: del time
// 2: fine wait time
// 3: add time
// 4: first key code
// 5: second key code
// 6: thrid key code
// 7: first mpu treshold
// 8: second mpu treshold



extern int config[];
// extern volatile int keys_array[];

GTimer_ms second(1000);
GTimer_ms fine_wait(1000);

byte startButton = 3;

unsigned long time;






void update_led_time(){
        int minuts = time / (1000 * 60);
        int seconds = time % (1000 * 60);
        led_print(minuts /10,minuts % 10,seconds/10, seconds%10);
}

void pre_init(){
        sd_setup();
        sd_load_config();
        rfid_setup();
        mpu_setup();
        time = config[0] * 60 * 1000;
        fine_wait.setInterval(config[2] * 1000);
}

void post_init(){
  led_enable();
  lcd_enable();
}

void stage_a() {


}

void stage_b() {

}

void stage_c() {

}

void finish_a(){
        return;
}

void finish_b(){
        return;
}

void refresh(){
        if (second.isReady()) {
                time -= 1000;
                update_led_time();
        }
        if (fine_wait.isReady()){
          //check
        }

}

void setup() {
        pre_init();
        while (!digitalRead(startButton)) {
                // wait for start
        }
        post_init();

        for (int i = 0; i < 3; i++) {
                stage_a();
                stage_b();
                stage_c();
        }
        finish_a();
}

void loop() {
}
