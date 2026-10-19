/*
  Laboratory Activity 9: ESP32 Wi-Fi Manager (Individual)
  Course: BCA188 (AY26-27 JFV)
  Student: Michael Cadiz
  Microcontroller Target: ESP32-S3-N16R8 (16MB Octal Flash, 8MB Octal PSRAM)
*/

#include <WiFi.h>
#include <WiFiManager.h> // WiFiManager by tzapu

// ---------------- Pin Definitions ----------------
#define ONBOARD_BOOT_PIN 0  // Built-in BOOT button (wired internally to GPIO 0)
#define EXTERNAL_BTN_PIN 4  // Optional Breadboard button (wired to Pin 4 / IO4)
#define STATUS_LED_PIN   2  // Optional Status LED (wired to Pin 2 / IO2)

// ---------------- Network Name -------------------
// Network SSID prefix followed by student identifier
const char* AP_SSID = "BCA188-ESP32-XX"; 

WiFiManager wm;

// Variables for 3-second button hold detection
unsigned long buttonPressStartTime = 0;
bool isButtonPressed = false;

// Helper to check if either the onboard BOOT or breadboard button is pressed
bool isButtonPressedNow() {
  return (digitalRead(ONBOARD_BOOT_PIN) == LOW || digitalRead(EXTERNAL_BTN_PIN) == LOW);
}

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println("\n==============================================");
  Serial.println("     ESP32-S3 Wi-Fi Manager (Lab 9)           ");
  Serial.println("==============================================");

  // Configure Button Pins (Internal Pull-Up)
  pinMode(ONBOARD_BOOT_PIN, INPUT_PULLUP);
  pinMode(EXTERNAL_BTN_PIN, INPUT_PULLUP);

  // Configure Status LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // Lab Requirement: 180-second portal timeout so a wrong password never gets stuck
  wm.setConfigPortalTimeout(180);

  Serial.printf("[INFO] Starting WiFiManager AP: '%s'\n", AP_SSID);
  Serial.println("[INFO] Connect your phone to this network and open 192.168.4.1");

  digitalWrite(STATUS_LED_PIN, HIGH);

  // autoConnect attempts to connect to saved Wi-Fi.
  // If no credentials exist or connection fails, it launches AP mode.
  bool isConnected = wm.autoConnect(AP_SSID);

  if (!isConnected) {
    Serial.println("[ERROR] Failed to connect or 180-second portal timeout reached!");
    Serial.println("[INFO] Restarting board in 2 seconds...");
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(2000);
    ESP.restart();
  }

  // Successfully connected to Wi-Fi
  digitalWrite(STATUS_LED_PIN, HIGH);
  Serial.println("\n----------------------------------------------");
  Serial.println("[SUCCESS] Connected to Wi-Fi!");
  Serial.print("[INFO] Connected SSID : ");
  Serial.println(WiFi.SSID());
  Serial.print("[INFO] Assigned IP    : ");
  Serial.println(WiFi.localIP());
  Serial.print("[INFO] Signal RSSI    : ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println("----------------------------------------------");
  Serial.println("Hold the BOOT button for 3 seconds while running to reset Wi-Fi.");
  Serial.println("==============================================\n");
}

void loop() {
  // Practice Task 2: Hold button for 3 seconds while running to reset Wi-Fi
  if (isButtonPressedNow()) {
    if (!isButtonPressed) {
      isButtonPressed = true;
      buttonPressStartTime = millis();
      Serial.println("[BTN] Button pressed. Hold for 3 seconds to reset Wi-Fi...");
    } else {
      // Blink LED rapidly while holding
      digitalWrite(STATUS_LED_PIN, (millis() / 150) % 2);

      // Check if held for 3 seconds (3000 ms)
      if (millis() - buttonPressStartTime >= 3000) {
        Serial.println("\n[RESET] 3 seconds reached!");
        Serial.println("[RESET] Wiping saved Wi-Fi credentials from NVS flash...");
        
        wm.resetSettings(); // Erases saved credentials from NVS memory
        
        Serial.println("[RESET] Done! Restarting ESP32-S3 now...");
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(1000);
        ESP.restart();
      }
    }
  } else {
    if (isButtonPressed) {
      isButtonPressed = false;
      Serial.println("[BTN] Released early. Reset cancelled.");
      digitalWrite(STATUS_LED_PIN, HIGH); // Restore solid ON
    }
  }

  delay(20);
}
