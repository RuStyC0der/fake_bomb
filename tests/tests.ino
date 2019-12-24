#include "/home/unknown/Documents/fake_bomb/BWF/remote_control.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/smoke.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/gerkon_auth.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/led_strip.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/led_display.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/lcd_display.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/mpu.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/end_keys.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/mp3_player.ino"
#include "/home/unknown/Documents/fake_bomb/BWF/keypad.ino"



#include "GyverTimer.h"

#define MPU 0
#define REMOTE 0
#define LED_strip 0
#define LED_DISPLAY 0
#define LCD_DISPLAY 0
#define MP3 1
#define END_KEYS 0
#define GERKON 0
#define KEYPAD 0
#define SMOKE 0


// test vars :)

GTimer_ms clock(40);
GTimer_ms mid_timer(5000);

byte counter = 127;
char keypad_presed_keys[3] = {'_', '_', '_'};




#if MPU
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
void LED_strip_setup(){
    led_strip_setup();
}
void LED_strip_test(){
    Serial.print("LED_strip_test, all colors and brightness is : ");
    Serial.println(counter);
    led_strip_color(0, 0, counter);
    led_strip_Brightness(255);
    
}
#else  
void LED_strip_setup(){}
void LED_strip_test(){}
#endif

#if LED_DISPLAY
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
void KEYPAD_setup(){}
void KEYPAD_test(){
    keypad_update_keys(keypad_presed_keys);
    Serial.print("keypad keys: ");
    Serial.print(keypad_presed_keys[0]);
    Serial.print(keypad_presed_keys[1]);
    Serial.print(keypad_presed_keys[2]);
    Serial.print(" check state: ");
    Serial.println(keypad_check());
}
#else  
void KEYPAD_setup(){}
void KEYPAD_test(){}
#endif

#if SMOKE
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
}

void loop(){
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
        counter++;
    }
    delay(1000);
}