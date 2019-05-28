#define GERKON_1_PIN 10
#define GERKON_2_PIN 11
#define GERKON_3_PIN 12

int gerkon_state[3];

void gerkons_update(){
        gerkon[0] = digitalRead(GERKON_1_PIN);
        gerkon[1] = digitalRead(GERKON_2_PIN);
        gerkon[2] = digitalRead(GERKON_3_PIN);
}
