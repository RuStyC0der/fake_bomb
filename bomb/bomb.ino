////////////////////////////////////////////////////////////////////////////////
// buzzer
int buzzer_pin = 8; //change me

////////////////////////////////////////////////////////////////////////////////
//button`s
byte start_button_pin = 41;
int end_keys_pins[4] = {A12, A13, A14, A15}; // CHANGE ME BITCH!!!!!!!
int disactivation_key_pin = 45;

////////////////////////////////////////////////////////////////////////////////
//jumper_pins
int jumper_pins[3] = {A8,A9,A10};

////////////////////////////////////////////////////////////////////////////////
//aertefacts pins
int artefact_list_pins[3] = {34,32,30};
// int artefact_led_pins[3] = {36,38,40}; // UNUSED

// smoke_pin
int smoke_pin = 13;

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// LED line
#include <Adafruit_NeoPixel.h>
#define PIN   26
#define NUMPIXELS 17

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void led_strip_setup() {
		pixels.begin();
		pixels.clear();
}

void led_strip_color(int r, int g, int b) {
		for (int i = 0; i < NUMPIXELS; i++) {
				pixels.setPixelColor(i, pixels.Color(r, g, b));
		}
		pixels.show();
}

void led_strip_brightness(int i){
	pixels.setBrightness(i);
	pixels.show();
}
////////////////////////////////////////////////////////////////////////////////
//SD card
#include <SD.h>

File file;

#define CONFIG_SIZE 13
#define SD_POWER_PIN A7
#define SD_SS_PIN 6
long config[CONFIG_SIZE] = {0};

void sd_setup()
{
		pinMode(SD_POWER_PIN, OUTPUT);
		sd_power(1);
		pinMode(SD_SS_PIN, OUTPUT);
		Serial.print("Initializing SD card...");
// на Ethernet шилде CS соответствует 4 пину. По умолчанию он установлен в режим output
// обратите внимание, что если он не используется в качестве CS пина, SS пин на оборудовании
// (10 на большинстве плат Arduino, 53 на Arduino Mega) надо оставить в режиме output.
// иначе функции библиотеки SD library не будут работать.
		if (!SD.begin(SD_SS_PIN)) {
				Serial.println("initialization failed!");
				return;
		}
		Serial.println("initialization done.");

}

char symbol;
bool sd_load_config(){      // thos func return 1 if config loaded succfully, and 0 if not

		file = SD.open("config.cfg");
		if (!file) {
				Serial.println("error opening config.cfg");
				pinMode(6, INPUT);
				sd_power(0);
				return false;
		}

		for (int i = 0; i < CONFIG_SIZE; i++) {
				// Serial.println(i);
				while (symbol != ' ') {
						symbol = file.read();
						// Serial.println(symbol);
						if (isdigit(symbol)) {
								config[i] = (config[i] * 10) + ((int)(symbol) - 48);
						}
				}
				while (1) {
						symbol = file.read();
						// Serial.println(symbol);
						if (symbol == '\n') {
								break;
						}
				}
		}
		sd_power(0);
		return 1;

}
void sd_power(bool flag){
		if (flag) {
				digitalWrite(SD_POWER_PIN, LOW);
		}else{
				digitalWrite(SD_POWER_PIN, HIGH);
		}
}
////////////////////////////////////////////////////////////////////////////////
//rfid
#include <MFRC522.h>
#include <SPI.h>
#define RFID_RST_PIN 5
byte rfid_keys_array[4][4] = {{78,189,226,33},{126,35,94,33},{38,136,22,18},{18,164,106,29}};
byte rfid_access_flag_key[4] = {48,110,185,164};
byte rfid_current_key[4];

MFRC522 rfid(53, RFID_RST_PIN);
MFRC522::MIFARE_Key key;

void rfid_setup() {
		SPI.begin();
		rfid.PCD_Init();
		for (byte i = 0; i < 6; i++) {
				key.keyByte[i] = 0xFF;
		}
}

int rfid_authentificate() {
		if ( !rfid.PICC_IsNewCardPresent())
				return 0;
		if ( !rfid.PICC_ReadCardSerial())
				return 0;

		Serial.print(F("PICC type: "));
		MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
		Serial.println(rfid.PICC_GetTypeName(piccType));
		// Check is the PICC of Classic MIFARE type
		if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
		    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
		    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
				Serial.println(F("Your tag is not of type MIFARE Classic."));
				return 0;
		}
		for (byte i = 0; i < 4; i++) {
				rfid_current_key[i] = rfid.uid.uidByte[i];
				// Serial.print(rfid_current_key[i]);
				// Serial.print(",");
		}
		// Serial.println();
		// delay(1000);

		bool flag = true;

		for (byte i = 0; i < 4; i++) {
				if (rfid_current_key[i] != rfid_access_flag_key[i]) {
						flag = false;
				}
		}
		if (flag == true) {
				return 2;
		}

		for (byte i = 0; i < 4; i++) {
				flag = true;
				// Serial.println(i);
				for (byte j = 0; j < 4; j++) {

						if (rfid_current_key[j] != rfid_keys_array[i][j]) {
								flag = false;

								break;
						}
				}
				if (flag == true) {
						memset(rfid_keys_array[i], 0, sizeof(rfid_keys_array[i]));
						return 1;
				}
		}
		// delay(1000);
		return 0;
		rfid.PICC_HaltA();
		rfid.PCD_StopCrypto1();
}

////////////////////////////////////////////////////////////////////////////////
//remote control
#define chanel_pin_1 22
#define chanel_pin_2 24
#define chanel_pin_3 26
#define chanel_pin_4 28

int remote_check(){
		if (digitalRead(chanel_pin_1)) {
				return 1;
		}else if (digitalRead(chanel_pin_2)) {
				return 2;
		}else if(digitalRead(chanel_pin_3)) {
				return 3;
		}else if (digitalRead(chanel_pin_4)) {
				return 4;
		}else{
				return 0;
		}
}

////////////////////////////////////////////////////////////////////////////////
// MP3_PLAYER
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 12); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void mp3_setup()
{
		mySoftwareSerial.begin(9600);
		if (!myDFPlayer.begin(mySoftwareSerial)) {
				Serial.println(F("MP3_PLAYER ERROR!"));
				while (true);
		}
		Serial.println(F("DFPlayer Mini online."));
		myDFPlayer.volume(20);     //Set volume value. From 0 to 30
		//myDFPlayer.play(4);
}

void mp3_play(int number) {
		myDFPlayer.playMp3Folder(number - 1);
}

////////////////////////////////////////////////////////////////////////////////
//mpu6050
#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	#include "Wire.h"
#endif
// #define mila 123
// extern int config[];

int mpu_first_treshold; //change me
int mpu_second_treshold; //change me

MPU6050 accel;

void mpu_setup(/* arguments */) {
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
float varProcess = 0.005; // скорость реакции на изменение (подбирается вручную)
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

////////////////////////////////////////////////////////////////////////////////
//LED display
#include "TM1637_6D.h"

#define CLK 2 //pins definitions for TM1637 and can be changed to other ports
#define DIO 3

TM1637_6D tm1637_6D(CLK,DIO);
int8_t ListDispPoint[6] = {POINT_OFF,POINT_OFF,POINT_ON,POINT_OFF,POINT_ON,POINT_OFF};
int8_t ListDisp[6] = {0,0,0,0,0,0};

void led_setup() {
		tm1637_6D.init();
		tm1637_6D.set(BRIGHTEST);    //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

}

void led_enable(/* arguments */) {
		tm1637_6D.set(BRIGHTEST);    //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
		tm1637_6D.display(ListDisp,ListDispPoint);
}

void led_print_time(long time){
		int minuts = time / 60000;
		int seconds = time % 60000 / 1000;
		int ms = (time % 60000) % 1000 / 10;
		ListDisp[5] = minuts /10;
		ListDisp[4] =  minuts % 10;
		ListDisp[3] =  seconds/10;
		ListDisp[2] =  seconds%10;
		ListDisp[1] = ms /10;
		ListDisp[0] = ms % 10;
		tm1637_6D.display(ListDisp, ListDispPoint);
}

////////////////////////////////////////////////////////////////////////////////
//LCD display
#include <LCD_1602_RUS.h>
#define LCD_CHARS 20
#define LCD_LINES 4

LCD_1602_RUS lcd(0x27, LCD_CHARS, LCD_LINES);

void lcd_setup()
{
		lcd.init();
		lcd.noBacklight();
}

void lcd_enable(/* arguments */) {
		lcd.backlight();
}

void lcd_clear(){
		lcd.clear();
}

void lcd_clear(int line){
		lcd.setCursor(0, line);
		lcd.print("                    ");
}

void lcd_print(int line_num, char str[])
{
		lcd.setCursor(0, line_num);
		lcd.print(str);
}

////////////////////////////////////////////////////////////////////////////////
//keypad
#include <Keypad.h>
const byte keypad_COLS = 3;
const byte keypad_ROWS = 4;

char keypad_hexaKeys[keypad_ROWS][keypad_COLS] = {
		{'1','2','3'},
		{'4','5','6'},
		{'7','8','9'},
		{'*','0','#'}
};

byte keypad_rowPins[keypad_ROWS] = {35, 33, 31, 37};
// byte keypad_rowPins[keypad_ROWS] = {37, 35, 33, 31};
// byte keypad_colPins[keypad_COLS] = {A2, A1, A0};
byte keypad_colPins[keypad_COLS] = {A0, A1, A2};
// byte keypad_colPins[keypad_COLS] = {39, 41, 43};
char keypad_presed_keys[3] = {'_', '_', '_'};
byte keypad_presed_keys_count = 0;

Keypad customKeypad = Keypad( makeKeymap(keypad_hexaKeys), keypad_rowPins, keypad_colPins, keypad_ROWS, keypad_COLS);

void keypad_get_key() {
		if (keypad_presed_keys_count < 3) {
				char key = customKeypad.getKey();
				if(key) {
						keypad_presed_keys[keypad_presed_keys_count] = key;
						keypad_presed_keys_count++;
				}

		}else{
				memset(keypad_presed_keys, 95,3);
				keypad_presed_keys_count = 0;
				char key = customKeypad.getKey();
				if (key) {
						keypad_presed_keys[keypad_presed_keys_count] = key;
						keypad_presed_keys_count++;
				}
		}

}

bool keypad_check(){
		if (customKeypad.getKey()) {
				return true;
		}else{
				return false;
		}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// logic
#include "GyverTimer.h"

int clock_step = 40;

GTimer_ms step_time(clock_step); // try change this to 20 or 50
GTimer_ms second(1000);

GTimer_ms ten_second(10000);

GTimer_ms jump_delay(5000);


GTimer_ms five_second(5000);

GTimer_ms ignore_time(10000);


long access_time_config;
long del_time;
long add_time;
long time_move_step = 300000;


long time;
long access_time = 0;

bool update_flag = true;


void mpu_alarm(){
		lcd_clear();
		update_flag = true;
		ignore_time.reset();
		five_second.reset();
		// led_strip_color(200, 0, 0);
		mp3_play(3);
}

void alarm(){
		lcd_clear();
		lcd_print(0,"SDFEERHNVYDRTYJDFHYT");
		lcd_print(1,"##### WARNING! #####");
		lcd_print(2,"## CRITICAL ERROR ##");
		lcd_print(3,"ATHTKBYFEEVGYDTYDSY6");
		update_flag = true;
		ignore_time.reset();
		five_second.reset();
		// led_strip_color(255, 0, 0);
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

int min_br = 10, max_br = 250, step_br = 4;
int current_br = min_br;

void update(){
		if (step_time.isReady()) {

				if (five_second.isReady()){
					led_strip_color(255,255,0);
				}else{
					led_strip_color(255,0,0);
				}

				if (current_br >= max_br && step_br == 4){
					step_br = -4;
				} else if (current_br <= min_br && step_br == -4){
					step_br = 4;
				}

				led_strip_brightness(current_br);
				current_br += step_br;

				digitalWrite(buzzer_pin, LOW);
				led_print_time(time);
				time -= clock_step;
				access_time -= clock_step;
				if (second.isReady()) {

						digitalWrite(buzzer_pin, HIGH);
						switch (remote_check()) {
						case 0:
								break;
						case 1:
								if (time < 5940000){
								time += time_move_step;
								}
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
								if (keypad_check()) {
										alarm();
								}
						}else{
								mpu_check();
						}
				}else{
						keypad_get_key();
				}
		}

}

void smoke_request(){
	digitalWrite(smoke_pin , HIGH);
	delay(1000);
	digitalWrite(smoke_pin , LOW);
}

void pre_init(){
		pinMode(buzzer_pin, OUTPUT);
		pinMode(smoke_pin, OUTPUT);
		// Serial.begin(9600);
		mp3_setup();
		led_strip_setup();
		rfid_setup();
		led_setup();
		lcd_setup();
		sd_setup();
		sd_load_config();
		mpu_setup();


		time = config[0];
		del_time = config[1];
		ignore_time.setInterval(config[2]);
		add_time = config[3];
		// mpu_first_treshold = config[10];
		// mpu_second_treshold = config[11];
		mpu_first_treshold = 500;
		mpu_second_treshold = 800;
		access_time_config = config[12];
		five_second.setMode(0);
		ignore_time.setMode(0);
		jump_delay.setMode(0);
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
		if (access_time >= 0) {
				char lcd_time[] = "       00:00";
				int minuts = access_time / 60000;
				int seconds = access_time % 60000 / 1000;
				lcd_time[7] = (char)((minuts / 10) + 48);
				lcd_time[8] = (char)((minuts % 10) + 48);
				lcd_time[10] = (char)((seconds / 10) + 48);
				lcd_time[11] = (char)((seconds % 10) + 48);
				lcd_print(0, lcd_time);
		}
}

void stage_a(int iteration) { // keyboard
		// mp3_play(conifg_num + i + 1);
		while(time > 0) {
				update();
				if (five_second.isReady()) {
						if (update_flag) {
								lcd_clear();
								// led_strip_color(0,255,0);
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



						if (keypad_presed_keys_count == 3) {
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
						//
				}
		}
		bad_final();
}

bool flag;

void stage_b(int iteration) { // artefacts
		// mp3_play(conifg_num + i + 2);
		while(time > 0) {
				update();
				if (five_second.isReady()) {
						if (update_flag) {
								lcd_clear();
								update_flag = false;
								// led_strip_color(0,255,0);
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

						flag = true;

						for (int i = 0; i <= iteration; i++) {
							if (!digitalRead(artefact_list_pins[i])){
								flag = false;
								break;
							}
						}

						if (flag) {             /*stage finished*/
								update_flag = true;
								return;
						}
				}
		}
		bad_final();
}


void stage_c(int iteration) { // jumpers
		// mp3_play(conifg_num + i + 3);
		while(time > 0) {
				update();
				// if (jump_delay.isReady()){
				// 	clock_step *= 2;
				// 	second.setInterval(500);
				// }
				if (five_second.isReady()) {
						if (update_flag) {
								lcd_clear();
								update_flag = false;
								// led_strip_color(0,255,0);
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

						flag = true;

						for (int i = 0; i <= iteration; i++) {
							if (!digitalRead(jumper_pins[i])){
								flag = false;
								break;
							}
						}

						if (flag) {             /*stage finished*/
								update_flag = true;
								// jump_delay.reset();
								// clock_step /= 2;
								// second.setInterval(1000);
								return;
						}

				}
		}
		bad_final();
}



void final_block(){
		mp3_play(9);
		ten_second.reset();
		lcd_clear();
		lcd_print(1,"     ВСТАВТЕ КЛЮЧ");
		lcd_print(2,"     ДЕ3АКТИВАЦII");
		// lcd_print(3,"Та натисніть секретні кнопки");
		while ((keys_check() != sizeof(end_keys_pins) -1) || !digitalRead(disactivation_key_pin)) {
				update();

				if (ten_second.isReady()) {
						mp3_play(9);
				}

				if (time <= 0) {bad_final();}         // finish b if timeout
		}

}

void good_final(){
		smoke_request();
		lcd_print(0,"********************");
		lcd_print(1,"******C9H13NO3******");
		lcd_print(2,"********************");
		lcd_print(3,"ATHTKBYFEEVGYDTYDSY6");

		mp3_play(2);
		for (int i = 255; i >= 0; i--) {
			led_strip_brightness(i);
			delay(20);
		}
		while(true) {delay(1000);}     // wait for reset
}

void bad_final(){
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
		good_final();
}

void loop() {
}
