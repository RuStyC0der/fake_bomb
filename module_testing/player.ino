
#include <mp3TF.h>

mp3TF mp3tf = mp3TF ();

void mp3_setup () {
	delay (5000);
	Serial1.begin(9600);

	mp3tf.init (&Serial1);
	Serial.begin (9600);
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
