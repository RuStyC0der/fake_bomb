#include <SD.h>

File file;

#define CONFIG_SIZE 7
int config[CONFIG_SIZE] = {0};

void sd_setup()
{
        Serial.begin(9600);
        Serial.print("Initializing SD card...");
// на Ethernet шилде CS соответствует 4 пину. По умолчанию он установлен в режим output
// обратите внимание, что если он не используется в качестве CS пина, SS пин на оборудовании
// (10 на большинстве плат Arduino, 53 на Arduino Mega) надо оставить в режиме output.
// иначе функции библиотеки SD library не будут работать.
        pinMode(10, OUTPUT);
        if (!SD.begin(10)) {
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
                return false;
        }

        for (int i; i < CONFIG_SIZE; i++ ) {
                while (symbol != ' ') {
                        symbol = file.read();
                        if (isdigit(symbol)) {
                                config[i] = (config[i] * 10) + ((int)(symbol) - 48);

                        }
                }
                while (file.read() != '\n') {

                }
        }
        return 1;
}
