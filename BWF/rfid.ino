////////////////////////////////////////////////////////////////////////////////
//rfid
#include <MFRC522.h>
#include <SPI.h>
#define RFID_RST_PIN 5
byte rfid_keys_array[4][4] = {{78,189,226,33},{126,35,94,33},{38,136,22,18},{18,164,106,29}};
byte rfid_access_flag_key[4] = {48,110,185,164};
byte rfid_current_key[4];

MFRC522 rfid(53, RFID_RST_PIN);
MFRC522::MIFARE_Key key;

void rfid_setup() {
		SPI.begin();
		rfid.PCD_Init();
		for (byte i = 0; i < 6; i++) {
				key.keyByte[i] = 0xFF;
		}
}

int rfid_authentificate() {
		if ( !rfid.PICC_IsNewCardPresent())
				return 0;
		if ( !rfid.PICC_ReadCardSerial())
				return 0;

		Serial.print(F("PICC type: "));
		MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
		Serial.println(rfid.PICC_GetTypeName(piccType));
		// Check is the PICC of Classic MIFARE type
		if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
		    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
		    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
				Serial.println(F("Your tag is not of type MIFARE Classic."));
				return 0;
		}
		for (byte i = 0; i < 4; i++) {
				rfid_current_key[i] = rfid.uid.uidByte[i];
				// Serial.print(rfid_current_key[i]);
				// Serial.print(",");
		}
		// Serial.println();
		// delay(1000);

		bool flag = true;

		for (byte i = 0; i < 4; i++) {
				if (rfid_current_key[i] != rfid_access_flag_key[i]) {
						flag = false;
				}
		}
		if (flag == true) {
				return 2;
		}

		for (byte i = 0; i < 4; i++) {
				flag = true;
				// Serial.println(i);
				for (byte j = 0; j < 4; j++) {

						if (rfid_current_key[j] != rfid_keys_array[i][j]) {
								flag = false;

								break;
						}
				}
				if (flag == true) {
						memset(rfid_keys_array[i], 0, sizeof(rfid_keys_array[i]));
						return 1;
				}
		}
		// delay(1000);
		return 0;
		rfid.PICC_HaltA();
		rfid.PCD_StopCrypto1();
}
