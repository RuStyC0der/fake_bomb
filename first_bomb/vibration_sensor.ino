#define VIBRO_1_PIN 8
#define VIBRO_2_PIN 7

void vibro_init(){
  pinMode(VIBRO_1_PIN, INPUT);
  pinMode(VIBRO_2_PIN, INPUT);
}

bool vibro_1(/* arguments */) {
        return digitalRead(VIBRO_1_PIN);
}

bool vibro_2(/* arguments */) {
      return digitalRead(VIBRO_2_PIN);
}
