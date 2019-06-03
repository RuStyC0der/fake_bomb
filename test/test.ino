
/*
 *	Copyright:	DFRobot
 *	name:		DFPlayer_Mini_Mp3 test code
 *	Author:		lisper <lisper.li@dfrobot.com>
 *	Date:		2014-05-22
 *	Description:	mp3 library for DFPlayer mini board
 *
 *	this code is test on leonardo
 *	you can try input:
 *	play			//play current music
 *	play 3			//play mp3/0003.mp3
 *	next			//play next
 *	prev			//paly previous
 *	pause			//pause	current play
 *	stop			//stop current play
 *	state       //get current state of module
 *	current			//get current track of tf card
 *	volume			//get current volume
 *	volume 20		//set volume to 20 (0~30)
 *	single open/close   //set single loop open or close
 *	reply open/close	//set if need reply
 */

// #include <SoftwareSerial.h>
// #include <DFRobot_utility.h>

#include "DFPlayer_Mini_Mp3.h"

// #define BUFSIZE 20  //buf size
// #define CMDNUM 8  //cmdbuf size
//
// uint8_t buf[BUFSIZE]; //data buffer for read from Serial
// char *cmdbuf[CMDNUM]; //for split string from buf

//
void setup () {
        Serial1.begin (9600);
        Serial.begin (9600);
        while (!Serial);

        mp3_set_serial (Serial1); //set Serial1 for DFPlayer-mini mp3 module
        mp3_set_volume (15);
        // mp3_get_tf_sum ();
        // print_info ();
}



//
void loop () {
        mp3_play(2);
}
