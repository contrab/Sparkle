/**
 * This is a simple Arduino library for controlling LEDs.
 *
 * It contains two classes: LedDef and Sparkle. LedDef defines a single
 * LED (characteristics such as pin[s], color, etc), and Sparkle uses
 * these LED definitions to control the LEDs assigned to it.
 */

#ifndef _SPARKLE_H_
#define _SPARKLE_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/**
 * Color definitions for the Sparkle class.
 */
enum LedColor {
  ANY = 0,
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  BLUE,
  PURPLE,
  WHITE
};

/**
 * LED class. Used by the Sparkle class.
 */
class LedDef {
  private:
  unsigned short pin;
  enum LedColor color;
  bool commonCathode;
  bool ledIsOn;

  public:
  /**
   * Constructor. Provide which pin, color, and whether the LED is switched using
   * common cathode or common anode, so that the logic works correctly.
   */
  LedDef(unsigned short ledPin, enum LedColor ledColor, bool ledCommonCathode) {
    pin = ledPin;
    color = ledColor;
    commonCathode = ledCommonCathode;
  }

  /**
   * Initialize the pins, etc.
   */
  void initPin();

  /**
   * Turn off the LED. Pay attention to whether it is common cathode or anode.
   */
  void turnOff();

  /**
   * Turn on the LED. Pay attention to whether it is common cathode or anode.
   */
  void turnOn();

  /**
   * Get the color.
   */
  enum LedColor getColor();

  /**
   * Return whether the LED is on or not.
   */
  bool isOn();
};


/**
 * The Sparkle class manages LEDs that are assigned to it. Enables more complex behaviors,
 * especially in groups.
 */
class Sparkle {
  private:
  LedDef *leds;
  unsigned short count;

  public:
  /**
   * Constructor.
   * ledList is a list of ledDef LEDs defined for the class to control.
   */
  template <size_t ledCount>
  Sparkle(LedDef (&ledList)[ledCount])  {
    leds = ledList;
    count = sizeof(ledList) / sizeof(LedDef);
  }

  /**
   * Used to set the list of LEDs to manage to digital output.
   */
  void initPins();

  /**
   * Turn off all managed LEDs.
   */
  void allOff();

  /**
   * Turn on all managed LEDs.
   */
  void allOn();

  /**
   * Turn on all LEDs of a particular color.
   * color to turn on.
   */
   void turnOnAllColor(enum LedColor color);

  /**
   * Turn off all LEDs of a particular color.
   * color to turn off.
   */
   void turnOffAllColor(enum LedColor color);
};

#endif
