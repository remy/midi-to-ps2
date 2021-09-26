//#include <Adafruit_TinyUSB.h>
#include <Keyboard.h>

#include "ps2dev.h"
#include "scancodes.h" // inludes the defines for SCANCODE_*

#define midiIn Serial1

PS2dev keyboard(21, 20); //clock, data

char buff[20];

// notes are from 0 to 28 (29 keys) starting at C2 (0 index shifted to 48)
// A good reference is this pdf: https://www.hobbytronics.co.uk/datasheets/9_MIDI_code.pdf
const uint8_t notesToKey[] = {
    SCANCODE_Z, SCANCODE_S, SCANCODE_X, SCANCODE_D, SCANCODE_C, SCANCODE_V, SCANCODE_G, SCANCODE_B, SCANCODE_H, SCANCODE_N, SCANCODE_J, SCANCODE_M,
    SCANCODE_Q, SCANCODE_2, SCANCODE_W, SCANCODE_3, SCANCODE_E, SCANCODE_R, SCANCODE_5, SCANCODE_T, SCANCODE_6, SCANCODE_Y, SCANCODE_7, SCANCODE_U, SCANCODE_I, SCANCODE_9, SCANCODE_O, SCANCODE_0, SCANCODE_P};

const uint8_t notesToChar[] = {
    'z', 's', 'x', 'd', 'c', 'v', 'g', 'b', 'h', 'n', 'j', 'm',
    'q', '2', 'w', '3', 'e', 'r', '5', 't', '6', 'y', '7', 'u', 'i', '9', 'o', '0', 'p'};

uint8_t getCodeForNote(uint8_t note)
{
  return notesToKey[note - 0x30];
}

uint8_t getCharForNote(uint8_t note)
{
  return notesToChar[note - 0x30];
}

void setup()
{
  midiIn.setRX(13);
  midiIn.setTX(12);

  pinMode(LED_BUILTIN, OUTPUT);
  keyboard.init();

  Serial.begin(115200);
  Keyboard.begin();

  midiIn.begin(31250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
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

  if (midiIn.available())
  {
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

    //    sprintf (buff, "(%1x) %2x %2x %2x", nibble, status, data1, data2);
    //    Serial.println(buff);
    int code = getCodeForNote(data1);
    int chr = getCharForNote(data1);

    if (nibble == MIDI_NOTE_OFF)
    {
      // note off
      keyboard.write(0xf0);
      keyboard.write(code);
      Keyboard.release(chr);
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (nibble == MIDI_NOTE_ON)
    {
      digitalWrite(LED_BUILTIN, HIGH);

      if (data2 == 0)
      {
        // velocity off = note off
        keyboard.write(0xf0);
        Keyboard.release(chr);
        digitalWrite(LED_BUILTIN, LOW);
      }

      // note on
      keyboard.write(code);
      Keyboard.press(chr);
    }
    else if (nibble == MIDI_CONTROL)
    {
      // control
      if (data1 == 0x4a)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        // shift down
        keyboard.write(SCANCODE_CONTROLLEFT);
        Keyboard.press(KEY_LEFT_CTRL);
        // send N/M
        int code = data2 == 1 ? SCANCODE_M : SCANCODE_N;
        int chr = data2 == 1 ? 'm' : 'n';
        keyboard.write(code);
        Keyboard.press(chr);
        // send keyup
        keyboard.write(0xf0);
        keyboard.write(code);
        // shift up
        keyboard.write(0xf0);
        keyboard.write(SCANCODE_CONTROLLEFT);
        digitalWrite(LED_BUILTIN, LOW);
        Keyboard.releaseAll();
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
