#include "Joystick.h"

// ***********************************************************************
// **                                                                   **
// ** Joystick USB HID for Teensy or Pro Micro                          **
// ** By DJ Sures (Synthiam.com) (c)2019                                **
// **                                                                   **
// ** Updated: November 16, 2019                                        **
// **                                                                   **
// ** NOTE                                                              **
// ** ~~~~                                                              **
// ** For this to work on Raspberry Pi (Linux), you need to add the     **
// ** the following to the /boot/cmdline.txt...                         **
// **                                                                   **
// ** usbhid.quirks=0x2341:0x8036:0x040    <- for Leonardo              **
// ** usbhid.quirks=0x2341:0x8037:0x040    <- for Micro                 **
// **                                                                   **
// ***********************************************************************

// uncomment to use for serial terminal debugging rather than usb hid device
// this is so you can see the ascii values to verify
//#define SERIAL_DEBUG

/**
  white - common
  grey - right
  blue - left
  green - reverse
  yellow - forward
  orange - twist left
  red - twist right
  brown - pull up
  black - push down
*/

#define J1RIGHT A3
#define J1LEFT A2
#define J1DOWN A1
#define J1UP A0

// button 0
#define J1TWIST_LEFT 15

// button 1
#define J1TWIST_RIGHT 14

// button 2
#define J1PUSH_DOWN 10

// button 3
#define J1PULL_UP 16

#define J2RIGHT 2
#define J2LEFT 3
#define J2DOWN 4
#define J2UP 5
#define J2TWIST_LEFT 6
#define J2TWIST_RIGHT 7
#define J2PUSH_DOWN 9
#define J2PULL_UP 8


Joystick_ _j1 = Joystick_(
                  0x03,
                  JOYSTICK_TYPE_JOYSTICK,
                  4,
                  0,
                  true,
                  true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false
                );

Joystick_ _j2 = Joystick_(
                  0x06,
                  JOYSTICK_TYPE_JOYSTICK,
                  4,
                  0,
                  true,
                  true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false
                );

byte _lastJ1X = 1; // 0 left, 1 center, 2 right
byte _lastJ1Y = 1; // 0 up, 1 center, 2 down
byte _lastJ1TWIST = 1; // 0 twist left, 1 center, 2 twist right
byte _lastJ1PUSH = 1; // 0 push down, 1 center, 2 pull up

byte _lastJ2X = 1; // 0 left, 1 center, 2 right
byte _lastJ2Y = 1; // 0 up, 1 center, 2 down
byte _lastJ2TWIST = 1; // 0 twist left, 1 center, 2 twist right
byte _lastJ2PUSH = 1; // 0 push down, 1 center, 2 pull up

void setup() {

#ifdef SERIAL_DEBUG
  Serial.begin(9600);
#else
  _j1.setXAxisRange(-127, 127);
  _j1.setYAxisRange(-127, 127);
  _j1.begin();

  _j2.setXAxisRange(-127, 127);
  _j2.setYAxisRange(-127, 127);
  _j2.begin();
#endif

  pinMode(J1UP, INPUT_PULLUP);
  pinMode(J1RIGHT, INPUT_PULLUP);
  pinMode(J1DOWN, INPUT_PULLUP);
  pinMode(J1LEFT, INPUT_PULLUP);
  pinMode(J1TWIST_LEFT, INPUT_PULLUP);
  pinMode(J1TWIST_RIGHT, INPUT_PULLUP);
  pinMode(J1PULL_UP, INPUT_PULLUP);
  pinMode(J1PUSH_DOWN, INPUT_PULLUP);

  pinMode(J2UP, INPUT_PULLUP);
  pinMode(J2RIGHT, INPUT_PULLUP);
  pinMode(J2DOWN, INPUT_PULLUP);
  pinMode(J2LEFT, INPUT_PULLUP);
  pinMode(J2TWIST_LEFT, INPUT_PULLUP);
  pinMode(J2TWIST_RIGHT, INPUT_PULLUP);
  pinMode(J2PULL_UP, INPUT_PULLUP);
  pinMode(J2PUSH_DOWN, INPUT_PULLUP);
}

void loop() {

#ifdef SERIAL_DEBUG

  Serial.print(digitalRead(J1UP));
  Serial.print(digitalRead(J1RIGHT));
  Serial.print(digitalRead(J1DOWN));
  Serial.print(digitalRead(J1LEFT));
  Serial.print(digitalRead(J1TWIST_LEFT));
  Serial.print(digitalRead(J1TWIST_RIGHT));
  Serial.print(digitalRead(J1PULL_UP));
  Serial.print(digitalRead(J1PUSH_DOWN));

  Serial.print(" ");

  Serial.print(digitalRead(J2UP));
  Serial.print(digitalRead(J2RIGHT));
  Serial.print(digitalRead(J2DOWN));
  Serial.print(digitalRead(J2LEFT));
  Serial.print(digitalRead(J2TWIST_LEFT));
  Serial.print(digitalRead(J2TWIST_RIGHT));
  Serial.print(digitalRead(J2PULL_UP));
  Serial.print(digitalRead(J2PUSH_DOWN));

  Serial.write("\r\n");

#else

  // ---------------------------------------------------------
  // JOYSTICK 1
  // ---------------------------------------------------------
  {
    byte j1y;

    if (!digitalRead(J1UP))
      j1y = 0;
    else if (!digitalRead(J1DOWN))
      j1y = 2;
    else
      j1y = 1;

    if (j1y != _lastJ1Y) {

      if (j1y == 0)
        _j1.setYAxis(-127);
      else if (j1y == 2)
        _j1.setYAxis(127);
      else
        _j1.setYAxis(0);

      _lastJ1Y = j1y;
    }
  }

  {
    // ---------------------------------------------------------
    byte j1x;

    if (!digitalRead(J1LEFT))
      j1x = 0;
    else if (!digitalRead(J1RIGHT))
      j1x = 2;
    else
      j1x = 1;

    if (j1x != _lastJ1X) {

      if (j1x == 0)
        _j1.setXAxis(-127);
      else if (j1x == 2)
        _j1.setXAxis(127);
      else
        _j1.setXAxis(0);

      _lastJ1X = j1x;
    }
  }

  {
    // ---------------------------------------------------------
    byte j1twist;

    if (!digitalRead(J1TWIST_LEFT))
      j1twist = 0;
    else if (!digitalRead(J1TWIST_RIGHT))
      j1twist = 2;
    else
      j1twist = 1;

    if (j1twist != _lastJ1TWIST) {

      if (j1twist == 0) {

        _j1.setButton(0, true);
        _j1.setButton(1, false);
      }  else if (j1twist == 2) {

        _j1.setButton(0, false);
        _j1.setButton(1, true);
      } else {

        _j1.setButton(0, false);
        _j1.setButton(1, false);
      }

      _lastJ1TWIST = j1twist;
    }
  }

  {
    // ---------------------------------------------------------
    byte j1push;

    if (!digitalRead(J1PUSH_DOWN))
      j1push = 0;
    else if (!digitalRead(J1PULL_UP))
      j1push = 2;
    else
      j1push = 1;

    if (j1push != _lastJ1PUSH) {

      if (j1push == 0) {

        _j1.setButton(2, true);
        _j1.setButton(3, false);
      }  else if (j1push == 2) {

        _j1.setButton(2, false);
        _j1.setButton(3, true);
      } else {

        _j1.setButton(2, false);
        _j1.setButton(3, false);
      }

      _lastJ1PUSH = j1push;
    }
  }

  // ---------------------------------------------------------
  // JOYSTICK 2
  // ---------------------------------------------------------
  {
    byte j2y;

    if (!digitalRead(J2UP))
      j2y = 0;
    else if (!digitalRead(J2DOWN))
      j2y = 2;
    else
      j2y = 1;

    if (j2y != _lastJ2Y) {

      if (j2y == 0)
        _j2.setYAxis(-127);
      else if (j2y == 2)
        _j2.setYAxis(127);
      else
        _j2.setYAxis(0);

      _lastJ2Y = j2y;
    }
  }

  {
    byte j2x;

    if (!digitalRead(J2LEFT))
      j2x = 0;
    else if (!digitalRead(J2RIGHT))
      j2x = 2;
    else
      j2x = 1;

    if (j2x != _lastJ2X) {

      if (j2x == 0)
        _j2.setXAxis(-127);
      else if (j2x == 2)
        _j2.setXAxis(127);
      else
        _j2.setXAxis(0);

      _lastJ2X = j2x;
    }
  }
  
  // ---------------------------------------------------------
  {
    byte j2twist;

    if (!digitalRead(J2TWIST_LEFT))
      j2twist = 0;
    else if (!digitalRead(J2TWIST_RIGHT))
      j2twist = 2;
    else
      j2twist = 1;

    if (j2twist != _lastJ2TWIST) {

      if (j2twist == 0) {

        _j2.setButton(0, true);
        _j2.setButton(1, false);
      }  else if (j2twist == 2) {

        _j2.setButton(0, false);
        _j2.setButton(1, true);
      } else {

        _j2.setButton(0, false);
        _j2.setButton(1, false);
      }

      _lastJ2TWIST = j2twist;
    }
  }

  {
    // ---------------------------------------------------------
    byte j2push;

    if (!digitalRead(J2PUSH_DOWN))
      j2push = 0;
    else if (!digitalRead(J2PULL_UP))
      j2push = 2;
    else
      j2push = 1;

    if (j2push != _lastJ2PUSH) {

      if (j2push == 0) {

        _j2.setButton(2, true);
        _j2.setButton(3, false);
      }  else if (j2push == 2) {

        _j2.setButton(2, false);
        _j2.setButton(3, true);
      } else {

        _j2.setButton(2, false);
        _j2.setButton(3, false);
      }

      _lastJ2PUSH = j2push;
    }
  }
  
#endif

  delay(25);
}
