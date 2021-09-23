#include "GyverTimer.h"

byte buzzer_pin = 8;
byte start_button_pin = 41;
byte disactivation_key_pin = 45;

byte jumper_pins[3] = {A10, A9, A8};

byte artefact_list_pins[3] = {34, 32, 30};

char keypad_presed_keys[3] = {'_', '_', '_'};

extern byte end_keys_pins[4];

long int config[] = {
	5400000, // 0: work time
	120000,	 // 1: del time
	10000,	 // 2: ignore time
	300000,	 // 3: add time
	348,	 // 4: first key code
	218,	 // 5: second key code
	971,	 // 6: thrid key code
	444,	 // 7: first stage code
	555,	 // 8: second stage code
	666,	 // 9: thrid stage code
	4000,	 // 10: first mpu treshold
	8000,	 // 11: second mpu treshold
	60000,	 // 12: rfid access time
};

byte stage_colors[3][3] = {
	{255, 255, 0},
	{0, 0, 255},
	{255, 0, 0}};
byte alarm_colors[3][3] = {
	{255, 0, 0},
	{0, 255, 0},
	{0, 0, 255}};

int alarm_actual_color[3] = {0, 0, 0};

int min_brightness = 5, max_brightness = 125, step_brightness = 4;
int current_brightness = min_brightness;

////////////////////////////////////////////////////////////////////////////////
//sound folder structure
// 1: Bomb activated
// 2: Bomb deactivated
// 3: The accelerometer fixed the variable position of the bomb!
// 4: The bomb is accelerated for 2 minutes
// 5: Access is allowed for 1 minute
// 6: Insert the artifact
// 7: Insert jumper
// 8: Enter the code
// 9: Insert deactivation key
// 10: sound after deactivation
// 11: end of first stage, starting second stage
// 12: end of second stage, starting thrid stage
// 13: end of thrid stage, starting final block

// PROTOTYPES
void led_strip_setup();
void led_strip_color(int, int, int);
void led_strip_Brightness(int);

void mp3_play(int);
void led_setup();
void led_print_time(long);
void led_enable();
void lcd_setup();
void lcd_enable();
void lcd_clear();
void lcd_print(int line_num, char str[]);
void lcd_print(int line_num, long value, int cursor = 0);
void gerkon_setup();
bool gerkon_auth();
void end_keys_setup();
int end_keys_presed_count();
void end_keys_light_brightness(byte br);
void smoke_setup();
bool smoke_run();
bool full_motion_detected();
bool particle_motion_detected();
bool particle_motion_detected_inverted();

////////////////////////////////////////////////////////////////////////////////
// logic

int clock_step_ms = 110; // try change this to 20 or 50

GTimer_ms second(1000);
GTimer_ms end_cycle_timer(10000);

GTimer_ms no_update_time(5000);

GTimer_ms touch_ignore_time(10000);

long forfeit_time;
int time_move_step = 300000;

long time;
long access_time = 0;

bool update_flag = true;

void motion_alarm()
{
	// Serial.println("mpu_alarm");
	lcd_clear();
	update_flag = true;
	touch_ignore_time.reset();
	no_update_time.reset();
	led_strip_color(alarm_actual_color[0], alarm_actual_color[1], alarm_actual_color[2]);
	mp3_play(3);
}

void alarm()
{
	// Serial.println("alarm");
	lcd_clear();
	update_flag = true;
	touch_ignore_time.reset();
	no_update_time.reset();
	led_strip_color(alarm_actual_color[0], alarm_actual_color[1], alarm_actual_color[2]);
	mp3_play(4);
}

void access_granted()
{
	access_time = config[12];
	mp3_play(5);
	// Serial.println("access_granted");
}

void update()
{

	bool isUpTreshold = (current_brightness >= max_brightness && step_brightness > 0);
	bool isDownTreshold = (current_brightness <= min_brightness && step_brightness < 0);

	if (isUpTreshold || isDownTreshold)
	{
		step_brightness = -step_brightness; //invert brightess step direction
	}

	led_strip_Brightness(current_brightness);
	current_brightness += step_brightness;

	digitalWrite(buzzer_pin, LOW);

	led_print_time(time);

	time -= clock_step_ms;
	access_time -= clock_step_ms;

	if (second.isReady())
	{
		digitalWrite(buzzer_pin, HIGH);

		switch (remote_check())
		{
		case 0:
			break;
		case 1:
			alarm();
			break;
		case 2:
			alarm();
			// Serial.println("remote alarm - time");
			time -= forfeit_time;
			break;
		case 3:
			smoke_run();
			break;
		}
	}

	if (access_time < 0 & gerkon_auth())
	{
		access_granted();
		// Serial.println("access_g");
	}

	if (access_time < 0)
	{

		// (int)random(1, 7)
		if (particle_motion_detected_inverted() & touch_ignore_time.isReady())
		{
			Serial.println("particle_motion_detected_inverted");
			motion_alarm();
		}

		if (keypad_check() & touch_ignore_time.isReady())
		{
			// TODO: add fake mpu detect :)
			// Serial.println("alarm keypad");
			alarm();
			time -= forfeit_time;
		}
	}
	else
	{
		keypad_update_keys(keypad_presed_keys);
	}

	delay(clock_step_ms);
}

void pre_init()
{
	Serial.begin(9600);
	pinMode(buzzer_pin, OUTPUT);
	smoke_setup();
	mp3_setup();
	led_strip_setup();
	gerkon_setup();
	led_setup();
	lcd_setup();
	remote_setup();

	time = config[0];
	forfeit_time = config[1];
	touch_ignore_time.setInterval(config[2]);

	no_update_time.setMode(0);
	touch_ignore_time.setMode(0);
}

void post_init()
{
	led_enable();
	lcd_enable();

	mp3_play(1);
	lcd_print(1, "  БOMБУ AKTИBOBAHO");
	lcd_print(2, "   BIДЛIK ПOЧATO");

	no_update_time.reset();
}

void lcd_time_print()
{
	if (access_time >= 0)
	{
		lcd_print(0, access_time, 5);
	}
}

void change_strip_color(int iteration)
{
	alarm_actual_color[0] = alarm_colors[iteration][0];
	alarm_actual_color[1] = alarm_colors[iteration][1];
	alarm_actual_color[2] = alarm_colors[iteration][2];
}

void stage_prepare_front(char line[], int iteration, int size_of_line)
{
	lcd_clear();
	led_strip_color(stage_colors[iteration][0], stage_colors[iteration][1], stage_colors[iteration][2]);

	line[size_of_line - 2] = (char)(iteration + 49);
	lcd_print(1, line);
	if (iteration > 0)
	{
		char ch[3];
		itoa(config[7 + iteration], ch, 10);
		lcd_print(3, ch);
	}
	update_flag = false;
}

void stage_a(int iteration)
{
	change_strip_color(iteration);
	while (time > 0)
	{
		update();
		if (no_update_time.isReady())
		{
			if (update_flag)
			{
				stage_prepare_front("Введiть код # ", iteration, sizeof("Введiть код # "));
			}
			lcd_time_print();
			lcd_print(2, keypad_presed_keys);

			if (keypad_presed_keys[2] != '_')
			{
				int num = 0;
				for (int i = 0; i < 3; i++)
				{
					num = (num * 10) + (int)keypad_presed_keys[i] - 48;
				}
				if (num == config[iteration + 4])
				{
					update_flag = true;
					return;
				}
			}
		}
	}
	bad_final();
}

void stage_b(int iteration)
{
	change_strip_color(iteration);
	while (time > 0)
	{
		update();
		if (no_update_time.isReady())
		{
			if (update_flag)
			{
				stage_prepare_front("Вставте артефакт # ", iteration, sizeof("Вставте артефакт # "));
			}
			lcd_time_print();

			if (digitalRead(artefact_list_pins[iteration]))
			{ /*stage finished*/
				update_flag = true;
				return;
			}
		}
	}
	bad_final();
}

void stage_c(int iteration)
{
	change_strip_color(iteration);
	while (time > 0)
	{
		update();
		if (no_update_time.isReady())
		{
			if (update_flag)
			{
				stage_prepare_front("Вставте перемичку # ", iteration, sizeof("Вставте перемичку # "));
			}
			lcd_time_print();
			if (digitalRead(jumper_pins[iteration]))
			{ /*stage finished*/
				update_flag = true;
				return;
			}
		}
	}
	bad_final();
}

void final_block()
{
	end_cycle_timer.reset();
	lcd_clear();
	lcd_print(1, "    ВСТАВТЕ КЛЮЧ");
	lcd_print(2, "    ДЕ3АКТИВАЦII");
	max_brightness = current_brightness = 250;
	mp3_play(9);
	while (true)
	{
		end_keys_light_brightness(current_brightness);
		update();
		if (end_cycle_timer.isReady())
		{
			mp3_play(9);
		}
		if (time <= 0)
		{
			bad_final();
		}
		if ((end_keys_presed_count() >= (sizeof(end_keys_pins))) && digitalRead(disactivation_key_pin))
		{
			delay(20);
			if ((end_keys_presed_count() >= (sizeof(end_keys_pins))) && digitalRead(disactivation_key_pin))
			{
				return;
			}
		}
	}
}

void good_final()
{
	digitalWrite(buzzer_pin, HIGH);
	lcd_print(0, "********************");
	lcd_print(1, "******C9H13NO3******");
	lcd_print(2, "********************");
	lcd_print(3, "ATHTKBYFEEVGYDTYDSY6");
	mp3_play(2);
	smoke_run();
	for (int i = max_brightness; i >= 0; i--)
	{
		led_strip_Brightness(i);
		end_keys_light_brightness(i);
		delay(17);
	}
	mp3_play(10);
	// delay(2000);
	digitalWrite(buzzer_pin, LOW);
	while (true)
	{
		delay(1000);
	} // wait for reset
}

void bad_final()
{
	digitalWrite(buzzer_pin, HIGH);
	while (true)
	{
		delay(1000);
	} // wait for reset
}

void validate_disactivation_key()
{
	if (digitalRead(disactivation_key_pin))
	{
		digitalWrite(buzzer_pin, HIGH);
		delay(4000);
		digitalWrite(buzzer_pin, LOW);
		while (true)
		{
			delay(1000);
		}
	}
}

void setup()
{
	pre_init();
	validate_disactivation_key();
	while (!digitalRead(start_button_pin))
	{
	} // wait to push start button
	post_init();
	for (int i = 0; i < 3; i++)
	{
		stage_a(i);
		// mp3_play(10);
		stage_b(i);
		// mp3_play(11);
		stage_c(i);
		// mp3_play(12);
	}
	final_block();
	good_final();
}

void loop() {}
