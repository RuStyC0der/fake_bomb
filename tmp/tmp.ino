
int pin = 37;

void setup()
{
  // pinMode(pin , INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  Serial.println(digitalRead(pin));
  delay(100);
}