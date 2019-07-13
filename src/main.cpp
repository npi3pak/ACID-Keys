#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "MyCallbacks.cpp"
#include "MyCharacteristicCallback.cpp"

#define SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

BLECharacteristic *pCharacteristic;

uint8_t midiPacket[] = {
    0x80, // header
    0x80, // timestamp, not implemented
    0x00, // status
    0x60, // 0x3c == 60 == middle c
    0x00  // velocity
};

void setup()
{
  Serial.begin(115200);
  ESP_LOGD(LOG_TAG, "Starting BLE work!");

  BLEDevice::init("ESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,
                                                   BLECharacteristic::PROPERTY_READ |
                                                       BLECharacteristic::PROPERTY_NOTIFY |
                                                       BLECharacteristic::PROPERTY_WRITE_NR);

  pCharacteristic->setCallbacks(new MyCharacteristicCallback());

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);

  ESP_LOGD(LOG_TAG, "Advertising started!");

  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
}

boolean prevIsPressed = false;
void loop()
{
  midiPacket[2] = 0x80;                     // note up, channel 0
  midiPacket[4] = 0;                        // velocity
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
  pCharacteristic->notify();
  delay(50);
  midiPacket[2] = 0x90;                     // note down, channel 0
  midiPacket[4] = 127;                      // velocity
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
  pCharacteristic->notify();
  delay(50);
}
