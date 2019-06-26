#include <SD.h>

File file;


#define CONFIG_SIZE 12
#define SD_POWER_PIN A7
long config[CONFIG_SIZE] = {0};

void sd_setup()
{
        pinMode(SD_POWER_PIN, OUTPUT);
        sd_power(1);
        pinMode(6, OUTPUT);
        Serial.print("Initializing SD card...");
// на Ethernet шилде CS соответствует 4 пину. По умолчанию он установлен в режим output
// обратите внимание, что если он не используется в качестве CS пина, SS пин на оборудовании
// (10 на большинстве плат Arduino, 53 на Arduino Mega) надо оставить в режиме output.
// иначе функции библиотеки SD library не будут работать.
        if (!SD.begin(6)) {
                Serial.println("initialization failed!");
                return;
        }
        Serial.println("initialization done.");

}

char symbol;
bool sd_load_config(){      // thos func return 1 if config loaded succfully, and 0 if not

        file = SD.open("config.cfg");
        if (!file) {
                Serial.println("error opening config.cfg");
                pinMode(6, INPUT);

                return false;
        }

        for (int i = 0; i < CONFIG_SIZE; i++) {
          // Serial.println(i);
                while (symbol != ' ') {
                        symbol = file.read();
                        // Serial.println(symbol);
                        if (isdigit(symbol)) {
                                config[i] = (config[i] * 10) + ((int)(symbol) - 48);
                        }
                }
                while (1) {
                  symbol = file.read();
                  // Serial.println(symbol);
                  if (symbol == '\n'){
                    break;
                  }
                }
        }
        sd_power(0);
        return 1;

}
void sd_power(bool flag){
  if (flag){
    digitalWrite(SD_POWER_PIN, HIGH);
  }else{
    digitalWrite(SD_POWER_PIN, LOW);
  }
}
void setup() {
  Serial.begin(9600);
  sd_setup();
  sd_load_config();
}
void loop() {
for (int i =0 ; i < CONFIG_SIZE; i++){
  Serial.println(config[i]);
}

}
