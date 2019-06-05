#include <SD.h>

File myFile;

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
// открываем файл для считывания данных:
        myFile = SD.open("test.txt");
        if (myFile) {
                Serial.println("test.txt:");
// считываем все данные из файла:
                while (myFile.available()) {
                        Serial.write(myFile.read());
                }
// закрываем файл:
                myFile.close();
        } else {
// если файл не открылся, отображаем сообщение об ошибке:
                Serial.println("error opening test.txt");
        }
}
