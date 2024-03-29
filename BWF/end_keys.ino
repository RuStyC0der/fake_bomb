
// byte end_keys_pins[1] = {A12};
//  A15 yellow right side
//  
byte end_keys_pins[4] = {A12,A13,A14,A15};
byte end_keys_light_pin = 46; // this is not 11

void end_keys_setup(){
    pinMode(end_keys_light_pin, OUTPUT);
}

int end_keys_presed_count(){
		int pressed_keys_count = 0;
		for(int i = 0; i<sizeof(end_keys_pins); i++) {
				if (digitalRead(end_keys_pins[i])) {
						pressed_keys_count += 1;
				}
		}
		return pressed_keys_count;
}

void end_keys_light_brightness(byte br){
    analogWrite(end_keys_light_pin, br);
}