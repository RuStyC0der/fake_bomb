#define VIBRO_1_PIN 6
#define VIBRO_2_PIN 7

bool vibro_state[2];

void vibro_update(/* arguments */) {
        vibro_state[0] = digitalRead(VIBRO_1_PIN);
        vibro_state[1] = digitalRead(VIBRO_2_PIN);
}
