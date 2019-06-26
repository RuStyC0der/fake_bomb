////////////////////////////////////////////////////////////////////////////////
// buzzer
int buzzer_pin = 8; //change me

////////////////////////////////////////////////////////////////////////////////
//button`s
byte start_button_pin = 12;
int end_keys_pins[4] = {14,15,16,17}; // CHANGE ME BITCH!!!!!!!
int disactivation_key_pin = 13;

////////////////////////////////////////////////////////////////////////////////
//jumper_pins
int jumper_pins[3] = {A8,A9,A10};

////////////////////////////////////////////////////////////////////////////////
//aertefacts pins
int artefact_list_pins[3] = {30,32,33};
int artefact_led_pins[3] = {36,38,40};

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
#define PIN   7
#define NUMPIXELS 3

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

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void mp3_setup()
{
		mySoftwareSerial.begin(9600);
		if (!myDFPlayer.begin(mySoftwareSerial)) {
				Serial.println(F("MP3_PLAYER ERROR!"));
				while (true);
		}
		Serial.println(F("DFPlayer Mini online."));
		myDFPlayer.volume(20); //Set volume value. From 0 to 30
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

int mpu_first_treshold = 2000; //change me
int mpu_second_treshold = 4000; //change me

MPU6050 accel;

void mpu_setup(/* arguments */) {
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
		Wire.begin();
	#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
		Fastwire::setup(400, true);
	#endif
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
		unsigned int sum;

		accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);
		sum = abs((constrain((gx_raw + gy_raw + gz_raw), -48000, 48000)));
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
		tm1637_6D.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

}

void led_enable(/* arguments */) {
		tm1637_6D.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
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

byte keypad_rowPins[keypad_ROWS] = {31, 33, 35, 37};
byte keypad_colPins[keypad_COLS] = {39, 41, 43};
char keypad_presed_keys[3] = "___";
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

GTimer_ms step_time(40); // try change this to 20 or 50
GTimer_ms second(1000);
GTimer_ms ten_second(10000);


GTimer_ms five_second(5000);

GTimer_ms ignore_time(10000);


long access_time_config;
long del_time;
long add_time;

long time;
long access_time = 0;

bool alarm_flag = true;


void mpu_alarm(){
		bool alarm_flag = true;
		ignore_time.reset();
		five_second.reset();
		led_strip_color(200, 0, 0);
		mp3_play(3);
}

void alarm(){
		bool alarm_flag = true;
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
		time += 300000;
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
				time -= 40;
				access_time -= 40;
				if (second.isReady()) {
						// Serial.println(access_time);
						// Serial.println(add_time);
						// Serial.println(del);

						digitalWrite(buzzer_pin, HIGH);
						switch (remote_check()) {
						case 0:
								break;
						case 1:
								time += 300000;
								break;
						case 2:
								time -= 300000;
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
						}
				}else{
						keypad_get_key();
				}
		}

}



void pre_init(){
		pinMode(buzzer_pin, OUTPUT);
		Serial.begin(9600);
		mp3_setup();

		rfid_setup();
		led_setup();
		lcd_setup();
		// delay(500);
		sd_setup();
		sd_load_config();
		mpu_setup();


		time = config[0];
		del_time = config[1];
		ignore_time.setInterval(config[2]);
		add_time = config[3];
		// mpu_first_treshold = config[10];
		// mpu_second_treshold = config[11];
		mpu_first_treshold = 16000;
		mpu_second_treshold = 20000;
		// access_time_config = config[12];
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
						if (alarm_flag) {
								lcd_clear();
								alarm_flag = false;
								led_strip_color(0,255,0);
								char line[] = "Введiть код # ";
								line[sizeof(line) - 2] = (char)(iteration + 49);
								lcd_print(1, line);
								if (iteration > 0) {
										char ch[3];
										itoa(config[7 + iteration],ch,10);
										lcd_print(3, ch);
								}
						}
						lcd_time_print();
						lcd_print(2, keypad_presed_keys);



						if (keypad_presed_keys_count == 3) {
								int num = 0;
								for (int i = 0; i < 3; i++) {
										num = (num * 10) + (int)keypad_presed_keys[i] - 48;
								}
								if(num == config[iteration + 4]) {
										alarm_flag = true;
										return;
								}
						}
				}else{
						//
				}
		}
		finish_b();
}



void stage_b(int iteration) { // artefacts
		while(time > 0) {
				update();
				if (five_second.isReady()) {
						if (alarm_flag) {
								lcd_clear();
								alarm_flag = false;
								led_strip_color(0,255,0);
								char line[] = "Вставте артефакт # ";
								line[sizeof(line) - 2] = (char)(iteration + 49);
								lcd_print(2, line);
								if (iteration > 0) {
										char ch[3];
										itoa(config[7 + iteration],ch,10);
										lcd_print(3, ch);
								}
								lcd_time_print();

						}
						if (digitalRead(artefact_list_pins[iteration])) { /*stage finished*/
								Adafruit_NeoPixel pixel(NUMPIXELS, artefact_led_pins[iteration], NEO_GRB + NEO_KHZ800);
								pixel.begin();
								pixel.clear();
								pixel.setPixelColor(0, pixel.Color(0, 255, 0));
								pixel.show();
								alarm_flag = true;
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
						if (alarm_flag) {
								lcd_clear();
								alarm_flag = false;
								led_strip_color(0,255,0);
								char line[] = "Вставте перемичку # ";
								line[sizeof(line) - 2] = (char)(iteration + 49);
								lcd_print(2, line);

								if (iteration > 0) {
										char ch[3];
										itoa(config[7 + iteration],ch,10);
										lcd_print(3, ch);
								}
								lcd_time_print();
						}
						if (digitalRead(jumper_pins[iteration])) { /*stage finished*/
								alarm_flag = true;
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
		lcd_print(2,"ВСТАВТЕ КЛЮЧ ДЕЗАКТИВАЦII");
		// lcd_print(3,"Та натисніть секретні кнопки");
		while ((keys_check() != sizeof(end_keys_pins) -1) || !digitalRead(disactivation_key_pin)) {
				update();

				if (ten_second.isReady()) {
						mp3_play(9);
				}

				if (time <= 0) {finish_b();} // finish b if timeout
		}

}

void finish_a(){
		mp3_play(2);
		for (int i = 255; i >= 0; i--) {
				pixels.setBrightness(i);
				delay(20);
				pixels.show();
		}
		while(true) {delay(1000);} // wait for reset
}

void finish_b(){
		while(true) {delay(1000);} // wait for reset
}
// void setup() {
//      pre_init();
//      delay(2000);
//      post_init();
// }
//
// void loop() {
//      update();
// }
void setup() {
		pre_init();
		while (!digitalRead(start_button_pin)) {} // wait to push start button
		post_init();
		for (int i = 0; i < 3; i++) {
				stage_a(i);
				stage_b(i);
				stage_c(i);
		}
		final_block();
		finish_a();
}

void loop() {
}
