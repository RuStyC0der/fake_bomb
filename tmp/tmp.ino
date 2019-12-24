int mosfet_pin = 51;

void setup()
{
  pinMode(mosfet_pin, OUTPUT);
  digitalWrite(mosfet_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(mosfet_pin, LOW);
}

void loop()
{
  
}