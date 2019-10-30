

int gerkon_pin;


void gerkon_setup(){
  pinMode(gerkon_pin, INPUT);
}

bool gerkon_auth(){
  if (1){
    return true;
  }else{
    return false;
  }
}