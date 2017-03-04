/**
 * This is a simple Arduino library for controlling LEDs.
 *
 * It contains two classes: LedDef and Sparkle. LedDef defines a single
 * LED (characteristics such as pin[s], color, etc), and Sparkle uses
 * these LED definitions to control the LEDs assigned to it.
 */

/**
 * You can save memory on your Arduino by defining only the Sparkle/LED features
 * that your code/project is using. Comment out a definition below to disable.
 * If you only want to control LEDs individually (without the Sparkle class), you
 * can comment out the __USE_SPARKLE__ line.
 */
#define __LED_TIMED_ENABLED__ 1
#define __LED_BLINK_ENABLED__ 1
#define __LED_BLINK_RANDOM_ENABLED__ 1
#define __LED_TIMED_ENABLED__ 1
#define __LED_FADE_ENABLED__ 1
#define __USE_SPARKLE__ 1

#ifndef _SPARKLE_H_
#define _SPARKLE_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/**
 * Color definitions for LEDs.
 */
enum LedColor {
  ANY = 0,
  IR,        // Infrared, for whatever it's worth
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  AQUA,
  BLUE,
  PURPLE,
  UV,        // Ultraviolet, for whatever it's worth
  WHITE
};

/**
 * Display modes for LEDs.
 */
enum DisplayMode {
  DISABLED = 0,
#ifdef __LED_TIMED_ENABLED__
  TIMED,
#endif
#ifdef __LED_BLINK_ENABLED__
  BLINK,
#endif
#ifdef __LED_BLINK_RANDOM_ENABLED__
  BLINK_RANDOM,
#endif
#ifdef __LED_FADE_ENABLED__
  FADE,       // For PWM pins only
#endif
  MANUAL
};

/**
 * Interface for all LED types. Cover the basics; later on, add more advanced
 * LED controls that can handle 2- and 3-color LEDs.
 */
struct ILedDef {
  /**
   * Initialize the pins, etc.
   */
  virtual void initPin();

  /**
   * Turn on the LED. Pay attention to whether it is common cathode or anode.
   */
  virtual void turnOn();

  /**
   * Turn off the LED. Pay attention to whether it is common cathode or anode.
   */
  virtual void turnOff();

  /**
   * Get the color.
   */
  virtual enum LedColor getColor();

  /**
   * Return whether the LED is on or not.
   */
  virtual bool isOn();

  /**
   * Call periodically to update the state.
   */
  virtual void update();
};

/**
 * LED class. Used by the Sparkle class.
 * Refer to interface ILedDef for base definitions.
 */
class LedDef : ILedDef {
  private:
  unsigned char pin;
  enum LedColor color;
  bool commonCathode;
  bool pwm;
  bool ledIsOn;
  enum DisplayMode displayMode;
#ifdef __LED_BLINK_ENABLED__
  unsigned short blinkOffDuration;
  unsigned short blinkOnDuration;
#endif
#ifdef __LED_TIMED_ENABLED__
  unsigned short timerDuration;
#endif
#ifdef __LED_BLINK_RANDOM_ENABLED__
  unsigned short randMinOffDuration;
  unsigned short randMaxOffDuration;
  unsigned short randMinOnDuration;
  unsigned short randMaxOnDuration;
  unsigned short randOffDuration;
  unsigned short randOnDuration;
#endif
  unsigned long lastTime;

  protected:
  void on();
  void off();

  public:
  /**
   * Constructor. Provide which pin, color, and whether the LED is switched using
   * common cathode or common anode, so that the logic works correctly. Also, set
   * whether the pin is PWM capable. ("false" is a safe default.)
   */
  LedDef(unsigned char ledPin, enum LedColor ledColor,
         bool ledCommonCathode, bool pwmCapable):
         pin(ledPin),
         color(ledColor),
         commonCathode(ledCommonCathode),
         pwm(pwmCapable),
         displayMode(DISABLED),
#ifdef __LED_BLINK_ENABLED__
         blinkOnDuration(0),
         blinkOffDuration(0),
#endif
#ifdef __LED_TIMED_ENABLED__
         timerDuration(0),
#endif
#ifdef __LED_BLINK_RANDOM_ENABLED__
         randMinOffDuration(0),
         randMaxOffDuration(0),
         randMinOnDuration(0),
         randMaxOnDuration(0),
         randOffDuration(0),
         randOnDuration(0),
#endif
         lastTime(0) {
  }

  void initPin();
  void turnOff();
  void turnOn();
  enum LedColor getColor();
  bool isOn();
  void update();

  /**
   * Set the blink mode with the duration on and off.
   * This does not change the mode to BLINK from MANUAL or any other mode. To
   * do that, call startBlink().
   * If onDuration or offDuration are zero, both settings are ignored.
   */
#ifdef __LED_BLINK_ENABLED__
  void setBlink(unsigned short onDuration, unsigned short offDuration);
#endif

  /**
   * Start blinking, using the parameters set by setBlink.
   * If setBlink() wasn't called to initialize the blink durations, then
   * startBlink() doesn't do anything.
   */
#ifdef __LED_BLINK_ENABLED__
  void startBlink();
#endif

  /**
   * Set the random blink mode with the max/min durations for both on and off.
   * This does not change the mode to BLINK_RANDOM from MANUAL or any other
   * mode. To do that, call startRandomBlink().
   * If any argument is zero, all settings are ignored.
   */
  void setRandomBlink(unsigned short minOffDuration,
                      unsigned short maxOffDuration,
                      unsigned short minOnDuration,
                      unsigned short maxOnDuration);

  /**
   * Start random blinking, using the parameters set by setRandomBlink.
   * If setRandomBlink() wasn't called to initialize the blink durations, then
   * startRandomBlink() doesn't do anything.
   */
  void startRandomBlink();

  /**
   * Set the time'd LED's duration.
   * This does not change the mode to TIMED from MANUAL or any other mode. To
   * do that, call startTimer().
   * If duration is zero, the setting is ignored.
   */
#ifdef __LED_TIMED_ENABLED__
  void setTimer(unsigned short duration);
#endif

  /**
   * Turn on the LED for the duration set in setTimer.
   * If setTimer() wasn't called to initialize the duration, then
   * startTimer() doesn't do anything.
   */
  void startTimer();
};


/**
 * The Sparkle class manages LEDs that are assigned to it. Enables more complex behaviors,
 * especially in groups.
 */
#ifdef __USE_SPARKLE__
class Sparkle {
  private:
  LedDef *leds;
  unsigned char count;

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

  /**
   * Update the status of the LEDs under Sparkle control.
   */
   void update();
};
#endif //__USE_SPARKLE__

#endif
