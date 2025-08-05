#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
//#include <LiquidCrystal.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;
//int rs = 12;
//int en = 11;
//int d4 = 5;
////int d5 = 4;
//int d6 = 3;
//int d7 = 2;

//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // I2C LCD address might need adjustment
int redLED = 4;
int yellowLED = 3;
//int greenLED = 5;
int buzzer = 7;
int servoPin = 6;

// Authorized card UID
byte authorizedUID[] = {0x23, 0x29, 0x67, 0x10};  // Replace with your RFID tag UID

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  //lcd.begin(16, 2);
 
  //lcd.setCursor(0, 0);
  //lcd.print("Scan Your Card");

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(0);  // Lock position

  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  //digitalWrite(greenLED, LOW);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  byte readUID[4];
  for (byte i = 0; i < 4; i++) {
    readUID[i] = rfid.uid.uidByte[i];
  }

  if (checkUID(readUID, authorizedUID)) {
    // Authorized access
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");

    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    tone(buzzer, 1000, 200); // Buzzer short beep

    myServo.write(90);  // Unlock
    delay(3000);        // Keep unlocked for 3 seconds

    myServo.write(0);   // Lock again
    digitalWrite(greenLED, LOW);
  } else {
    // Unauthorized access
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");

    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    tone(buzzer, 500, 1000); // Buzzer long beep
    delay(2000);             // Delay for display
    digitalWrite(redLED, LOW);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool checkUID(byte *currentUID, byte *allowedUID) {
  for (byte i = 0; i < 4; i++) {
    if (currentUID[i] != allowedUID[i]) {
      return false;
    }
  }
  return true;
}
