# ESP32 Bluetooth Class of Device Scanner

ESP32-based Bluetooth Classic scanner that discovers nearby Bluetooth devices and prints their address, RSSI, device name, and a detailed interpretation of their Bluetooth Class of Device (CoD).

The tool decodes the 24-bit Class of Device field into:

- Format type
- Major service classes
- Major device class
- Minor device class

This makes it easier to identify the category and advertised capabilities of nearby Bluetooth Classic devices.

## Features

- Scans nearby Bluetooth Classic devices using an ESP32.
- Prints device address, device name, RSSI, and Class of Device value.
- Decodes the CoD field into human-readable service and device classes.
- Supports major service class detection, such as audio, networking, telephony, object transfer, rendering, capturing, positioning, and information services.
- Supports multiple major device categories, including phones, computers, wearables, peripherals, audio/video devices, imaging devices, toys, and health devices.
- Avoids storing duplicate devices with the same Bluetooth address.
- Prevents writing outside the configured device buffer.
- Includes optional binary CoD debugging output for manual verification.
- Uses the onboard LED to indicate scanning status.

## Hardware Requirements

- ESP32 development board
- USB cable for flashing and serial output
- Optional: onboard LED connected to GPIO 2

## Software Requirements

- Arduino IDE or PlatformIO
- Arduino core for ESP32
- ESP32 Bluetooth Classic support
- `BluetoothSerial` library
- `BTAddress` support from the ESP32 Bluetooth stack

## Configuration

The maximum number of unique devices to scan is controlled by:

```cpp
#define SCAN_DEV_MAX 2
```

Increase this value to scan and store more devices:

```cpp
#define SCAN_DEV_MAX 10
```

The LED pin is configured with:

```cpp
#define LED_1 2
```

Change it if your ESP32 board uses a different LED pin.

## How It Works

The scanner starts an asynchronous Bluetooth Classic discovery process using the ESP32 Bluetooth stack.

For each discovered device, it stores:

- Bluetooth address
- Device name
- RSSI
- 24-bit Class of Device value

The CoD value is decoded by separating its bit fields:

```text
Bits 0-1    -> Format type
Bits 2-7    -> Minor device class
Bits 8-12   -> Major device class
Bits 13-23  -> Major service class
```

Only Format `00` is decoded in detail.

## Usage

1. Open the project in Arduino IDE or PlatformIO.
2. Select your ESP32 board.
3. Upload the sketch.
4. Open the Serial Monitor.
5. Set the baud rate to `9600`.
6. Wait until the configured number of unique Bluetooth devices is discovered.

## Example Output

```text
Scanning Bluetooth Classic devices.
+1
+1
------------------------------------
# 1: XX:XX:XX:XX:XX:XX
Name: HUAWEI WATCH GT 2e-7F4
RSSI: -77
Class of Device: 280704
Format: 00
Major Service Class:
 - Audio (Speaker, Microphone, Headset Service, ...)
 - Capturing (Scanner, Microphone, ...)
Major Device Class: Wearable
Minor Device Class: Wrist Watch
------------------------------------
# 2: XX:XX:XX:XX:XX:XX
Name: S89Pro
RSSI: -31
Class of Device: 5A020C
Format: 00
Major Service Class:
 - Telephony (Cordless Telephony, Modem, Headset Service, ...)
 - Object Transfer (v-Inbox, v-Folder, ...)
 - Capturing (Scanner, Microphone, ...)
 - Networking (LAN, Ad hoc, ...)
Major Device Class: Phone (cellular, cordless, payphone, modem, ...)
Minor Device Class: Smartphone
------------------------------------
Scan finished.
```

## Optional Debugging

The sketch includes a commented helper function for printing CoD values in binary:

```cpp
// void printBinary(uint32_t value, int bits)
```

Enable this helper and the related debug calls inside `generateDetailedCOD()` if you want to manually inspect the CoD bit fields.

## Reference

The Class of Device decoding is based on the Bluetooth CoD Format `00` definition.

Reference document:

```text
https://www.ampedrftech.com/datasheets/cod_definition.pdf
```

Online CoD checker:

```text
https://www.ampedrftech.com/cod.htm
```

## Limitations

- This project focuses on Bluetooth Classic discovery, not BLE scanning.
- Only Class of Device Format `00` is decoded in detail.
- Some devices may hide their name or report incomplete CoD information.
- RSSI values are approximate and depend on distance, antenna orientation, interference, and device behavior.
- The scanner stops after finding the configured number of unique devices.

## Legal Notice

Use this tool only in environments where Bluetooth scanning is allowed. This project is intended for educational, research, and personal experimentation purposes.

## License

This project is licensed under the PolyForm Noncommercial License 1.0.0.

You may use, copy, modify, and share this software for personal, educational, and other non-commercial purposes.

Commercial use is not permitted without a separate commercial license. For commercial licensing, contact the author through the GitHub profile associated with this repository.

Third-party dependencies are distributed under their respective licenses.
