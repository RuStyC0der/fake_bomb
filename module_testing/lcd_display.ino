#include <LCD_1602_RUS.h>
#define CHARS 20
#define LINES 4
LCD_1602_RUS lcd(0x27, CHARS, LINES);

void lcd_setup()
{
        lcd.init();
}

void lcd_enable(/* arguments */) {
  lcd.backlight();
}

void lcd_print(char str[])
{
        char * sub_str;
        sub_str = strtok (str,"|");
        int i = 0;
        while (sub_str != NULL)
        {
                lcd.setCursor((CHARS/2) - (strlen(sub_str) / 2), i);
                i++;
                //Serial.println(sub_str);
                lcd.print(sub_str);
                sub_str = strtok (NULL, "|");
        }
}

void lcd_clear() {
        lcd.clear();
}
