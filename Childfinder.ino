#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>

// ---------------- Pins ----------------
#define SOS_BUTTON 7
#define SPEAKER_PIN 6

SoftwareSerial sim800(2, 3);   // RX, TX
SoftwareSerial gpsSerial(4, 5); // RX, TX

LiquidCrystal_I2C lcd(0x27, 16, 2);
TinyGPSPlus gps;

// ---------------- Numbers ----------------
String parentNumber = "9336565250";
String teacherNumber = "9773633704";

// ---------------- Functions ----------------
void sendSMS(String msg) {
  sim800.println("AT+CMGF=1");
  delay(500);

  sim800.print("AT+CMGS=\"");
  sim800.print(parentNumber);
  sim800.println("\"");
  delay(200);
  sim800.print(msg);
  sim800.write(26);
  delay(800);

  sim800.print("AT+CMGS=\"");
  sim800.print(teacherNumber);
  sim800.println("\"");
  delay(200);
  sim800.print(msg);
  sim800.write(26);
  delay(800);
}

void beep() {
  tone(SPEAKER_PIN, 2000, 200);
}

void sendLocation() {
  if (gps.location.isValid()) {
    String link = "https://maps.google.com/?q=" +
                  String(gps.location.lat(), 6) + "," +
                  String(gps.location.lng(), 6);

    sendSMS(link);

    lcd.clear();
    lcd.print("Location Sent");
    beep();
  }
}

// ---------------- Setup ----------------
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Starting...");

  pinMode(SOS_BUTTON, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);

  sim800.begin(9600);
  gpsSerial.begin(9600);
  delay(2000);

  lcd.clear();
  lcd.print("Ready");
}

// ---------------- Loop ----------------
unsigned long sosPressTime = 0;

void loop() {
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  // SOS LOGIC
  if (digitalRead(SOS_BUTTON) == LOW) {
    if (sosPressTime == 0) sosPressTime = millis();

    if (millis() - sosPressTime > 5000) {
      sendLocation();
      lcd.clear();
      lcd.print("SOS Sent");
      beep();
      delay(2000);
    }
  } else {
    sosPressTime = 0;
  }

  // SMS commands
  if (sim800.available()) {
    String cmd = sim800.readString();
    if (cmd.indexOf("WHERE") > -1 || cmd.indexOf("where") > -1) {
      sendLocation();
    }
  }
}
