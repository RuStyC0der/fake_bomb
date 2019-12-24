////////////////////////////////////////////////////////////////////////////////
#define MP3_DISABLE 0

#if MP3_DISABLE == 0 
// MP3_PLAYER
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void mp3_setup()
{
	mySoftwareSerial.begin(9600);
	if (!myDFPlayer.begin(mySoftwareSerial)) {
		Serial.println(F("MP3_PLAYER ERROR!"));
		while (true);
	}
	Serial.println(F("DFPlayer Mini online."));
	myDFPlayer.volume(10);     //Set volume value. From 0 to 30
	//myDFPlayer.play(4);
}

void mp3_play(int number) {
	myDFPlayer.playMp3Folder(number - 1);
}

#else

void mp3_setup(){Serial.println(F("mp3_setup called, but it disabled!"));}
void mp3_play(int number) {Serial.println(F("mp3_play disabled!"));}
#endif