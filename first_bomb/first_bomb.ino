#include <Adafruit_NeoPixel.h>
#include "GyverTimer.h"
#include <SPI.h>
#include <MFRC522.h>
#include <LCD_1602_RUS.h>
#include "TM1637.h"
#include <Keypad.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


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
char keypad_presed_keys[3];
byte keypad_presed_keys_count = 0;

#define RFID_RST_PIN 5
byte rfid_keys_array[4][4] = {{91,21,228,13},{48,110,185,164},{38,136,22,18},{139,218,190,13}};
byte rfid_access_flag_key[4] = {1,1,1,1};
byte rfid_current_key[4];

#define LCD_CHARS 20
#define LCD_LINES 4

#define LED_CLK 2
#define LED_DIO 3

int8_t led_NumTab[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //0~9,A,b,C,d,E,F
int8_t led_ListDisp[4];

#define CONFIG_SIZE 12
int sd_ss_pin = 6;
int config[CONFIG_SIZE] = {0};


int mpu_first_treshold = 1000; //change me
int mpu_second_treshold = 2000; //change me
////////////////////////////////////////////////////////////////////////////////
// buzzer
int buzzer_pin = 8; //change me

////////////////////////////////////////////////////////////////////////////////
// LED line
#define PIN   7
#define NUMPIXELS 3

////////////////////////////////////////////////////////////////////////////////
//button`s
byte start_button_pin = 12;
int end_keys_pins[4] = {1,1,1,1}; // CHANGE ME BITCH!!!!!!!
int disactivation_key_pin = 13;

////////////////////////////////////////////////////////////////////////////////
//remote control
int remote_add_time_pin = 22;
int remote_minus_time_pin = 24;
int remote_1_pin = 26;
int remote_2_pin = 28;

////////////////////////////////////////////////////////////////////////////////
//jumper_pins
int jumper_pins[3] = {A8,A9,A10};

////////////////////////////////////////////////////////////////////////////////
//aertefacts pins
int artefact_list_pins[3] = {A0,A1,A2};
int artefact_led_pins[3] = {A3,A4,A5};

////////////////////////////////////////////////////////////////////////////////
//objects Instance
Keypad customKeypad = Keypad( makeKeymap(keypad_hexaKeys), keypad_rowPins, keypad_colPins, keypad_ROWS, keypad_COLS);

MPU6050 accel;

TM1637 tm1637(LED_CLK, LED_DIO);

File config_file;

LCD_1602_RUS lcd(0x27, LCD_CHARS, LCD_LINES);

MFRC522 rfid(53, RFID_RST_PIN);
MFRC522::MIFARE_Key key;

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini Player;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

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
/////////////////sound folder structure
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

void mp3_setup()
{
		mySoftwareSerial.begin(9600);
		if (!Player.begin(mySoftwareSerial)) {
				Serial.println(F("MP3_PLAYER ERROR!"));
				while (true);
		}
		Serial.println(F("DFPlayer Mini online."));
		Player.volume(20); // 0 - 30
}

void mp3_play(int track){
		Player.play(track);
}


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
				// Serial.print(current_key[i]);
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
						return 0;
				}
		}
		// delay(1000);
		return 0;
		rfid.PICC_HaltA();
		rfid.PCD_StopCrypto1();
}

void lcd_setup()
{
		lcd.init();
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
		lcd.setCursor(((LCD_CHARS - strlen(str))/2), line_num);
		lcd.print(str);
}




void keyboard_get_key() {
		if (keypad_presed_keys_count < 3) {
				char key = customKeypad.getKey();
				keypad_presed_keys[keypad_presed_keys_count] = key;
				keypad_presed_keys_count++;
		}else{
				memset(keypad_presed_keys, 95,3);
				keypad_presed_keys_count = 0;
				char key = customKeypad.getKey();
				keypad_presed_keys[keypad_presed_keys_count] = key;
				keypad_presed_keys_count++;

		}

}


void led_setup() {
		tm1637.init();
}

void led_enable() {
		tm1637.set(7);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}

void led_print_time(long time){

		int minuts = time / (1000 * 60);
		int seconds = time % (1000 * 60);
		tm1637.display(0, minuts /10);
		tm1637.display(1, minuts % 10);
		tm1637.display(2, seconds/10);
		tm1637.display(3, seconds%10);
}


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

		if (sum > mpu_second_treshold) {
				return 2;
		}else if (sum > mpu_first_treshold) {
				return 1;
		}else{
				return 0;
		}
}


void sd_setup(){
		Serial.print("Initializing SD card...");
// на Ethernet шилде CS соответствует 4 пину. По умолчанию он установлен в режим output
// обратите внимание, что если он не используется в качестве CS пина, SS пин на оборудовании
// (10 на большинстве плат Arduino, 53 на Arduino Mega) надо оставить в режиме output.
// иначе функции библиотеки SD library не будут работать.
		pinMode(sd_ss_pin, OUTPUT);
		if (!SD.begin(10)) {
				Serial.println("initialization failed!");
				return;
		}
		Serial.println("initialization done.");

}

char symbol;
bool sd_load_config(){      // thos func return 1 if config loaded succfully, and 0 if not
		config_file = SD.open("config.cfg");
		if (!config_file) {
				Serial.println("error opening config.cfg");
				return false;
		}

		for (int i; i < CONFIG_SIZE; i++ ) {
				while (symbol != ' ') {
						symbol = config_file.read();
						if (isdigit(symbol)) {
								config[i] = (config[i] * 10) + ((int)(symbol) - 48);

						}
				}
				while (config_file.read() != '\n') {

				}
		}
		return 1;
}

////////////////////////////////////////////////////////////////////////////////



GTimer_ms ten_ms(10); // try change this to 20 or 50
GTimer_ms second(1000);
GTimer_ms ten_second(10000);
GTimer_ms ten_minute(600000);

GTimer_ms five_second(5000);



bool access_flag;
byte trigger;

int fine_wait;
int del_time;
int add_time;

long time;
long access_time;


void mpu_alarm(){
		five_second.reset();
		led_strip_color(200, 0, 0);
		mp3_play(3);
}

void alarm(){
		five_second.reset();
		led_strip_color(255, 0, 0);
		mp3_play(4);
		time -= del_time;
}

int keys_check(){
		int pressed_keys_count = 0;
		for(int i = 0; i<sizeof(end_keys_pins); i++) {
				if (digitalRead(i)) {
						pressed_keys_count += 1;
				}
		}
		return pressed_keys_count;
}

void time_added(){
		time += add_time;
		// bomb reaction
}

void access_granted(){
		access_time = fine_wait;

		mp3_play(5);
		//bomb reaction
}


void update(){
		if (ten_ms.isReady()) {
				digitalWrite(buzzer_pin, LOW);
				led_print_time(time);
				time -= 10;
				access_time -= 10;
		}
		if (second.isReady()) {
				digitalWrite(buzzer_pin, HIGH);
		}

		if (digitalRead(remote_add_time_pin)) {
				time += add_time;
		}else if (digitalRead(remote_minus_time_pin)) {
				time -= add_time;
		}else if (digitalRead(remote_1_pin)) {
				// make move
		}else if (digitalRead(remote_2_pin)) {
				//make move
		}

		if (access_time > 0) {
				int flag;
				if ((customKeypad.keyStateChanged() || keys_check())  && !access_flag) {
						alarm();
				}else if ((flag = mpu_check())) {
						if(flag == 1) {
								mpu_alarm();
						}else{

								alarm();
						}
				}
		}
		byte rfid_status;
		if ((rfid_status = rfid_authentificate())) {
				if (rfid_status == 1) {
						time_added();
				}else if(access_time < 0) {
						access_granted();
				}
		}
}


void pre_init(){
		Serial.begin(9600);
		mp3_setup();
		sd_setup();
		sd_load_config();
		rfid_setup();
		mpu_setup();

		time = config[0];
		del_time = config[1];
		fine_wait = config[2];
		add_time = config[3];
		mpu_first_treshold = config[7];
		mpu_second_treshold = config[8];

		five_second.setMode(0);
}

void post_init(){
		mp3_play(1);
		led_enable();
		lcd_enable();
		lcd_print(1, "Бомбу активовано");
		lcd_print(1, "Вiдлiк почато");
}



void stage_a(int iteration) { // keyboard
		while(time < 0) {
				update();
				if (five_second.isReady()) {
				led_strip_color(0,255,0);
				lcd_print(1, keypad_presed_keys);
				char line[strlen("Введiть код #") + 2] = "Введiть код #";
				line[-1] = (char)(iteration + 48);
				lcd_print(2, line);
				if (access_time > 0){
					char lcd_time[5];
					int minuts = access_time / (1000 * 60);
					int seconds = access_time % (1000 * 60);
					lcd_time[0] = minuts /10;
					lcd_time[1] = minuts %10;
					lcd_time[2] = ':';
					lcd_time[3] = seconds /10;
					lcd_time[4] = seconds %10;

					lcd_print(3, lcd_time);
				}
				if (iteration > 0){
					lcd_print(4, config[7 + iteration]);
				}

				if (keypad_presed_keys_count == 3) {
					int num = 0;
					for (int i = 0; i < 3; i++){
						num = (num * 10) + (int)keypad_presed_keys[i] - 48;
					}
					if(num == config[iteration + 4]){
						return;
					}
				}
		}
}
finish_b();
}

void stage_b(int iteration) { // artefacts
		while(time < 0) {
				update();
				if (five_second.isReady()) {
				led_strip_color(0,255,0);
				char line[strlen("Вставте артефакт #") + 2] = "Вставте артефакт #";
				line[-1] = (char)(iteration + 48);
				lcd_print(2, line);
				if (access_time > 0){
					char lcd_time[5];
					int minuts = access_time / (1000 * 60);
					int seconds = access_time % (1000 * 60);
					lcd_time[0] = minuts /10;
					lcd_time[1] = minuts %10;
					lcd_time[2] = ':';
					lcd_time[3] = seconds /10;
					lcd_time[4] = seconds %10;

					lcd_print(3, lcd_time);
				}
				if (iteration > 0){
					lcd_print(4, config[7 + iteration]);
				}
				if (digitalRead(artefact_list_pins[iteration])) { /*stage finished*/
						Adafruit_NeoPixel pixel(NUMPIXELS, artefact_led_pins[iteration], NEO_GRB + NEO_KHZ800);
						pixel.begin();
						pixel.clear();
						pixel.setPixelColor(0, pixel.Color(1, 245, 100));
						pixel.show();

						return;
				}
		}
}
finish_b();
}

void stage_c(int iteration) { // jumpers
		while(time < 0) {
				update();
				if (five_second.isReady()) {
						lcd_clear();
						led_strip_color(0,255,0);
						char line[strlen("Вставте перемичку #") + 2] = "Вставте перемичку #";
						line[-1] = (char)(iteration + 48);
						lcd_print(2, line);
						if (access_time > 0){
							char lcd_time[5];
							int minuts = access_time / (1000 * 60);
							int seconds = access_time % (1000 * 60);
							lcd_time[0] = minuts /10;
							lcd_time[1] = minuts %10;
							lcd_time[2] = ':';
							lcd_time[3] = seconds /10;
							lcd_time[4] = seconds %10;

							lcd_print(3, lcd_time);
						}
						if (iteration > 0){
							lcd_print(4, config[7 + iteration]);
						}

						if (digitalRead(jumper_pins[iteration])) { /*stage finished*/
								return;
						}
				}
		}
		finish_b();
}


bool desactivation_key(){
		if (digitalRead(disactivation_key_pin)) {
				return true;
		}
		return false;
}

void final_block(){
		mp3_play(9);
		ten_second.reset();
		lcd_clear();
		lcd_print(2,"ВСТАВТЕ КЛЮЧ ДЕЗАКТИВАЦII");
		lcd_print(3,"Та натисніть секретні кнопки");
		while ((keys_check() != sizeof(end_keys_pins)) || !desactivation_key()) {
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
