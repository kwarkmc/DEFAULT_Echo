#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10 //Chip Select 핀
#define RST_PIN 9 //Reset 핀

//다른 핀들은 SPI 라이브러리 사용.

MFRC522 rfid(SS_PIN, RST_PIN); // 라이브러리 생성

MFRC522::MIFARE_Key key;

byte nuidPICC[4]; // 이전 ID와 비교하기 위한 변수

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i<bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i<bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin(); //SPI 시작
  rfid.PCD_Init(); //RFID 시작

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  } //초기 키 ID를 초기화 합니다.

  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key : "));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  } //카드가 인식되었다면 다음으로 넘어가고, 아니면 더이상 실행하지 않고 리턴

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("PICC type : "));

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  Serial.println(rfid.PICC_GetTypeName(piccType));

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  } //Type 이 MIFARE 방식이면 다음으로 넘어가고, 아니면 return.

  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("New card has been detected."));
      //바로 전에 인식한 카드와 다른 ID 를 가진다면
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    } //ID를 저장

    Serial.println(F("The NUID tag is : "));
    Serial.print(F("In Hex : "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    Serial.print(F("In dec : "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Card read previously."));

  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}
