/**
 * This is a simple Arduino library for controlling LEDs.
 *
 * It contains two classes: LedDef and Sparkle. LedDef defines a single
 * LED (characteristics such as pin[s], color, etc), and Sparkle uses
 * these LED definitions to control the LEDs assigned to it.
 */

#include "Sparkle.h"

// LedDef Class //////////////////////////////////////////////////////////////////

  /**
   * Initialize the pins, etc.
   */
  void LedDef::initPin() {
    pinMode(pin, OUTPUT);
    turnOff();
  }

  /**
   * Turn off the LED. Pay attention to whether it is common cathode or anode.
   */
  void LedDef::turnOff() {
    digitalWrite(pin, (commonCathode?LOW:HIGH));
    ledIsOn = false;
  }


  /**
   * Turn on the LED. Pay attention to whether it is common cathode or anode.
   */
  void LedDef::turnOn() {
    digitalWrite(pin, (commonCathode?HIGH:LOW));
    ledIsOn = true;
  }

  /**
   * Get the color.
   */
  enum LedColor LedDef::getColor() {
    return color;
  }

  /**
   * Return whether the LED is on or not.
   */
  bool LedDef::isOn() {
    return ledIsOn;
  }

// Sparkle Class /////////////////////////////////////////////////////////////////

/**
 * Used to set the list of LEDs to manage to digital output.
 */
void Sparkle::initPins() {
  for (unsigned short i=0; i<count; i++) {
    leds[i].initPin();
  }
}

/**
 * Turn off all managed LEDs.
 */
void Sparkle::allOff() {
  for (unsigned short i=0; i<count; i++) {
    leds[i].turnOff();
  }
}

/**
 * Turn on all managed LEDs.
 */
void Sparkle::allOn() {
  for (unsigned short i=0; i<count; i++) {
    leds[i].turnOn();
  }
}

/**
 * Turn on all LEDs of a particular color.
 * color to turn on.
 */
void Sparkle::turnOnAllColor(enum LedColor color) {
  for (unsigned short i=0; i<count; i++) {
    if (leds[i].getColor() == color) {
      leds[i].turnOn();
    }
  }
}

/**
 * Turn off all LEDs of a particular color.
 * color to turn off.
 */
void Sparkle::turnOffAllColor(enum LedColor color) {
  for (unsigned short i=0; i<count; i++) {
    if (leds[i].getColor() == color) {
      leds[i].turnOff();
    }
  }
}
