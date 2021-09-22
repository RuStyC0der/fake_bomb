////////////////////////////////////////////////////////////////////////////////
//remote control
#define REMOTE_DISABLE 0

#if REMOTE_DISABLE == 0 

#define chanel_pin_1 47
#define chanel_pin_2 49
#define chanel_pin_3 51
// #define chanel_pin_4 

void remote_setup(){
	pinMode(chanel_pin_1, INPUT);
	pinMode(chanel_pin_2, INPUT);
	pinMode(chanel_pin_3, INPUT);
}

int remote_check(){
		if (digitalRead(chanel_pin_1)) {
				return 1;
		}else if (digitalRead(chanel_pin_2)) {
				return 2;
		}else if(digitalRead(chanel_pin_3)) {
				return 3;
		// }else if (digitalRead(chanel_pin_4)) {
		// 		return 4;
		}else{
				return 0;
		}
		// return 0;
}

#else

int remote_check(){
	Serial.println(F("remote_check called, but it disabled!"));
	}
#endif