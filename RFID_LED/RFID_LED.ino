#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_PIN 13  // Single LED for RFID detection

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

// To store the UID of the last detected card
byte lastUID[4] = { 0x00, 0x00, 0x00, 0x00 };  // Default value for no card detected
bool ledState = false;  // LED state (ON/OFF)

void setup() {
  Serial.begin(9600);
  SPI.begin();  // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  pinMode(LED_PIN, OUTPUT);  // Initialize LED pin as output
  digitalWrite(LED_PIN, LOW);  // Make sure the LED is off initially

  Serial.println(F("This code will toggle the LED based on the RFID tag detected."));
}

void loop() {
  // Check if a new card is present
  if (rfid.PICC_IsNewCardPresent()) {
    // Read the card
    if (rfid.PICC_ReadCardSerial()) {
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // Check if the PICC is of MIFARE Classic type
      if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
          piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
          piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
      }

      // Compare UID and toggle the LED based on the detected RFID tag
      if (compareUID(rfid.uid.uidByte, lastUID)) {
        Serial.println(F("Same tag detected again, turning off the LED."));
        // Turn off the LED
        digitalWrite(LED_PIN, LOW);
        ledState = false;  // Set LED state to OFF
      } 
      else {
        Serial.println(F("New tag detected, turning on the LED."));
        // Turn on the LED
        digitalWrite(LED_PIN, HIGH);
        ledState = true;  // Set LED state to ON
      }

      // Store the current UID as the last detected UID
      for (byte i = 0; i < 4; i++) {
        lastUID[i] = rfid.uid.uidByte[i];
      }

      // Halt PICC and stop encryption
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  } 
  else {
    // If no card is detected, keep the LED in its current state
    // No need to do anything
  }
}

// Function to compare the UID of the detected card with the last detected UID
bool compareUID(byte* detectedUID, byte* lastUID) {
  for (byte i = 0; i < 4; i++) {
    if (detectedUID[i] != lastUID[i]) {
      return false;  // UID doesn't match
    }
  }
  return true;  // UID matches
}
