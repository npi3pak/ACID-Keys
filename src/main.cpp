#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "MyCallbacks.cpp"
#include "MyCharacteristicCallback.cpp"

#include "esp32-hal-adc.h"

#define EN_1 15 //may be broken
#define EN_2 13
#define SIG_1 39
#define SIG_2 36
#define S0 19
#define S1 23
#define S2 18
#define S3 5
#define SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

//TODO: Change SIG 12 to SIG SENSOR_VP	GPIO39
//TODO: Change SIG 14 to SIG SENSOR_VТ	GPIO36

extern void readButtonBank(int bank_en, int bank_sig);
extern void processButtons(BLECharacteristic *pCharacteristic);

BLECharacteristic *pCharacteristic;

// uint8_t midiPacket[] = {
//     0x80, // header
//     0x80, // timestamp, not implemented
//     0x00, // status
//     0x60, // 0x3c == 60 == middle c
//     0x00  // velocity
// };

void setup()
{
  Serial.begin(115200);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  pinMode(EN_1, OUTPUT);
  digitalWrite(EN_1, HIGH);

  pinMode(EN_2, OUTPUT);
  digitalWrite(EN_2, HIGH);

  pinMode(SIG_1, INPUT);
  pinMode(SIG_2, INPUT);

  // adcAttachPin(SIG_1);
  // adcAttachPin(SIG_2);
  // analogReadResolution(11);
  // analogSetAttenuation(ADC_6db);

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
  Serial.print('ok!');
}

boolean prevIsPressed = false;
void loop()
{
  // midiPacket[2] = 0x80;                     // note up, channel 0
  // midiPacket[4] = 0;                        // velocity
  // pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
  // pCharacteristic->notify();
  // delay(50);
  // midiPacket[2] = 0x90;                     // note down, channel 0
  // midiPacket[4] = 127;                      // velocity
  // pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
  // pCharacteristic->notify();
  // delay(50);

  // Serial.print('..');
  // readButtonBank(EN_1, SIG_1);
  // Serial.print('..');
  // readButtonBank(EN_2, SIG_2);
  // Serial.println();
  // delay(100);
  processButtons(pCharacteristic);
}