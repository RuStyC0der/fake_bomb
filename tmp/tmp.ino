// move detector



int pin = 7;

void setup()
{
  // pinMode(pin , INPUT;)
  Serial.begin(9600);
}

void loop()
{
  // if (digitalRead(pin))
  // {
  //     Serial.println("pass");
  // }
  
  Serial.println(digitalRead(pin));

  delay(100);
}