#include <SPI.h>
#include <MFRC522.h>
#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <Servo.h>

Servo servo;

#define DB_LENGTH 2

#define ITEM_INIT_IDX 6
#define ITEM_END_IDX 9

#define CUP_DIST 50

#define SS_PIN_RFID 4
#define SS_PIN_BARC 10
#define RST_PIN 9 //Reset 핀
#define RELAY 6
#define echoPin 2
#define trigPin 3

String content = "";
float distance = 0;
int mode = 0;
char BarcodeString[30];
char* Var = "Hello";
String BarcodeDB[DB_LENGTH] = {"abc", "123"};
int Material = 0;
int BarcodeCompletion;
int ref_angle_value = 90;
int target_angle = 0;
int delta_angle = 0;

class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

int flag = 0;

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;


  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }
    return;
  }
}



uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters

  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) {
      return (key - 4 + 'A');
    }
    else {
      return (key - 4 + 'a');
    }
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  BarcodeCompletion = 1;
  char ascii = KeyToAscii(upper, mod, key);

  BarcodeString[flag] = ascii;
  flag++;

  Serial.print((char)ascii);
}



void MyParser::OnScanFinished() {
  Serial.println("");
  BarcodeString[flag] = '\0';
  String ConvertedString = String(BarcodeString);
  String ItemString = ConvertedString.substring(ITEM_INIT_IDX, ITEM_END_IDX);
  Serial.print("Item : ");
  Serial.println(ItemString);

  if (ItemString.equals(BarcodeDB[0])) {
    Serial.println("Plastic");
    Material = 1;
  }
  else if (ItemString.equals(BarcodeDB[1])) {
    Serial.println("Paper");
    Material = 2;
  }
  else {
    Serial.println("None");
    Material = 0;
  }

  flag = 0;

  BarcodeCompletion = 0;

  Serial.println(" - Finished");

}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

MFRC522 rfid(SS_PIN_RFID, RST_PIN); // 라이브러리 생성

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

  servo.attach(7);
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(SS_PIN_RFID, OUTPUT);
  digitalWrite(SS_PIN_RFID, LOW);
  //  pinMode(SS_PIN_BARC, OUTPUT);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  } //초기 키 ID를 초기화 합니다.

  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  //  if (Usb.Init() == -1) {
  //    Serial.println("OSC did not start.");
  //  }

  delay( 200 );

  //  Hid.SetReportParser(0, &Parser);

  SPI.begin(); //SPI 시작
  rfid.PCD_Init(); //RFID 시작
}

void loop() {
  digitalWrite(trigPin, LOW);
  digitalWrite(echoPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //
  if (BarcodeCompletion == 0 ) {
    unsigned long duration = pulseIn(echoPin, HIGH);
  }
  //  distance = ((float)(340 * duration) / 10000) / 2;
  //  Material = 0;

  /* if (distance < CUP_DIST) {
     mode = 1; // 컵이 들어왔을 경우
    }
    else {
     mode = 2; // 컵이 들어오지 않았을 경우
    }*/

  //  mode = 2;
  //
  //  switch (mode) {
  //    case 1:
  digitalWrite(SS_PIN_BARC, HIGH);
  digitalWrite(SS_PIN_RFID, LOW);
  Serial.println("RFID READY!");

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID Tag : ");

//  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  printHex(rfid.uid.uidByte, rfid.uid.size);

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "89 3B F1 B8") {
    Serial.println("Authorized access");
    digitalWrite(RELAY, HIGH);
    delay(1000);
    digitalWrite(RELAY, LOW);
    Serial.println();
    delay(1000);
  }
  else {
    Serial.println(" Access denied");
    delay(500);
  }

//  rfid.PICC_HaltA();
//
//  rfid.PCD_StopCrypto1();
  //
  //      break;
  //    case 2:
  //      digitalWrite(SS_PIN_RFID, HIGH);
  //      digitalWrite(SS_PIN_BARC, LOW);
  //      Usb.Task();
  //      while (Material == 0) {
  //        delay(10);
  //      }
  //      if (Material == 1) {
  //        target_angle = 0;
  //        delta_angle = abs(target_angle - ref_angle_value);
  //        servo.write(ref_angle_value - delta_angle);
  //        delay(delta_angle * 11);
  //        servo.write(ref_angle_value);
  //        digitalWrite(RELAY, HIGH);
  ////        delay(1000);
  //        digitalWrite(RELAY, LOW);
  ////        delay(1000);
  //      }
  //      else if (Material == 2) {
  //        target_angle = 180;
  //        delta_angle = abs(target_angle - ref_angle_value);
  //        servo.write(ref_angle_value + delta_angle);
  //        delay(delta_angle * 11);
  //        servo.write(ref_angle_value);
  //        digitalWrite(RELAY, HIGH);
  //        delay(1000);
  //        digitalWrite(RELAY, LOW);
  //        delay(1000);
  //
  //      }
  //      else if (Material == 3) {
  //        //ERROR!
  //      }
  //      break;
  //    default:
  //      break;
  //  }
}
