#include <Arduino.h>
#include <BLEServer.h>

class MyCallbacks : public BLEServerCallbacks
{
  private: bool deviceConnected = false;
  void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param)
  {
    pServer->updateConnParams(param->connect.remote_bda, 0x01, 0x90, 0, 800);
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};
  