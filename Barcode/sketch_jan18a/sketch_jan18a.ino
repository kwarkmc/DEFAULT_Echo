#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>

#define DB_LENGTH 2

#define ITEM_INIT_IDX 6
#define ITEM_END_IDX 9

char BarcodeString[30];
char* Var = "Hello";
String BarcodeDB[DB_LENGTH] = {"abc","123"};

class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

int flag=0;

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
  
  if(ItemString.equals(BarcodeDB[0])){
    Serial.println("Plastic");
  }
  else if(ItemString.equals(BarcodeDB[1])){
    Serial.println("Paper");
  }
  else{
    Serial.println("None");
  }

  flag=0;
  
  Serial.println(" - Finished");
  
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

void setup() {
  Serial.begin( 115200 );
  Serial.println("Start");

  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Parser);
}

void loop() {
  Usb.Task();
}
