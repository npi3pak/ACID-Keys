
#define EN_1 15
#define EN_2 13
#define SIG_1 12
#define SIG_2 14
#define S0 19
#define S1 23
#define S2 18
#define S3 5

#include <Arduino.h>

typedef struct
{
  int id;
  int channel;
  int bank;
  boolean value;
} buttons;

buttons buttonPrevState[]{
    {0, 0, 1, false},
    {1, 1, 1, false},
    {2, 2, 1, false},
    {3, 3, 1, false},
    {4, 4, 1, false},
    {5, 5, 1, false},
    {6, 6, 1, false},
    {7, 7, 1, false},
    {8, 8, 1, false},
    {9, 9, 1, false},
    {10, 10, 1, false},
    {11, 11, 1, false},
    {12, 0, 2, false},
    {13, 1, 2, false},
    {14, 2, 2, false},
    {15, 3, 2, false},
    {16, 4, 2, false},
    {17, 5, 2, false},
    {18, 6, 2, false},
    {19, 7, 2, false},
    {20, 8, 2, false},
    {21, 9, 2, false},
    {22, 10, 2, false},
    {23, 11, 2, false},
    {24, 12, 2, false},
};

buttons buttonState[]{
    {0, 0, 1, false},
    {1, 1, 1, false},
    {2, 2, 1, false},
    {3, 3, 1, false},
    {4, 4, 1, false},
    {5, 5, 1, false},
    {6, 6, 1, false},
    {7, 7, 1, false},
    {8, 8, 1, false},
    {9, 9, 1, false},
    {10, 10, 1, false},
    {11, 11, 1, false},
    {12, 0, 2, false},
    {13, 1, 2, false},
    {14, 2, 2, false},
    {15, 3, 2, false},
    {16, 4, 2, false},
    {17, 5, 2, false},
    {18, 6, 2, false},
    {19, 7, 2, false},
    {20, 8, 2, false},
    {21, 9, 2, false},
    {22, 10, 2, false},
    {23, 11, 2, false},
    {24, 12, 2, false},
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
  // Serial.println();
}

void processButtons()
{
  for (int i = 0; i < sizeof(buttonState) / sizeof(buttons); ++i)
  {
    byte bank_en;
    byte bank_sig;
    byte value;

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
    if (value > 4500)
    {
      buttonState[i].value = true;
    }
    else
    {
      buttonState[i].value = false;
    }

    if (buttonState[i].value != buttonPrevState[i].value)
    {
      //send message
      Serial.println(buttonState[i].value);
      Serial.println(buttonState[i].channel);
    }
  }
  memcpy(&buttonPrevState, &buttonState, sizeof(buttonState));
}