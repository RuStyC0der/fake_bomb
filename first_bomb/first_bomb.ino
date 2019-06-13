#include "GyverTimer.h"
#include <SPI.h>
#include <MFRC522.h>
#include <LCD_1602_RUS.h>
#include "TM1637.h"
#include <Keypad.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
////////////////////////////////////////////////////////////////////////////////
// Keyboard variables
const byte COLS = 3; // число строк клавиатуры
const byte ROWS = 4; // число столбцов клавиатуры

char hexaKeys[ROWS][COLS] = {
		{'1','2','3'},
		{'4','5','6'},
		{'7','8','9'},
		{'*','0','#'}
};

byte rowPins[ROWS] = {31, 33, 35, 37}; // к каким выводам подключаем управление строками
byte colPins[COLS] = {39, 41, 43}; // к каким выводам подключаем управление столбцами
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int presed_keys[3];
byte presed_keys_count = 0;
////////////////////////////////////////////////////////////////////////////////
//rfid variables
#define RST_PIN 5

MFRC522 rfid(53, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte keys_array[4][4] = {{91,21,228,13},{48,110,185,164},{38,136,22,18},{139,218,190,13}};
byte current_key[4];
////////////////////////////////////////////////////////////////////////////////
// lcd variables
#define CHARS 20
#define LINES 4
LCD_1602_RUS lcd(0x27, CHARS, LINES);
////////////////////////////////////////////////////////////////////////////////
// led variables
#define CLK 2
#define DIO 3

TM1637 tm1637(CLK, DIO);
int8_t NumTab[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //0~9,A,b,C,d,E,F
int8_t ListDisp[4];
////////////////////////////////////////////////////////////////////////////////
// SD card variables
File file;

#define CONFIG_SIZE 7
int config[CONFIG_SIZE] = {0};
////////////////////////////////////////////////////////////////////////////////
//mpu variables
MPU6050 accel;

int first_treshold = 1; //change me
int second_treshold = 2; //change me

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

bool rfid_authentificate() {

		// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
		if ( !rfid.PICC_IsNewCardPresent())
				return false;

		// Verify if the NUID has been readed
		if ( !rfid.PICC_ReadCardSerial())
				return false;

		Serial.print(F("PICC type: "));
		MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
		Serial.println(rfid.PICC_GetTypeName(piccType));

		// Check is the PICC of Classic MIFARE type
		if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
		    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
		    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
				Serial.println(F("Your tag is not of type MIFARE Classic."));
				return false;
		}


		for (byte i = 0; i < 4; i++) {
				current_key[i] = rfid.uid.uidByte[i];
				// Serial.print(current_key[i]);
				// Serial.print(",");
		}
		// Serial.println();
		// delay(1000);

		bool flag = false;

		for (byte i = 0; i < 4; i++) {
				flag = true;
				// Serial.println(i);
				for (byte j = 0; j < 4; j++) {

						if (current_key[j] != keys_array[i][j]) {
								flag = false;

								break;
						}
				}
				if (flag == true) {
						return true;
				}
		}
		// delay(1000);
		return false;

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

void lcd_print(char str[])
{
		char * sub_str;
		sub_str = strtok (str,"|");
		int i = 0;
		while (sub_str != NULL)
		{
				lcd.setCursor((CHARS/2) - (strlen(sub_str) / 2), i);
				i++;
				//Serial.println(sub_str);
				lcd.print(sub_str);
				sub_str = strtok (NULL, "|");
		}
}




void keyboard_get_key() {
		if (presed_keys_count < 3) {
				char key = customKeypad.getKey();
				presed_keys[presed_keys_count] = ((int)key - 48);
				presed_keys_count++;
		}else{
				memset(presed_keys, 0,3);
				presed_keys_count = 0;
				char key = customKeypad.getKey();
				presed_keys[presed_keys_count] = ((int)key - 48);
				presed_keys_count++;

		}

}


void led_setup() {
		tm1637.init();
}

void led_enable(/* arguments */) {
		tm1637.set(7);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
}

void led_print(int num_0, int num_1, int num_2, int num_3){
		tm1637.display(0, num_0);
		tm1637.display(1, num_1);
		tm1637.display(2, num_2);
		tm1637.display(3, num_3);
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

		if (sum > second_treshold) {
				return 2;
		}else if (sum > first_treshold) {
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
		pinMode(10, OUTPUT);
		if (!SD.begin(10)) {
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
				return false;
		}

		for (int i; i < CONFIG_SIZE; i++ ) {
				while (symbol != ' ') {
						symbol = file.read();
						if (isdigit(symbol)) {
								config[i] = (config[i] * 10) + ((int)(symbol) - 48);

						}
				}
				while (file.read() != '\n') {

				}
		}
		return 1;
}



////////////////////////////////////////////////////////////////////////////////

int final_keys_pins[4] = {1,1,1,1} // CHANGE ME BITCH!!!!!!!


GTimer_ms second(1000);
GTimer_ms fine_wait(1000);

byte startButton = 3;



bool acces_flag;
byte trigger;
int del_time;
int add_time;

unsigned long time;

void mpu_alarm(){

}

void alarm(){

}

void update_led_time(){
		int minuts = time / (1000 * 60);
		int seconds = time % (1000 * 60);
		led_print(minuts /10,minuts % 10,seconds/10, seconds%10);
}

int keys_check(){

}

void update(){
		if (second.isReady()) {
				update_led_time();
		}
		if (fine_wait.isReady()) {
				int flag;
				if ((customKeypad.keyStateChanged() || keys_check())  && !acces_flag) {
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
}


void pre_init(){
		Serial.begin(9600);
		sd_setup();
		sd_load_config();
		rfid_setup();
		mpu_setup();
		time = config[0] * 60 * 1000;
		del_time = config[1];
		fine_wait.setInterval(config[2] * 1000);
		add_time = config[3];
		first_treshold = config[7];
		second_treshold = config[8];
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

void final_block(){

}

void finish_a(){
		return;
}

void finish_b(){
		return;
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
