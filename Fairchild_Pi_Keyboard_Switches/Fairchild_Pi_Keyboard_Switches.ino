#include "Keyboard.h"

// ***********************************************************************
// **                                                                   **
// ** Keyboard USB HID for Teensy or Pro Micro                          **
// ** By DJ Sures (Synthiam.com) (c)2019                                **
// **                                                                   **
// ** Updated: November 15, 2019                                        **
// **                                                                   **
// ***********************************************************************

// uncomment to use for serial terminal debugging rather than usb hid device
// this is so you can see the ascii values to verify
//#define SERIAL_DEBUG

#define SWITCH_RESET 2
#define SWITCH_S1 3
#define SWITCH_S2 4
#define SWITCH_S3 5
#define SWITCH_S4 6

bool _last_reset = false;
bool _last_s1 = false;
bool _last_s2 = false;
bool _last_s3 = false;
bool _last_s4 = false;
bool _last_start = false;
bool _last_select = false;

void setup() {

#ifdef SERIAL_DEBUG
  Serial.begin(9600);
#else
  Keyboard.begin();
#endif

  pinMode(SWITCH_RESET, INPUT_PULLUP);
  pinMode(SWITCH_S1, INPUT_PULLUP);
  pinMode(SWITCH_S2, INPUT_PULLUP);
  pinMode(SWITCH_S3, INPUT_PULLUP);
  pinMode(SWITCH_S4, INPUT_PULLUP);
}

void loop() {

#ifdef SERIAL_DEBUG

  Serial.print(!digitalRead(SWITCH_RESET));
  Serial.print(!digitalRead(SWITCH_S1));
  Serial.print(!digitalRead(SWITCH_S2));
  Serial.print(!digitalRead(SWITCH_S3));
  Serial.print(!digitalRead(SWITCH_S4));
  Serial.print("\r\n");

#else

  bool reset = !digitalRead(SWITCH_RESET);
  bool s1 = !digitalRead(SWITCH_S1);
  bool s2 = !digitalRead(SWITCH_S2);
  bool s3 = !digitalRead(SWITCH_S3);
  bool s4 = !digitalRead(SWITCH_S4);
  bool select = !digitalRead(SWITCH_RESET) && !digitalRead(SWITCH_S1);
  bool start = !digitalRead(SWITCH_RESET) && !digitalRead(SWITCH_S2);

 if (select != _last_select) {

    if (select) 
      Keyboard.press('q');
    else
      Keyboard.release('q');

    _last_select = select;
  }

  if (start != _last_start) {

    if (start) 
      Keyboard.press('w');
    else
      Keyboard.release('w');

    _last_start = start;
  }

  // no need to process anything further because 
  if (reset || select || start)
    return;

  if (reset != _last_reset) {

    if (reset) {

        // TODO: emulator needs support for RESET button
        // exit emulator now by pressing SELECT & START (reset + time + mode)
//      Keyboard.press('q');
//      Keyboard.press('w');
//      delay(250);
//      Keyboard.releaseAll();
    }

    _last_reset = reset;
  }
  
  if (s1 != _last_s1) {

    if (s1)
      Keyboard.press('1');
    else
      Keyboard.release('1');

    _last_s1 = s1;
  }

  if (s2 != _last_s2) {

    if (s2)
      Keyboard.press('2');
    else
      Keyboard.release('2');

    _last_s2 = s2;
  }

  if (s3 != _last_s3) {

    if (s3)
      Keyboard.press('3');
    else
      Keyboard.release('3');

    _last_s3 = s3;
  }

  if (s4 != _last_s4) {

    if (s4)
      Keyboard.press('4');
    else
      Keyboard.release('4');

    _last_s4 = s4;
  }

#endif

  delay(50);
}
