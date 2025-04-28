#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 23
#define SCL_PIN 22
#define BUZZER_PIN 21

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("LIGADNO O PN532...");
    pinMode(BUZZER_PIN, OUTPUT); 
    nfc.begin();

    uint32_t version = nfc.getFirmwareVersion();
    if (!version) {
        Serial.println("PN532 NÃO ENCONTRADO!");
        while (1);
    }

    nfc.SAMConfig();
    Serial.println("PN532 INICIALIZADO!");
}

void loop() {
    Serial.println("AGUARDANDO LEITURA DE TAG...");
    uint8_t success;
    uint8_t uid[] = {0};
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (success) {
        Serial.print("TAG NFC ENCONTRADA: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(uid[i], HEX);
        }
        Serial.println();
        digitalWrite(BUZZER_PIN, HIGH); // LIGA O BUZZER
        delay(100); 
        digitalWrite(BUZZER_PIN, LOW); // DESLIGA O BUZZER
        delay(100); 
        digitalWrite(BUZZER_PIN, HIGH); // LIGA O BUZZER
        delay(100); 
        digitalWrite(BUZZER_PIN, LOW); // DESLIGA O BUZZER
    }
    delay(1000);
}
