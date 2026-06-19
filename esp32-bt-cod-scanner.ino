#include <BluetoothSerial.h>
#include <BTAddress.h>
#include <unordered_map>

#define LED_1 2
#define SCAN_DEV_MAX 2

const uint32_t COD_MASK = 0x00FFFFFF;

std::unordered_map<uint16_t, const char*> service_map = {
  {1, "Limited Discoverable Mode"},
  {2, "(Reserved)"},
  {4, "(Reserved)"},
  {8, "Positioning (Location Identification)"},
  {16, "Networking (LAN, Ad hoc, ...)"},
  {32, "Rendering (Printer, Speaker, ...)"},
  {64, "Capturing (Scanner, Microphone, ...)"},
  {128, "Object Transfer (v-Inbox, v-Folder, ...)"},
  {256, "Audio (Speaker, Microphone, Headset Service, ...)"},
  {512, "Telephony (Cordless Telephony, Modem, Headset Service, ...)"},
  {1024, "Information (Web Server, WAP Server, ...)"}
};

std::unordered_map<uint16_t, const char*> imaging_minor_class_map = {
  {1, "Display"},
  {2, "Camera"},
  {4, "Scanner"},
  {8, "Printer"}
};

// Optional debugging helper.
// Enable this function and the related calls inside generateDetailedCOD()
// to manually inspect the CoD bit fields.
//
// void printBinary(uint32_t value, int bits) {
//   Serial.print("Binary: ");
//   for (int i = bits - 1; i >= 0; --i) {
//     Serial.print((value & (1 << i)) ? '1' : '0');
//   }
//   Serial.println();
// }

struct Device {
  BTAddress address;
  String name;
  int rssi;
  uint32_t class_of_device;
  String detailed_cod;

  Device() {
    rssi = 0;
    class_of_device = 0;
    detailed_cod = "";
  }

  Device(const BTAddress &addr, const String &device_name, int signal_strength, uint32_t cod) {
    address = addr;
    name = device_name;
    rssi = signal_strength;
    class_of_device = cod;
    detailed_cod = "";
    generateDetailedCOD();
  }

  void generateDetailedCOD() {
    detailed_cod = "";

    uint8_t format = class_of_device & 0x03;
    uint8_t minor_device_class = (class_of_device >> 2) & 0x3F;
    uint8_t major_device_class = (class_of_device >> 8) & 0x1F;
    uint16_t major_service_class = (class_of_device >> 13) & 0x7FF;

    // Optional bit-level debugging.
    // Serial.printf("24-bit CoD: %06X\n", class_of_device & COD_MASK);
    // Serial.printf("Format: %02X\n", format);
    // printBinary(format, 2);
    // Serial.printf("Minor Device Class: %02X\n", minor_device_class);
    // printBinary(minor_device_class, 6);
    // Serial.printf("Major Device Class: %02X\n", major_device_class);
    // printBinary(major_device_class, 5);
    // Serial.printf("Major Service Class: %04X\n", major_service_class);
    // printBinary(major_service_class, 11);

    if (format != 0) {
      detailed_cod += "Unrecognized CoD format\n";
      return;
    }

    detailed_cod += "Format: 00\n";
    detailed_cod += "Major Service Class:\n";

    bool service_found = false;
    for (const auto& entry : service_map) {
      if (major_service_class & entry.first) {
        detailed_cod += " - ";
        detailed_cod += entry.second;
        detailed_cod += "\n";
        service_found = true;
      }
    }

    if (!service_found) {
      detailed_cod += " - None\n";
    }

    switch (major_device_class) {
      case 0:
        detailed_cod += "Major Device Class: Miscellaneous\n";
        detailed_cod += "Minor Device Class: Miscellaneous\n";
        break;

      case 1:
        detailed_cod += "Major Device Class: Computer (desktop, notebook, PDA, organizers, ...)\n";
        switch (minor_device_class) {
          case 0:
            detailed_cod += "Minor Device Class: Uncategorized, code for device not assigned\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class: Desktop Workstation\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: Server-Class Computer\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: Laptop\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: Handheld PC/PDA (clamshell)\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: Palm-Sized PC/PDA\n";
            break;
          case 6:
            detailed_cod += "Minor Device Class: Wearable Computer (watch-sized)\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 2:
        detailed_cod += "Major Device Class: Phone (cellular, cordless, payphone, modem, ...)\n";
        switch (minor_device_class) {
          case 0:
            detailed_cod += "Minor Device Class: Uncategorized, code for device not assigned\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class: Cellular\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: Cordless\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: Smartphone\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: Wired Modem or Voice Gateway\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: Common ISDN Access\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 3:
        detailed_cod += "Major Device Class: LAN/Network Access Point\n";
        switch (minor_device_class >> 3) {
          case 0:
            detailed_cod += "Minor Device Class: Fully available\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class: 1% - 17% utilized\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: 17% - 33% utilized\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: 33% - 50% utilized\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: 50% - 67% utilized\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: 67% - 83% utilized\n";
            break;
          case 6:
            detailed_cod += "Minor Device Class: 83% - 99% utilized\n";
            break;
          case 7:
            detailed_cod += "Minor Device Class: No service available\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 4:
        detailed_cod += "Major Device Class: Audio/Video (headset, speaker, stereo, video display, VCR, ...)\n";
        switch (minor_device_class) {
          case 0:
            detailed_cod += "Minor Device Class: Uncategorized, code not assigned\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class: Wearable Headset Device\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: Hands-Free Device\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: (Reserved)\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: Microphone\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: Loudspeaker\n";
            break;
          case 6:
            detailed_cod += "Minor Device Class: Headphones\n";
            break;
          case 7:
            detailed_cod += "Minor Device Class: Portable Audio\n";
            break;
          case 8:
            detailed_cod += "Minor Device Class: Car Audio\n";
            break;
          case 9:
            detailed_cod += "Minor Device Class: Set-Top Box\n";
            break;
          case 10:
            detailed_cod += "Minor Device Class: Hi-Fi Audio Device\n";
            break;
          case 11:
            detailed_cod += "Minor Device Class: VCR\n";
            break;
          case 12:
            detailed_cod += "Minor Device Class: Video Camera\n";
            break;
          case 13:
            detailed_cod += "Minor Device Class: Camcorder\n";
            break;
          case 14:
            detailed_cod += "Minor Device Class: Video Monitor\n";
            break;
          case 15:
            detailed_cod += "Minor Device Class: Video Display and Loudspeaker\n";
            break;
          case 16:
            detailed_cod += "Minor Device Class: Video Conferencing\n";
            break;
          case 17:
            detailed_cod += "Minor Device Class: (Reserved)\n";
            break;
          case 18:
            detailed_cod += "Minor Device Class: Gaming/Toy\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 5:
        detailed_cod += "Major Device Class: Peripheral (mouse, joystick, keyboard, ...)\n";
        switch (minor_device_class >> 4) {
          case 0:
            detailed_cod += "Minor Device Class 1: Not Keyboard / Not Pointing Device\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class 1: Keyboard\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class 1: Pointing Device\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class 1: Combo Keyboard / Pointing Device\n";
            break;
          default:
            detailed_cod += "Minor Device Class 1: Reserved value\n";
            break;
        }
        switch (minor_device_class & 0x0F) {
          case 0:
            detailed_cod += "Minor Device Class 2: Uncategorized Device\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class 2: Joystick\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class 2: Gamepad\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class 2: Remote Control\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class 2: Sensing Device\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class 2: Digitizer Tablet\n";
            break;
          case 6:
            detailed_cod += "Minor Device Class 2: Card Reader (for example, SIM Card Reader)\n";
            break;
          default:
            detailed_cod += "Minor Device Class 2: Reserved value\n";
            break;
        }
        break;

      case 6:
        detailed_cod += "Major Device Class: Imaging (printer, scanner, camera, display, ...)\n";
        detailed_cod += "Minor Device Class:\n";
        for (const auto& entry : imaging_minor_class_map) {
          if ((minor_device_class >> 2) & entry.first) {
            detailed_cod += " - ";
            detailed_cod += entry.second;
            detailed_cod += "\n";
          }
        }
        break;

      case 7:
        detailed_cod += "Major Device Class: Wearable\n";
        switch (minor_device_class) {
          case 1:
            detailed_cod += "Minor Device Class: Wrist Watch\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: Pager\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: Jacket\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: Helmet\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: Glasses\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 8:
        detailed_cod += "Major Device Class: Toy\n";
        switch (minor_device_class) {
          case 1:
            detailed_cod += "Minor Device Class: Robot\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: Vehicle\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: Doll / Action Figure\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: Controller\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: Game\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 9:
        detailed_cod += "Major Device Class: Health\n";
        switch (minor_device_class) {
          case 0:
            detailed_cod += "Minor Device Class: Undefined\n";
            break;
          case 1:
            detailed_cod += "Minor Device Class: Blood Pressure Monitor\n";
            break;
          case 2:
            detailed_cod += "Minor Device Class: Thermometer\n";
            break;
          case 3:
            detailed_cod += "Minor Device Class: Weighing Scale\n";
            break;
          case 4:
            detailed_cod += "Minor Device Class: Glucose Meter\n";
            break;
          case 5:
            detailed_cod += "Minor Device Class: Pulse Oximeter\n";
            break;
          case 6:
            detailed_cod += "Minor Device Class: Heart/Pulse Rate Monitor\n";
            break;
          case 7:
            detailed_cod += "Minor Device Class: Health Data Display\n";
            break;
          default:
            detailed_cod += "Minor Device Class: Reserved value\n";
            break;
        }
        break;

      case 31:
        detailed_cod += "Major Device Class: Uncategorized, specific device code not specified\n";
        break;

      default:
        detailed_cod += "Major Device Class: Reserved value\n";
        break;
    }
  }
};

int light_mode = 1;
int found = 0;

BluetoothSerial SerialBT;
Device found_devices[SCAN_DEV_MAX];

String addressToString(const BTAddress &address) {
  return String(address.toString().c_str());
}

bool isDuplicateAddress(const BTAddress &address) {
  String new_address = addressToString(address);

  for (int i = 0; i < found; i++) {
    if (addressToString(found_devices[i].address) == new_address) {
      return true;
    }
  }

  return false;
}

void printDevice(const Device &device, int index) {
  Serial.print("# ");
  Serial.print(index + 1);
  Serial.print(": ");
  Serial.println(device.address.toString().c_str());

  Serial.print("Name: ");
  if (device.name.length() > 0) {
    Serial.println(device.name);
  } else {
    Serial.println("<unknown>");
  }

  Serial.print("RSSI: ");
  Serial.println(device.rssi);

  Serial.printf("Class of Device: %06X\n", static_cast<unsigned int>(device.class_of_device & COD_MASK));
  Serial.print(device.detailed_cod);
  Serial.println("------------------------------------");
}

void printScanResults() {
  Serial.println("------------------------------------");

  for (int i = 0; i < found; i++) {
    printDevice(found_devices[i], i);
  }

  Serial.println("Scan finished.");
}

void lights() {
  if (light_mode == 1) {
    digitalWrite(LED_1, HIGH);
    delay(250);
    digitalWrite(LED_1, LOW);
    delay(1250);
  } else if (light_mode == 2) {
    digitalWrite(LED_1, HIGH);
    delay(100);
    digitalWrite(LED_1, LOW);
    delay(100);
  }
}

void onAdvertisedDevice(BTAdvertisedDevice* pAdvertisedDevice) {
  if (found >= SCAN_DEV_MAX) {
    return;
  }

  BTAddress address = pAdvertisedDevice->getAddress();

  if (isDuplicateAddress(address)) {
    return;
  }

  found_devices[found].address = address;
  found_devices[found].name = String(pAdvertisedDevice->getName().c_str());
  found_devices[found].rssi = pAdvertisedDevice->getRSSI();
  found_devices[found].class_of_device = pAdvertisedDevice->getCOD() & COD_MASK;
  found_devices[found].generateDetailedCOD();

  found++;
  Serial.println("+1");

  if (found >= SCAN_DEV_MAX) {
    SerialBT.discoverAsyncStop();
    light_mode = 1;
    printScanResults();
  }
}

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32-CoD-Scanner");

  pinMode(LED_1, OUTPUT);

  if (SerialBT.discoverAsync(onAdvertisedDevice)) {
    Serial.println("\nScanning Bluetooth Classic devices.");
    light_mode = 2;
  } else {
    light_mode = 1;
    Serial.println("Scan error.");
  }
}

void loop() {
  lights();
}
