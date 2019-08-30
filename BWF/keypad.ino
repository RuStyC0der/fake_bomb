
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
byte keypad_colPins[keypad_COLS] = {A2, A1, A0};
byte keypad_presed_keys_count = 0;

Keypad customKeypad = Keypad( makeKeymap(keypad_hexaKeys), keypad_rowPins, keypad_colPins, keypad_ROWS, keypad_COLS);

void keypad_update_keys(char* keypad_presed_keys) {
		if (keypad_presed_keys_count < 3) {
				char key = customKeypad.getKey();
				if(key) {
						keypad_presed_keys[keypad_presed_keys_count] = key;
						keypad_presed_keys_count++;
				}

		}else{
				memset(keypad_presed_keys, '_',3);
				keypad_presed_keys_count = 0;
				char key = customKeypad.getKey();
				if (key) {
						keypad_presed_keys[keypad_presed_keys_count] = key;
						keypad_presed_keys_count++;
				}
		}

}



bool keypad_check(){
		return customKeypad.getKey();
}
