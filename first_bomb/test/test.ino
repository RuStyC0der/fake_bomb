#include <Keypad.h>

const byte ROWS = 4; // число строк клавиатуры
const byte COLS = 4; // число столбцов клавиатуры
char hexaKeys[ROWS][COLS] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
};

byte rowPins[ROWS] = {31, 33, 35, 37}; // к каким выводам подключаем управление строками
byte colPins[COLS] = {39, 41, 43,45}; // к каким выводам подключаем управление столбцами
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



char keyboard_get_key() {
        return customKeypad.getKey();
}

void setup() {
        Serial.begin(9600);
}

void loop() {
        char key = keyboard_get_key();
        if (key) {
                Serial.println(key);
        }
}
