
#include "GyverTimer.h"

int buzzer_pin = 8;

#define MPU 0
#define REMOTE 0
#define LED_strip 0
#define LED_DISPLAY 0
#define LCD_DISPLAY 0
#define MP3 0
#define END_KEYS 1
#define GERKON 0
#define KEYPAD 0
#define SMOKE 0
#define BUZZER 0


// test vars :)

GTimer_ms clock(80);
GTimer_ms buzzer_timer(1000);

GTimer_ms mid_timer(8000);

byte counter = 127;
char keypad_presed_keys_tt[3] = {'_', '_', '_'};




#if MPU
#include "/home/unknown/Documents/fake_bomb/BWF/mpu.ino"
void mpu_test_setup() {
    mpu_setup(4000, 8000);
}
void mpu_test(){
    Serial.print("MPU state is: ");
    Serial.println(mpu_check());
}
#else
void mpu_test_setup() {}
void mpu_test(){}
#endif

#if REMOTE
#include "/home/unknown/Documents/fake_bomb/BWF/remote_control.ino"
void remote_test_setup(){
    remote_setup();
}
void remote_test(){
Serial.print("remote_check result: ");
Serial.print(remote_check());
Serial.print(" pin state: ");
Serial.print(digitalRead(chanel_pin_1));
Serial.print(digitalRead(chanel_pin_2));
Serial.println(digitalRead(chanel_pin_3));

}
#else
void remote_test_setup(){}
void remote_test(){}
#endif

#if LED_strip
#include "/home/unknown/Documents/fake_bomb/BWF/led_strip.ino"
void LED_strip_setup(){
    led_strip_setup();
}
void LED_strip_test(){
    Serial.print("LED_strip_test, all colors and brightness is : ");
    Serial.println(counter);
    led_strip_color(counter, counter, counter);
    led_strip_Brightness(counter);
    
}
#else  
void LED_strip_setup(){}
void LED_strip_test(){}
#endif

#if LED_DISPLAY
#include "/home/unknown/Documents/fake_bomb/BWF/led_display.ino"
void LED_DISPLAY_setup(){
    led_setup();
    led_enable();
}
void LED_DISPLAY_test(){
    Serial.print("LED_DISPLAY_test, displaying time in ms: ");
    Serial.println((counter*6123)+counter);
    led_print_time((counter*6123)+counter);
}
#else  
void LED_DISPLAY_setup(){}
void LED_DISPLAY_test(){}
#endif

#if LCD_DISPLAY
#include "/home/unknown/Documents/fake_bomb/BWF/lcd_display.ino"
void LCD_DISPLAY_setup(){
    lcd_setup();
    lcd_enable();
}
void LCD_DISPLAY_test(){

    char trash[4][21];
    for (byte i = 0; i < 4; i++)
    {
        char symbol = random(101, 132);
        for (byte j = 0; j < 21; j++)
        {
            trash[i][j] = symbol;
        }  
    }
    lcd_print(0,trash[0]);
    lcd_print(1,trash[1]);
    lcd_print(2,trash[2]);
    lcd_print(3,trash[3]);
    Serial.println("LCD updated");
}
#else  
void LCD_DISPLAY_setup(){}
void LCD_DISPLAY_test(){}
#endif

#if END_KEYS
#include "/home/unknown/Documents/fake_bomb/BWF/end_keys.ino"
void END_KEYS_setup(){
end_keys_setup();
}
void END_KEYS_test(){
end_keys_light_brightness(counter);
Serial.print("end_keys_presed_count: ");
Serial.print(end_keys_presed_count());
Serial.print(" brightness: ");
Serial.println(counter);
}
#else  
void END_KEYS_setup(){}
void END_KEYS_test(){}
#endif

#if GERKON
#include "/home/unknown/Documents/fake_bomb/BWF/gerkon_auth.ino"
void GERKON_setup(){
    gerkon_setup();
}
void GERKON_test(){
    Serial.print("gerkon state: ");
    Serial.println(gerkon_auth());
} 
#else  
void GERKON_setup(){}
void GERKON_test(){}
#endif

#if MP3
#include "/home/unknown/Documents/fake_bomb/BWF/mp3_player.ino"
void MP3_setup(){
    mp3_setup();
}

void MP3_test(){
    if (mid_timer.isReady()){
        mp3_play((int)random(1, 7));
        Serial.println("tr changed");
    }
}
#else  
void MP3_setup(){} 
void MP3_test(){}
#endif

#if KEYPAD
#include "/home/unknown/Documents/fake_bomb/BWF/keypad.ino"
void KEYPAD_setup(){}
void KEYPAD_test(){
    keypad_update_keys(keypad_presed_keys_tt);
    Serial.print("keypad keys: ");
    Serial.print(keypad_presed_keys_tt[0]);
    Serial.print(keypad_presed_keys_tt[1]);
    Serial.print(keypad_presed_keys_tt[2]);
    Serial.println();
    // Serial.print(" check state: ");
    // Serial.println(keypad_check());
    // Serial.println();
}
#else  
void KEYPAD_setup(){}
void KEYPAD_test(){}
#endif

#if SMOKE
#include "/home/unknown/Documents/fake_bomb/BWF/smoke.ino"
void SMOKE_setup(){
    smoke_setup();
}
void SMOKE_test(){
Serial.print("battary_voltage: ");
Serial.print(smoke_battary_voltage());
Serial.print(" | run result: ");
Serial.println(smoke_run());
}
#else  
void SMOKE_setup(){}
void SMOKE_test(){}
#endif

#if BUZZER
void BUZZER_setup(){
    pinMode(buzzer_pin, OUTPUT);
}

void BUZZER_test(){
        digitalWrite(buzzer_pin, HIGH);
        delay(50);
        digitalWrite(buzzer_pin, LOW);
}
#else  
void BUZZER_setup(){}
void BUZZER_test(){}
#endif


void setup(){
    Serial.begin(9600);
    mpu_test_setup();
    remote_test_setup();
    LED_strip_setup();
    LED_DISPLAY_setup();
    LCD_DISPLAY_setup();
    MP3_setup();
    END_KEYS_setup();
    GERKON_setup();
    KEYPAD_setup();
    SMOKE_setup();
    BUZZER_setup();

}

void loop(){

    if (buzzer_timer.isReady())
    {
        BUZZER_test();
    }

    mpu_test();
    remote_test();
    LED_strip_test();
    LED_DISPLAY_test();
    LCD_DISPLAY_test();
    MP3_test();
    END_KEYS_test();
    GERKON_test();
    KEYPAD_test();
    SMOKE_test();
    if (clock.isReady()){
        counter+=4;
    }
}