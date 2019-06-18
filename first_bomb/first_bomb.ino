#include "GyverTimer.h"
#include <SPI.h>
#include <MFRC522.h>
#include <LCD_1602_RUS.h>
#include "TM1637.h"
#include <Keypad.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"


const byte keypad_COLS = 3; // число строк клавиатуры
const byte keypad_ROWS = 4; // число столбцов клавиатуры

char keypad_hexaKeys[keypad_ROWS][keypad_COLS] = {
		{'1','2','3'},
		{'4','5','6'},
		{'7','8','9'},
		{'*','0','#'}
};

byte keypad_rowPins[keypad_ROWS] = {31, 33, 35, 37}; // к каким выводам подключаем управление строками
byte keypad_colPins[keypad_COLS] = {39, 41, 43}; // к каким выводам подключаем управление столбцами
byte keypad_presed_keys[3];
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

#define CONFIG_SIZE 7
int sd_ss_pin = 10;
int config[CONFIG_SIZE] = {0};


int mpu_first_treshold = 1; //change me
int mpu_second_treshold = 2; //change me
////////////////////////////////////////////////////////////////////////////////
//objects Instance
Keypad customKeypad = Keypad( makeKeymap(keypad_hexaKeys), keypad_rowPins, keypad_colPins, keypad_ROWS, keypad_COLS);
MPU6050 accel;
TM1637 tm1637(LED_CLK, LED_DIO);
File config_file;
LCD_1602_RUS lcd(0x27, LCD_CHARS, LCD_LINES);

MFRC522 rfid(53, RFID_RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

////////////////////////////////////////////////////////////////////////////////
// config array structure
// 0: work time time
// 1: del time
// 2: fine wait time
// 3: add time
// 4: first key code
// 5: second key code
// 6: thrid key code
// 7: first mpu treshold
// 8: second mpu treshold



void rfid_setup() {
		SPI.begin(); // Init SPI bus
		rfid.PCD_Init(); // Init MFRC522

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

		// Halt PICC
		rfid.PICC_HaltA();
		// Stop encryption on PCD
		rfid.PCD_StopCrypto1();
}


void lcd_setup()
{
		lcd.init();
}

void lcd_enable(/* arguments */) {
		lcd.backlight();
}

void lcd_print(int line_num, char str[])
{
	lcd.setCursor(((LCD_CHARS - strlen(str))/2), line_num);
	lcd.print(str);
}




void keyboard_get_key() {
		if (keypad_presed_keys_count < 3) {
				char key = customKeypad.getKey();
				keypad_presed_keys[keypad_presed_keys_count] = ((int)key - 48);
				keypad_presed_keys_count++;
		}else{
				memset(keypad_presed_keys, 0,3);
				keypad_presed_keys_count = 0;
				char key = customKeypad.getKey();
				keypad_presed_keys[keypad_presed_keys_count] = ((int)key - 48);
				keypad_presed_keys_count++;

		}

}


void led_setup() {
		tm1637.init();
}

void led_enable(/* arguments */) {
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

int end_keys_pins[4] = {1,1,1,1}; // CHANGE ME BITCH!!!!!!!
int disactivation_key_pin = 1;

GTimer_ms second(1000);
GTimer_ms ten_second(10000);
GTimer_ms ten_minute(600000);

byte startButton = 3;



bool access_flag;
byte trigger;

int fine_wait;
int del_time;
int add_time;

long time;
long access_time;


void mpu_alarm(){

}

void alarm(){

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

void update(){
		if (second.isReady()) {
				led_print_time(time);
				time -= 1000;
				access_time -= 1000;
		}
		if (access_time > 0) {
				int flag;
				if ((customKeypad.keyStateChanged() || keys_check())  && !access_flag) {
						time -= del_time;
						alarm();
				}else if ((flag = mpu_check())) {
						if(flag == 1) {
								mpu_alarm();
						}else{
								time -= del_time;
								alarm();
						}
				}
		}
		byte rfid_status;
		if ((rfid_status = rfid_authentificate())) {
				if (rfid_status == 1) {
						time += add_time;
						// bomb reaction
				}else if(access_time < 0) {
						access_time = fine_wait;
						//bomb reaction
				}
		}
}


void pre_init(){
		Serial.begin(9600);
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
}

void post_init(){
		led_enable();
		lcd_enable();
}

void stage_a() {
		while(time < 0) {
				update();
		}
		finish_b();
}

void stage_b() {
		while(time < 0) {


		}
		finish_b();

}

void stage_c() {
		while(time < 0) {

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
		ten_second.reset();
		while ((keys_check() != sizeof(end_keys_pins)) || !desactivation_key()) { // add time exception  && time > 0
				if (ten_second.isReady()) {
						// sound
				}
		}

}

void finish_a(){
		while(true) {delay(1000);} // wait for reset
}

void finish_b(){
		while(true) {delay(1000);} // wait for reset
}

void setup() {
		pre_init();
		while (!digitalRead(startButton)) {}
		post_init();
		for (int i = 0; i < 3; i++) {
				stage_a();
				stage_b();
				stage_c();
		}
		final_block();
		finish_a();
}

void loop() {
}
