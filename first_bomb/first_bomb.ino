void setup() {
        lcd_setup();
        led_setup();
        rfid_setup();
        sd_setup();
        mp3_setup();
}

char str[] = "УВАГА|АКТИВОВАНО БОМБУ|КОД:";
char str1[] = "Test firmaware|hello world|lcdtest end";

void loop() {
        lcd_print(str1);
        led_print(1,5,7,2);
        rfid_authentificate();
}
