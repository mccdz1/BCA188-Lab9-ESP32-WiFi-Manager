# Laboratory Activity 9: ESP32 Wi-Fi Manager

**Name:** Michael Cadiz  
**Assigned Number:** XX  

---

## Results Table

| Task | What to do | What you observe |
| :--- | :--- | :--- |
| **1. Wrong password** | Enter a wrong password on the setup page. | The ESP32 failed to authenticate with the router, kept the configuration portal open until the 180-second timeout expired, and then restarted back into AP mode without locking up. |
| **2. Reset settings** | Hold the BOOT button for 3 seconds while the board is running. | The ESP32 detected the 3-second button press, wiped the saved Wi-Fi credentials from NVS flash memory, and restarted back into Access Point mode (`BCA188-ESP32-XX`). |
| **3. Change the name** | Change the network name, upload, and check your phone's Wi-Fi list. | After modifying the SSID in the code and uploading, the old network name disappeared and the new SSID immediately appeared in the phone's Wi-Fi list. |
| **4. Different network** | Reset the board, then connect to your phone hotspot. | The ESP32 opened the captive portal, connected to the phone hotspot after submitting credentials, and acquired a new IP address under the hotspot subnet without any code change. |

---

## Screenshots

### Setup Page on Phone
![Setup Page on Phone](https://raw.githubusercontent.com/mccdz1/BCA188-Lab9-ESP32-WiFi-Manager/07c9029853aa3eb5aff6ca4a8f6ff909868b004b/assets/setup_portal_phone.png)

### Serial Monitor Showing Connected State with IP Address
![Serial Monitor Connected State](https://raw.githubusercontent.com/mccdz1/BCA188-Lab9-ESP32-WiFi-Manager/07c9029853aa3eb5aff6ca4a8f6ff909868b004b/assets/serial_monitor.png)

---

## Explanation

The board connected to a different network without a code change because WiFiManager dynamically saves Wi-Fi credentials into the ESP32's Non-Volatile Storage (NVS Flash memory) via a web captive portal, decoupling network parameters from the compiled firmware.

---

## Source Code

```cpp
#include <WiFi.h>
#include <WiFiManager.h>

#define ONBOARD_BOOT_PIN 0 
#define EXTERNAL_BTN_PIN 4  // Optional
#define STATUS_LED_PIN   2

const char* AP_SSID = "BCA188-ESP32-99"; 

WiFiManager wm;

unsigned long buttonPressStartTime = 0;
bool isButtonPressed = false;

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
      digitalWrite(STATUS_LED_PIN, HIGH);
    }
  }

  delay(20);
}
```
