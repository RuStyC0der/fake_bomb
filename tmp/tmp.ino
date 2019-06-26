int end_keys_pins[4] = {A4,A3,A2,A1}; // CHANGE ME BITCH!!!!!!!


int keys_check(){
		int pressed_keys_count = 0;
		for(int i = 0; i<sizeof(end_keys_pins); i++) {
				if (digitalRead(end_keys_pins[i])) {
						pressed_keys_count += 1;
				}
		}
		return pressed_keys_count;
}
void setup() {
  Serial.begin(9600);
}
void loop() {
  Serial.println(keys_check());
}
