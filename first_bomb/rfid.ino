/*
   ------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte keys_array[4][4] = {{91,21,228,13},{48,110,185,164},{38,136,22,18},{139,218,190,13}};
byte current_key[4];

void rfid_setup() {
        // Serial.begin(9600);
        SPI.begin(); // Init SPI bus
        rfid.PCD_Init(); // Init MFRC522

        for (byte i = 0; i < 6; i++) {
                key.keyByte[i] = 0xFF;
        }


}

int rfid_authentificate() {

        // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        if ( !rfid.PICC_IsNewCardPresent())
                return -1;

        // Verify if the NUID has been readed
        if ( !rfid.PICC_ReadCardSerial())
                return -1;

        Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        Serial.println(rfid.PICC_GetTypeName(piccType));

        // Check is the PICC of Classic MIFARE type
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
            piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
            piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
                Serial.println(F("Your tag is not of type MIFARE Classic."));
                return -1;
        }


        for (byte i = 0; i < 4; i++) {
                current_key[i] = rfid.uid.uidByte[i];
                // Serial.print(current_key[i]);
                // Serial.print(",");
        }
        // Serial.println();
        // delay(1000);

        bool flag = false;

        for (byte i = 0; i < 4; i++) {
                flag = true;
                // Serial.println(i);
                for (byte j = 0; j < 4; j++) {

                        if (current_key[j] != keys_array[i][j]) {
                                flag = false;

                                break;
                        }
                }
                if (flag == true) {
                        return true;
                }
        }
        // delay(1000);
        return false;

        // Halt PICC
        rfid.PICC_HaltA();

        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
}
