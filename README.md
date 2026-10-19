# Laboratory Activity 9: ESP32 Wi-Fi Manager (Individual)

**Course:** BCA188 (AY26-27 JFV)  
**Student Name:** Michael Cadiz  
**Assigned Number:** XX  
**Target Microcontroller:** ESP32-S3-N16R8 (16MB Octal Flash, 8MB Octal PSRAM)  
**Prototyping Medium:** Solderless Breadboard  

---

## 1. Objective
* Configure and deploy the `WiFiManager` library on an ESP32-S3 microcontroller.
* Connect the ESP32 to multiple, distinct 2.4 GHz Wi-Fi networks (e.g., local router and smartphone hotspot) dynamically using an on-demand captive web portal (`192.168.4.1`) without re-flashing or changing source code.
* Implement a 180-second configuration portal timeout (`wm.setConfigPortalTimeout(180)`) and a hardware-triggered 3-second button hold to clear stored non-volatile network credentials (`wm.resetSettings()`).

---

## 2. Practice Tasks Results Table

| Task | What to do | What you observe |
| :--- | :--- | :--- |
| **1. Wrong password** | Enter a wrong password on the setup page (`192.168.4.1`). | The ESP32 attempts connection, fails to authenticate with the router, keeps the captive configuration portal active until the 180-second timeout (`wm.setConfigPortalTimeout(180)`) expires, and then restarts cleanly back into Access Point mode rather than hanging indefinitely. |
| **2. Reset settings** | Hold the `BOOT` button (GPIO 0) for 3 seconds while the board is running. | The Serial Monitor logs the button hold duration, detects the 3-second threshold, triggers `wm.resetSettings()`, completely clears stored Wi-Fi credentials from NVS flash memory, and reboots the ESP32 into Access Point mode broadcasting `BCA188-ESP32-XX`. |
| **3. Change the name** | Change the network name in code (e.g., `BCA188-ESP32-99`), upload, and check your phone's Wi-Fi list. | After modifying the SSID string in the sketch and re-uploading, the previous Access Point name disappeared and the new SSID `BCA188-ESP32-99` immediately broadcasted and was detected in the phone's Wi-Fi search list. |
| **4. Different network** | Reset the board, then connect to your phone hotspot (2.4 GHz). | The captive portal opened, the smartphone's 2.4 GHz hotspot SSID was selected, credentials were submitted, and the ESP32 successfully established connection to the mobile hotspot, receiving a new IP address under the hotspot's DHCP pool without modifying any C++ firmware code. |

---

## 3. Explanatory Question

**Question:** Explain why the board connected to a different network without a code change.  
> **Answer:** The board connected to a different network without a code change because the `WiFiManager` library dynamically receives network credentials through an on-demand web captive portal and writes them into the ESP32's Non-Volatile Storage (NVS Flash memory), completely decoupling network configuration parameters from the compiled firmware binary.

---

## 4. Verification Screenshots

### Screenshot 1: Captive Portal Setup Page on Phone
*Accessing the configuration portal at `http://192.168.4.1` on smartphone browser:*  
![Phone Setup Page](assets/setup_portal_phone.png)

---

### Screenshot 2: Serial Monitor Connected State & Reset Trigger
*Arduino IDE Serial Monitor displaying button hold detection, NVS memory erase, reboot, and successful connection:*  
![Serial Monitor Output](assets/serial_monitor.png)

---

## 5. Hardware & Pin Configuration (ESP32-S3-N16R8)

| Signal / Peripheral | Board Pin | Function / Description |
| :--- | :---: | :--- |
| **Onboard Reset / BOOT** | **GPIO 0** | Physical onboard `BOOT` switch used for the 3-second NVS credential reset |
| **External Push Button** | **Pin 4 (IO4)** | Optional breadboard tactile switch tied to GND |
| **Status LED** | **Pin 2 (IO2)** | Optional status LED with $220\,\Omega$ current-limiting resistor |
| **Ground** | **GND** | Common system ground rail |
| **Serial / Power** | **UART Port** | USB Type-C connection to PC at 115200 baud (CH343 USB-to-UART bridge) |
