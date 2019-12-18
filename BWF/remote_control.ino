////////////////////////////////////////////////////////////////////////////////
//remote control
#define chanel_pin_1 22
#define chanel_pin_2 24
#define chanel_pin_3 53
// #define chanel_pin_4 

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
