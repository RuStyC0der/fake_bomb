#include "GyverTimer.h"

byte buzzer_pin = 8; //change me

byte start_button_pin = 12;
byte end_keys_pins[4] = {14,15,16,17}; // CHANGE ME BITCH!!!!!!!
byte disactivation_key_pin = 45;
// byte disactivation_key_pin = 13;

byte jumper_pins[3] = {A8,A9,A10};

byte artefact_list_pins[3] = {30,32,34};
// int artefact_led_pins[3] = {36,38,40}; // UNUSED

char keypad_presed_keys[3] = "___";

extern int config[];

// config array structure
// 0: work time
// 1: del time
// 2: fine wait time
// 3: add time
// 4: first key code
// 5: second key code
// 6: thrid key code
// 7: first stage code
// 8: second stage code
// 9: thrid stage code
// 10: first mpu treshold
// 11: second mpu treshold

////////////////////////////////////////////////////////////////////////////////
//sound folder structure
// 1: Bomb activated
// 2: Bomb deactivated
// 3: The accelerometer fixed the variable position of the bomb!
// 4: The bomb is accelerated for 2 minutes
// 5: Access is allowed for 1 minute
// 6: Insert the artifact
// 7: Insert jumper
// 8: Enter the code
// 9: Insert deactivation key


// PROTOTYPES
void led_strip_setup();
void led_strip_color(int, int, int);
void sd_setup();
bool sd_load_config(int*, int);
void led_strip_Brightness(int);
void rfid_setup();
int rfid_authentificate();
int remote_check();
void mp3_setup();
void mp3_play(int);
void mpu_setup(int, int);
int mpu_check();
void led_setup();
void led_print_time(long);
void led_enable();
void lcd_setup();
void lcd_enable();
void lcd_clear();
void lcd_print(int line_num, char str[]);

////////////////////////////////////////////////////////////////////////////////
// logic

int clock_step = 40;

GTimer_ms step_time(clock_step); // try change this to 20 or 50
GTimer_ms second(1000);
GTimer_ms ten_second(10000);


GTimer_ms five_second(5000);

GTimer_ms ignore_time(10000);


unsigned int access_time_config;
unsigned int del_time;
unsigned int add_time;
unsigned int time_move_step = 300000;


long time;
unsigned int access_time = 0;

bool update_flag = true;


void mpu_alarm(){
		lcd_clear();
		update_flag = true;
		ignore_time.reset();
		five_second.reset();
		led_strip_color(200, 0, 0);
		mp3_play(3);
}

void alarm(){
		lcd_clear();
		update_flag = true;
		ignore_time.reset();
		five_second.reset();
		led_strip_color(255, 0, 0);
		mp3_play(4);
		time -= del_time;
}

int keys_check(){
		int pressed_keys_count = 0;
		for(int i = 0; i<sizeof(end_keys_pins); i++) {
				if (digitalRead(end_keys_pins[i])) {
						pressed_keys_count += 1;
				}
		}
		return pressed_keys_count;
}

void time_added(){
		time += time_move_step;
		// bomb reaction
}

void access_granted(){
		access_time = access_time_config;
		mp3_play(5);

		//bomb reaction
}


void update(){
		if (step_time.isReady()) {
				digitalWrite(buzzer_pin, LOW);
				led_print_time(time);
				time -= clock_step;
				access_time -= clock_step;
				if (second.isReady()) {
						// Serial.println(access_time);
						// Serial.println(add_time);
						// Serial.println(del);

						digitalWrite(buzzer_pin, HIGH);
						switch (remote_check()) {
						case 0:
								break;
						case 1:
								time += time_move_step;
								break;
						case 2:
								time -= time_move_step;
								break;
						case 3:
								// move for 3 chanel triger
								break;
						case 4:
								// move for 4 chanel triger
								break;
						}
				}

				switch (rfid_authentificate()) {
				case 0:
						break;
				case 1:
						time_added();
						break;
				case 2:
						if (access_time < 0) {
								access_granted();
						}
						break;
				}

				if (access_time < 0) {

						if (ignore_time.isReady()) {
								switch (mpu_check()) {
								case 0:
										break;
								case 1:
										mpu_alarm();
										break;
								case 2:
										alarm();
										break;
								}

								// if (keypad_check() || keys_check()) {
								if (keypad_check()) {
										alarm();
								}
						}else{
								mpu_check();
						}
				}else{
						keypad_update_keys(keypad_presed_keys);
				}
		}

}



void pre_init(){
		pinMode(buzzer_pin, OUTPUT);
		Serial.begin(9600);
		mp3_setup();
		led_strip_setup();
		rfid_setup();
		led_setup();
		lcd_setup();
		sd_setup();
		sd_load_config();
		mpu_setup(config[10], config[11]);


		time = config[0];
		del_time = config[1];
		ignore_time.setInterval(config[2]);
		add_time = config[3];
		access_time_config = config[12];

		five_second.setMode(0);
		ignore_time.setMode(0);
}

void post_init(){
		mp3_play(1);
		led_enable();
		lcd_enable();
		five_second.reset();
		lcd_print(1, "  БOMБУ AKTИBOBAHO");
		lcd_print(2, "   BIДЛIK ПOЧATO");
}

void lcd_time_print(){
		// access_time = 60000;
		if (access_time >= 0) {
				char lcd_time[] = "       00:00";
				int minuts = access_time / 60000;
				int seconds = access_time % 60000 / 1000;
				lcd_time[7] = (char)((minuts / 10) + 48);
				lcd_time[8] = (char)((minuts % 10) + 48);
				// lcd_time[9] = ':';
				lcd_time[10] = (char)((seconds / 10) + 48);
				lcd_time[11] = (char)((seconds % 10) + 48);
				lcd_print(0, lcd_time);
		}
}

void stage_a(int iteration) { // keyboard
		while(time > 0) {
				update();
				if (five_second.isReady()) {
						if (update_flag) {
								lcd_clear();
								led_strip_color(0,255,0);
								char line[] = "Введiть код # ";
								line[sizeof(line) - 2] = (char)(iteration + 49);
								lcd_print(1, line);
								if (iteration > 0) {
										char ch[3];
										itoa(config[7 + iteration],ch,10);
										lcd_print(3, ch);
								}
								update_flag = false;
						}
						lcd_time_print();
						lcd_print(2, keypad_presed_keys);



						if (keypad_presed_keys[2] != '_') {
								int num = 0;
								for (int i = 0; i < 3; i++) {
										num = (num * 10) + (int)keypad_presed_keys[i] - 48;
								}
								if(num == config[iteration + 4]) {
										update_flag = true;
										return;
								}
						}
				}else{

				}
		}
		finish_b();
}



void stage_b(int iteration) { // artefacts
		while(time > 0) {
				update();
				if (five_second.isReady()) {
						if (update_flag) {
								lcd_clear();
								update_flag = false;
								led_strip_color(0,255,0);
								char line[] = "Вставте артефакт # ";
								line[sizeof(line) - 2] = (char)(iteration + 49);
								lcd_print(2, line);
								if (iteration > 0) {
										char ch[3];
										itoa(config[7 + iteration],ch,10);
										lcd_print(3, ch);
								}

						}
						lcd_time_print();

						if (digitalRead(artefact_list_pins[iteration])) {             /*stage finished*/
								update_flag = true;
								return;
						}
				}
		}
		finish_b();
}

void stage_c(int iteration) { // jumpers
		while(time > 0) {
				update();
				if (five_second.isReady()) {
						if (update_flag) {
								lcd_clear();
								update_flag = false;
								led_strip_color(0,255,0);
								char line[] = "Вставте перемичку # ";
								line[sizeof(line) - 2] = (char)(iteration + 49);
								lcd_print(2, line);

								if (iteration > 0) {
										char ch[3];
										itoa(config[7 + iteration],ch,10);
										lcd_print(3, ch);
								}
						}
						lcd_time_print();
						if (digitalRead(jumper_pins[iteration])) {             /*stage finished*/
								update_flag = true;
								return;
						}
				}
		}
		finish_b();
}



void final_block(){
		mp3_play(9);
		ten_second.reset();
		lcd_clear();
		lcd_print(2,"ВСТАВТЕ КЛЮЧ ДЕ3АКТИВАЦII");
		// lcd_print(3,"Та натисніть секретні кнопки");
		while ((keys_check() != sizeof(end_keys_pins) -1) || !digitalRead(disactivation_key_pin)) {
				update();

				if (ten_second.isReady()) {
						mp3_play(9);
				}

				if (time <= 0) {finish_b();}         // finish b if timeout
		}

}

void finish_a(){
		mp3_play(2);
		for (int i = 255; i >= 0; i--) {
				led_strip_Brightness(i);
				delay(20);

		}
		while(true) {delay(1000);}     // wait for reset
}

void finish_b(){
		while(true) {delay(1000);}      // wait for reset
}

void setup() {
		pre_init();
		while (!digitalRead(start_button_pin)) {}     // wait to push start button
		post_init();
		for (int i = 0; i < 3; i++) {
				stage_a(i);
				stage_b(i);
				stage_c(i);
		}
		final_block();
		finish_a();
}

void loop() {}
