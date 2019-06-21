#include "DFRobotDFPlayerMini.h"



void mp3_setup()
{
		mySoftwareSerial.begin(9600);
		if (!Player.begin(mySoftwareSerial)) {
				Serial.println(F("MP3_PLAYER ERROR!"));
				while (true);
		}
		Serial.println(F("DFPlayer Mini online."));
		Player.volume(20); // 0 - 30
}

void mp3_play(int track){
		Player.play(track);
}

/****************************************************************
 *		mp3TF mp3tf = mp3TF ();
 *	UNO:
 *		Serial.begin (9600);
 *		mp3tf.init (&Serial);
 *	LEONARDO:
 *		Serial1.begin (9600);
 *		mp3tf.init (&Serial1);
 *
 *	function:
 *		mp3tf.play ();		play once
 *		mp3tf.play (1);		play first
 *		mp3tf.play (3);		play third
 *		mp3tf.pause ();		pause
 *		mp3tf.stop ();		stop (на самом деле это таже функция что и пауза)
 *		mp3tf.next ();		next
 *		mp3tf.previous ();	previous
 *		mp3tf.upVolume ();	upVolume
 *		mp3tf.downVolume ();	downVolume
 *		mp3tf.setVolume (20);	setVolume 20
 *****************************************************************/
