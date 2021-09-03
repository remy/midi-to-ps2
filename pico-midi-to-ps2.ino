#define PICO // comment this out if building for an arduino board

#ifdef PICO

#include "ps2dev.h"
#define midiIn Serial1

#else

#include <ps2dev.h>
#include <SoftwareSerial.h>
SoftwareSerial midiIn(A0, A1);

#endif

#include "scancodes.h" // inludes the defines for KEY_*

PS2dev keyboard(21, 20); //clock, data

char buff[20];

// notes are from 0 to 28 (29 keys) starting at C2 (0 index shifted to 48)
// A good reference is this pdf: https://www.hobbytronics.co.uk/datasheets/9_MIDI_code.pdf
const uint8_t notesToKey[] = {
    KEY_Z, KEY_S, KEY_X, KEY_D, KEY_C, KEY_V, KEY_G, KEY_B, KEY_H, KEY_N, KEY_J, KEY_M,
    KEY_Q, KEY_2, KEY_W, KEY_3, KEY_E, KEY_R, KEY_5, KEY_T, KEY_6, KEY_Y, KEY_7, KEY_U, KEY_I, KEY_9, KEY_O, KEY_0, KEY_P};

uint8_t getCodeForNote(uint8_t note)
{
  return notesToKey[note - 0x30];
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef PICO
  keyboard.init();
#endif

  midiIn.begin(31250);
  Serial.begin(9600);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
}

uint8_t next()
{
  while (!midiIn.available())
  {
  }
  return (uint8_t)midiIn.read();
}

#define MIDI_NOTE_OFF 0x8
#define MIDI_NOTE_ON 0x9
#define MIDI_CONTROL 0xb
#define MIDI_PITCH 0xe
#define MIDI_SYSEX 0xf

void loop()
{
  uint8_t status = 0;
  uint8_t nibble = 0;
  uint8_t data0 = 0;
  uint8_t data1 = 0;
  uint8_t data2 = 0;

  while (midiIn.available())
  {
    Serial.println("reading");
    data0 = next();

    if (data0 & 0x80)
    {
      status = data0;
      data1 = next();
      data2 = next();
    }
    else
    {
      data1 = data0;
      data2 = next();
    }

    nibble = (status >> 4);

    sprintf(buff, "(%1x) %2x %2x %2x", nibble, status, data1, data2);
    Serial.println(buff);

    if (nibble == MIDI_NOTE_OFF)
    {
      // note off
      int code = getCodeForNote(data1);
      keyboard.write(0xf0);
      keyboard.write(code);
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (nibble == MIDI_NOTE_ON)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      int code = getCodeForNote(data1);

      if (data2 == 0)
      {
        // velocity off = note off
        keyboard.write(0xf0);
        digitalWrite(LED_BUILTIN, LOW);
      }

      // note on
      keyboard.write(code);
    }
    else if (nibble == MIDI_CONTROL)
    {
      // control
      if (data1 == 0x4a)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        // shift down
        keyboard.write(KEY_CONTROLLEFT);
        //        delay(50);
        // send N/M
        int code = data2 == 1 ? KEY_M : KEY_N;
        keyboard.write(code);
        //        delay(100);
        // send keyup
        keyboard.write(0xf0);
        keyboard.write(code);
        // shift up
        //        delay(100);
        keyboard.write(0xf0);
        keyboard.write(KEY_CONTROLLEFT);
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    else if (nibble == MIDI_PITCH)
    {
      // pitch
    }
    else if (nibble == MIDI_SYSEX)
    {
      // system message - we just slurp
      //      while (next() != 0xF7) {}
    }
  }
}
