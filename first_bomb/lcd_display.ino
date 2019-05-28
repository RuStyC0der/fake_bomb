#include <LiquidCrystal.h>
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

void lcd_init()
{
        lcd.begin(16, 2);
        Serial.begin(9600);
        lcd.setCursor(0, 0);
}

void lcd_print(String text)
{
        lcd.setCursor(0, 0);
        if (text.length() > 32)
        {
                Serial.println("More then 32");
                lcd.print("More then 32");
        }
        else if (text.length() > 16 && text.length() <= 32)
        {
                lcd.print(text.substring(0,16));
                lcd.setCursor(0,1);
                lcd.print(text.substring(16,32));
                Serial.println(text);
        }
        else {
                Serial.println(text);
                lcd.print(text);
        }
}
