int gerkon_pin = 7; // this is not 11

void gerkon_setup(){
  pinMode(gerkon_pin, INPUT);
}

bool gerkon_auth(){
  return digitalRead(gerkon_pin);
}
