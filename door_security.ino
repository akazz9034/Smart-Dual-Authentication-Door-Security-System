
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <time.h>

// -------- RFID PINS --------
#define SS_PIN 5
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);

String allowedUID = "01:02:03:04";

// -------- KEYPAD PINS --------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

// -------- SERVO --------
#define SERVO_PIN 21
Servo doorServo;

// -------- ULTRASONIC SENSORS --------
#define TRIG_OUT 4
#define ECHO_OUT 34

#define TRIG_IN 16
#define ECHO_IN 17

// -------- LEDs --------
#define GREEN_LED 2
#define RED_LED 15

// -------- PASSWORD --------
String correctPassword = "2580";
String enteredPassword = "";

// -------- DOOR VARIABLES --------
bool doorOpen = false;
bool personDetected = false;

unsigned long doorOpenTime = 0;
unsigned long lastPersonTime = 0;

const int OPEN_ANGLE = 90;
const int CLOSE_ANGLE = 0;
const int DETECT_DISTANCE = 60;

const unsigned long CLOSE_DELAY = 2000;
const unsigned long SAFETY_TIMEOUT = 15000;

// -------- WIFI TIME --------
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// -------- LED TIMER --------
unsigned long redLedOffTime = 0;
unsigned long greenLedOffTime = 0;

// -------- PRINT DATE AND TIME --------
void printTime() {
  struct tm timeinfo;

  if (getLocalTime(&timeinfo, 1000)) {
    char buffer[30];

    strftime(buffer, sizeof(buffer),
             "%d-%m-%Y %H:%M:%S", &timeinfo);

    Serial.print(buffer);
  } else {
    Serial.print("Time unavailable");
  }
}

// -------- ULTRASONIC FUNCTION --------
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 25000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.034 / 2;
}

// -------- ACCESS GRANTED --------
void grantAccess(String method, String person) {
  if (doorOpen) {
    Serial.println("Door is already open.");
    return;
  }

  Serial.println("\nACCESS GRANTED");

  Serial.print("Person: ");
  Serial.println(person);

  Serial.print("Method: ");
  Serial.println(method);

  Serial.print("Date & Time: ");
  printTime();
  Serial.println();

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  redLedOffTime = 0;
  greenLedOffTime = millis() + 1000;

  doorServo.write(OPEN_ANGLE);

  doorOpen = true;
  personDetected = false;

  doorOpenTime = millis();
  lastPersonTime = 0;
}

// -------- ACCESS DENIED --------
void denyAccess(String reason) {
  Serial.println("\nACCESS DENIED");
  Serial.println(reason);

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  redLedOffTime = millis() + 1500;
}

// -------- RFID CHECK --------
void checkRFID() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String cardUID = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      cardUID += "0";
    }

    cardUID += String(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1) {
      cardUID += ":";
    }
  }

  cardUID.toUpperCase();

  Serial.print("\nScanned RFID UID: ");
  Serial.println(cardUID);

  if (cardUID == allowedUID) {
    grantAccess("RFID CARD", "Registered User");
  } else {
    denyAccess("Unknown RFID Card");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// -------- PASSWORD CHECK --------
void checkKeypad() {
  char key = keypad.getKey();

  if (key) {
    if (key == '*') {
      enteredPassword = "";
      Serial.println("Password cleared");
    }

    else if (key == '#') {
      if (enteredPassword == correctPassword) {
        grantAccess("PASSWORD", "PIN User");
      } else {
        denyAccess("Incorrect Password");
      }

      enteredPassword = "";
    }

    else if (key >= '0' && key <= '9') {
      if (enteredPassword.length() < 8) {
        enteredPassword += key;
        Serial.println("Password digit entered");
      }
    }
  }
}

// -------- AUTOMATIC DOOR CLOSING --------
void automaticDoorControl() {
  if (!doorOpen) {
    return;
  }

  long outsideDistance = getDistance(TRIG_OUT, ECHO_OUT);
  long insideDistance = getDistance(TRIG_IN, ECHO_IN);

  bool outsidePerson = outsideDistance < DETECT_DISTANCE;
  bool insidePerson = insideDistance < DETECT_DISTANCE;

  if (outsidePerson || insidePerson) {
    personDetected = true;
    lastPersonTime = millis();
  }

  // Close 2 seconds after both sensor zones are clear
  if (personDetected &&
      !outsidePerson &&
      !insidePerson &&
      millis() - lastPersonTime >= CLOSE_DELAY) {
    closeDoor();
    return;
  }

  // Safety timeout if nobody enters
  if (!personDetected &&
      millis() - doorOpenTime >= SAFETY_TIMEOUT) {
    closeDoor();
  }
}

// -------- CLOSE DOOR --------
void closeDoor() {
  Serial.println("\nPerson has cleared the doorway.");
  Serial.println("Door closing...");

  doorServo.write(CLOSE_ANGLE);

  doorOpen = false;
  personDetected = false;

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  greenLedOffTime = 0;
  redLedOffTime = 0;

  Serial.println("DOOR LOCKED");
}

// -------- LED CONTROL --------
void updateLEDs() {
  unsigned long now = millis();

  if (greenLedOffTime != 0 &&
      (long)(now - greenLedOffTime) >= 0) {
    digitalWrite(GREEN_LED, LOW);
    greenLedOffTime = 0;
  }

  if (redLedOffTime != 0 &&
      (long)(now - redLedOffTime) >= 0) {
    digitalWrite(RED_LED, LOW);
    redLedOffTime = 0;
  }
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_OUT, OUTPUT);
  pinMode(ECHO_OUT, INPUT);

  pinMode(TRIG_IN, OUTPUT);
  pinMode(ECHO_IN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Servo setup
  doorServo.attach(SERVO_PIN);
  doorServo.write(CLOSE_ANGLE);

  // RFID SPI setup
  SPI.begin(18, 19, 23, SS_PIN);
  rfid.PCD_Init();

  // WiFi and time setup
  Serial.println("\nConnecting to WiFi...");

  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED &&
         attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");

    configTime(19800, 0,
               "pool.ntp.org",
               "time.google.com");
  } else {
    Serial.println("\nWiFi unavailable.");
  }

  Serial.println("\nSMART DOOR SYSTEM READY");
  Serial.println("Scan RFID or enter password.");
  Serial.println("Default password: 2580");
}

// -------- MAIN LOOP --------
void loop() {
  checkRFID();
  checkKeypad();

  automaticDoorControl();
  updateLEDs();

  delay(50);
}
