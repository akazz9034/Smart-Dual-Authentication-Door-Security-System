
# Working Principle

## Smart Dual Authentication Door Security System

### 1. System Initialization
The ESP32 initializes the RFID module, keypad, servo motor, ultrasonic sensors, LEDs, and Wi-Fi connection.

### 2. User Authentication
The system supports two authentication methods:

- RFID Card Authentication
- Password Authentication using a 4x4 Keypad

Either one valid authentication method is sufficient to grant access.

### 3. RFID Authentication
- The user scans an RFID card.
- The MFRC522 module reads the card UID.
- ESP32 compares the UID with the authorized UID.
- If the UID matches, access is granted.

### 4. Password Authentication
- The user enters a password using the keypad.
- ESP32 checks the entered password.
- If the password is correct, access is granted.

### 5. Door Operation
- The servo motor rotates to open the door.
- The green LED indicates successful access.
- The red LED indicates denied access.

### 6. Automatic Door Closing
- Two ultrasonic sensors monitor the entrance.
- The system detects a person using distance measurements.
- After both sensor zones become clear for 2 seconds, the door closes automatically.
- A safety timeout is included when no person is detected.

### 7. Date and Time Tracking
- ESP32 connects to Wi-Fi.
- NTP retrieves the current date and time.
- The system displays the access timestamp through the Serial Monitor.

## Main Features

- Dual authentication using RFID and password.
- Automatic door opening and closing.
- Two-zone person detection.
- LED-based access indication.
- Wi-Fi-based date and time retrieval.

## Applications

- Smart homes
- Office access control
- Laboratory security
- Restricted entry areas

## Conclusion

This project demonstrates an ESP32-based smart access control system that combines RFID, keypad authentication, servo automation, ultrasonic sensing, and network time synchronization.
