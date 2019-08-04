
#define EN_1 15
#define EN_2 13
#define SIG_1 39
#define SIG_2 36
#define S0 19
#define S1 23
#define S2 18
#define S3 5

#include <Arduino.h>
#include <BLEDevice.h>

uint8_t midiPacket[] = {
    0x80, // header
    0x80, // timestamp, not implemented
    0x00, // status
    0x60, // 0x3c == 60 == middle c
    0x00  // velocity
};

int selectedOctave = 3;

typedef struct
{
  int id;
  int channel;
  int bank;
  boolean value;
  int data;
} buttons;

buttons octButtonState[]{
    {25, 12, 0, false, 0},
    {26, 13, 0, false, 0},
};

buttons octButtonPrevState[]{
    {25, 12, 0, false, 0},
    {26, 13, 0, false, 0},
};

buttons buttonPrevState[]{
    {0, 0, 1, false, 0},
    {1, 1, 1, false, 0},
    {2, 2, 1, false, 0},
    {3, 3, 1, false, 0},
    {4, 4, 1, false, 0},
    {5, 5, 1, false, 0},
    {6, 6, 1, false, 0},
    {7, 7, 1, false, 0},
    {8, 8, 1, false, 0},
    {9, 9, 1, false, 0},
    {10, 10, 1, false, 0},
    {11, 11, 1, false, 0},

    {12, 0, 2, false, 0},
    {13, 1, 2, false, 0},
    {14, 2, 2, false, 0},
    {15, 3, 2, false, 0},
    {16, 4, 2, false, 0},
    {17, 5, 2, false, 0},
    {18, 6, 2, false, 0},
    {19, 7, 2, false, 0},
    {20, 8, 2, false, 0},
    {21, 9, 2, false, 0},
    {22, 10, 2, false, 0},
    {23, 11, 2, false, 0},
    {24, 12, 2, false, 0},
};

buttons buttonState[]{
    {0, 0, 1, false, 0},
    {1, 1, 1, false, 0},
    {2, 2, 1, false, 0},
    {3, 3, 1, false, 0},
    {4, 4, 1, false, 0},
    {5, 5, 1, false, 0},
    {6, 6, 1, false, 0},
    {7, 7, 1, false, 0},
    {8, 8, 1, false, 0},
    {9, 9, 1, false, 0},
    {10, 10, 1, false, 0},
    {11, 11, 1, false, 0},

    {12, 0, 2, false, 0},
    {13, 1, 2, false, 0},
    {14, 2, 2, false, 0},
    {15, 3, 2, false, 0},
    {16, 4, 2, false, 0},
    {17, 5, 2, false, 0},
    {18, 6, 2, false, 0},
    {19, 7, 2, false, 0},
    {20, 8, 2, false, 0},
    {21, 9, 2, false, 0},
    {22, 10, 2, false, 0},
    {23, 11, 2, false, 0},
    {24, 12, 2, false, 0},
};

int readMux(int channel, int bank_en, int bank_sig)
{
  digitalWrite(bank_en, LOW);
  int controlPin[] = {S0, S1, S2, S3};

  int muxChannel[16][4] = {
      {0, 0, 0, 0},
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {1, 1, 0, 0},
      {0, 0, 1, 0},
      {1, 0, 1, 0},
      {0, 1, 1, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 1},
      {1, 0, 0, 1},
      {0, 1, 0, 1},
      {1, 1, 0, 1},
      {0, 0, 1, 1},
      {1, 0, 1, 1},
      {0, 1, 1, 1},
      {1, 1, 1, 1}};

  //loop through the 4 sig
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  int val = analogRead(bank_sig);
  // int val = digitalRead(bank_sig);
  digitalWrite(bank_en, LOW);
  return val;
}

void readButtonBank(int bank_en, int bank_sig)
{
  for (int i = 0; i < 16; i++)
  {
    Serial.print(readMux(i, bank_en, bank_sig));
    Serial.print(". ");
  }
}

void processButtons(BLECharacteristic *pCharacteristic)
{
  for (int i = 0; i < sizeof(buttonState) / sizeof(buttons); ++i)
  {
    byte bank_en;
    byte bank_sig;
    int value;

    if (buttonState[i].bank == 1)
    {
      bank_en = EN_1;
      bank_sig = SIG_1;
    }

    if (buttonState[i].bank == 2)
    {
      bank_en = EN_2;
      bank_sig = SIG_2;
    }

    //TODO: normalize to boolean;
    value = readMux(buttonState[i].channel, bank_en, bank_sig);
    Serial.print(value);
    Serial.print(' ');

    buttonState[i].value = (value > 4094);

    if (buttonState[i].value != buttonPrevState[i].value)
    {

      int send = (buttonState[i].value) ? 0x90 : 0x80;
      midiPacket[2] = send;                                           // note up, channel 0
      midiPacket[3] = 24 + (12 * (selectedOctave+buttonState[i].bank)) + buttonState[i].id; // note up, channel 0
      midiPacket[4] = 127;                                            // velocity
      pCharacteristic->setValue(midiPacket, 5);                       // packet, length in bytes)
      pCharacteristic->notify();
      Serial.print(send);
      Serial.print(' ');
      Serial.print(12 + buttonState[i].id);
    }
  }
  Serial.println(' ');

  byte bank_en = EN_1;
  byte bank_sig = SIG_1;

  int incValue = readMux(octButtonState[1].channel, bank_en, bank_sig);
  octButtonState[1].value = (incValue > 4094);
  
  if (octButtonState[1].value != octButtonPrevState[1].value && selectedOctave < 8)
  {
    selectedOctave += 1;
  }

  int decValue = readMux(octButtonState[0].channel, bank_en, bank_sig);
  octButtonState[0].value = (decValue > 4094);
  
  if (octButtonState[0].value != octButtonPrevState[0].value && selectedOctave > -1)
  {
    selectedOctave -= 1;
  }

  memcpy(&buttonPrevState, &buttonState, sizeof(buttonState));
  memcpy(&octButtonPrevState, &octButtonState, sizeof(octButtonState));
}

int buttonIdToData(int ButtonId, int selectedOctave) {
  return 24 + (12 * selectedOctave) + ButtonId;
}