#include <BLEServer.h>

class MyCallbacks : public BLEServerCallbacks
{
  private: bool deviceConnected = false;
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};
