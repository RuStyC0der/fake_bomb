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
