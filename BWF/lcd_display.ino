////////////////////////////////////////////////////////////////////////////////
//LCD display
#include <LCD_1602_RUS.h>
#define LCD_CHARS 20
#define LCD_LINES 4

LCD_1602_RUS lcd(0x27, LCD_CHARS, LCD_LINES);

void lcd_setup()
{
		lcd.init();
		lcd.noBacklight();
		Serial.print(" Lcd status is : ");
		Serial.println(lcd.status());
}

void lcd_enable(/* arguments */) {
		lcd.backlight();
}

void lcd_clear(){
		lcd.clear();
}

void lcd_print(int line_num, char str[]){
		lcd.setCursor(0, line_num);
		lcd.print(str);
}

void lcd_print(int line_num, long value, int cursor = 0){
		lcd.setCursor(cursor, line_num);
		lcd.print(value);
}
