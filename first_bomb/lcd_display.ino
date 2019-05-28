#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

void lcd_init()
{
        lcd.begin(20, 4);
        Serial.begin(9600);
        lcd.setCursor(0, 0);
}

void lcd_print(String text)
{
        lcd.setCursor(0, 0);
        if (text.length() > 80)
        {
                Serial.println("More then 80");
                lcd.print("More then 80");
        }
        else if (text.length() > 20 && text.length() <= 40)
        {
                lcd.print(text.substring(0,20));
                lcd.setCursor(0,1);
                lcd.print(text.substring(20,40));
                Serial.println(text);
        }
        else {
                Serial.println(text);
                lcd.print(text);
        }
}
