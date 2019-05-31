#include <LCD_1602_RUS.h>

LCD_1602_RUS lcd(0x27, 20, 4);

void lcd_setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print(L"АКТИВОВАНО");

}

void lcd_print(char str[])
{
    lcd.setCursor(0, 0);
	char * pch;
  	pch = strtok (str,"|");
    int i = 0;
	while (pch != NULL)
	{
      lcd.setCursor(0, i);
      i++;
	  Serial.println(pch);
      lcd.print(pch);
	  pch = strtok (NULL, "|");
	}
}

void lcd_loop() {
	char str[] ="alarm|need code to|desactivation";
	lcd_print(str);
}
