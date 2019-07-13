#include <Arduino.h>
#include <BLEUtils.h>

class MyCharacteristicCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pChar)
  {
    uint8_t value[5] = {0};
    memcpy(value, pChar->getValue().c_str(), 5);
    ESP_LOGW(LOG_TAG, "command: %d, note: %d, data: %d, %d, %d", value[2], value[3], value[0], value[1], value[4]);
  }
};