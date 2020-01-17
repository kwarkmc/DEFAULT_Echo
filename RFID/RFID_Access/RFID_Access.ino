#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 4 //Chip Select 핀
#define RST_PIN 9 //Reset 핀

String content = "";

//다른 핀들은 SPI 라이브러리 사용.

MFRC522 rfid(SS_PIN, RST_PIN); // 라이브러리 생성

MFRC522::MIFARE_Key key;

byte nuidPICC[4]; // 이전 ID와 비교하기 위한 변수

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    content.concat(String(buffer[i] < 0x10 ? " 0" : " "));
    content.concat(String(buffer[i], HEX));
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin(); //SPI 시작
  rfid.PCD_Init(); //RFID 시작

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  } //초기 키 ID를 초기화 합니다.

  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID Tag : ");

  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "BD 31 15 2B") {
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
  }
  else {
    Serial.println(" Access denied");
    delay(3000);
  }

  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}
