
# Circuit Connections

## 1. ESP32 to RFID RC522

| RFID Pin | ESP32 Pin |
|---|---|
| SDA (SS) | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| RST | GPIO 22 |
| 3.3V | 3.3V |
| GND | GND |

## 2. ESP32 to 4x4 Keypad

### Row Connections
| Keypad Pin | ESP32 Pin |
|---|---|
| R1 | GPIO 13 |
| R2 | GPIO 12 |
| R3 | GPIO 14 |
| R4 | GPIO 27 |

### Column Connections
| Keypad Pin | ESP32 Pin |
|---|---|
| C1 | GPIO 26 |
| C2 | GPIO 25 |
| C3 | GPIO 33 |
| C4 | GPIO 32 |

## 3. ESP32 to Servo Motor

| Servo Wire | ESP32 Connection |
|---|---|
| Signal | GPIO 21 |
| VCC | External 5V supply |
| GND | Common GND |

## 4. ESP32 to Ultrasonic Sensors

### Outside Sensor
| Sensor Pin | ESP32 Pin |
|---|---|
| TRIG | GPIO 4 |
| ECHO | GPIO 34 |
| VCC | 5V |
| GND | GND |

### Inside Sensor
| Sensor Pin | ESP32 Pin |
|---|---|
| TRIG | GPIO 16 |
| ECHO | GPIO 17 |
| VCC | 5V |
| GND | GND |

## 5. ESP32 to LEDs

| LED | ESP32 Pin |
|---|---|
| Green LED | GPIO 2 |
| Red LED | GPIO 15 |

Use suitable current-limiting resistors for both LEDs.

## Important Safety Notes

- RFID RC522 must use 3.3V.
- ESP32 GPIO pins are not 5V tolerant.
- Use a voltage divider on each HC-SR04 ECHO output before connecting it to the ESP32.
- Use a suitable external power supply for the servo.
- Connect the external power supply GND and ESP32 GND together.

## Working Principle

1. The user scans an authorized RFID card OR enters the correct password.
2. ESP32 verifies the authentication.
3. If authentication is successful, the servo opens the door.
4. Ultrasonic sensors detect whether a person is passing through.
5. Once both sensor zones are clear for 2 seconds, the door closes automatically.
6. The system retrieves the date and time using Wi-Fi and NTP.

## Controller

ESP32 Development Board

## Communication

- SPI
- Wi-Fi
- GPIO
- NTP
