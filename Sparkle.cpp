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
   * For internal class use only. Public method is turnOff().
   */
  void LedDef::off() {
    digitalWrite(pin, (commonCathode?LOW:HIGH));
    ledIsOn = false;
  }

  /**
   * Turn off the LED.
   */
  void LedDef::turnOff() {
    off();
    displayMode = MANUAL;
  }

  /**
   * Turn on the LED. Pay attention to whether it is common cathode or anode.
   * For internal class use only. Public method is turnOn().
   */
  void LedDef::on() {
    digitalWrite(pin, (commonCathode?HIGH:LOW));
    ledIsOn = true;
  }

  /**
   * Turn on the LED. Pay attention to whether it is common cathode or anode.
   */
  void LedDef::turnOn() {
    on();
    displayMode = MANUAL;
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

  /**
   * Set the blink mode with the duration on and off.
   * This does not change the mode to BLINK from MANUAL or any other mode. To
   * do that, call startBlink().
   * If onDuration or offDuration are zero, both settings are ignored.
   */
  void LedDef::setBlink(unsigned short onDuration, unsigned short offDuration) {
    if ((onDuration > 0) && (offDuration > 0)) {
      blinkOnDuration = onDuration;
      blinkOffDuration = offDuration;
    }
  }

  /**
   * Start blinking, using the parameters set by setBlink.
   * If setBlink() wasn't called to initialize the blink durations, then
   * startBlink() doesn't do anything.
   */
  void LedDef::startBlink() {
    if ((blinkOnDuration > 0) && (blinkOffDuration > 0)) {
      on();
      lastTime = millis();
      displayMode = BLINK;
    }
  }

  /**
   * Set the random blink mode with the max/min durations for both on and off.
   * This does not change the mode to BLINK_RANDOM from MANUAL or any other
   * mode. To do that, call startRandomBlink().
   * If any argument is zero, all settings are ignored.
   */
  void LedDef::setRandomBlink(unsigned short minOffDuration,
                              unsigned short maxOffDuration,
                              unsigned short minOnDuration,
                              unsigned short maxOnDuration) {
    if ((minOffDuration > 0) && (maxOffDuration > 0) &&
        (minOnDuration > 0) && (maxOnDuration > 0)) {
         randMinOffDuration = minOffDuration;
         randMaxOffDuration = maxOffDuration;
         randMinOnDuration = minOnDuration;
         randMaxOnDuration = maxOnDuration;
    }
  }

  /**
   * Start random blinking, using the parameters set by setRandomBlink.
   * If setRandomBlink() wasn't called to initialize the blink durations, then
   * startRandomBlink() doesn't do anything.
   */
  void LedDef::startRandomBlink() {
    if ((randMinOffDuration > 0) && (randMaxOffDuration > 0) &&
        (randMinOnDuration > 0) && (randMaxOnDuration > 0)) {
      // Don't just automatically start with the LED on as in other modes,
      // as this is a random blink pattern...
      if (random(2)) {
        on();
        randOnDuration = random(randMinOnDuration, randMaxOnDuration);
      } else {
        off();
        randOffDuration = random(randMinOffDuration, randMaxOffDuration);
      }
      lastTime = millis();
      displayMode = BLINK_RANDOM;
    }
  }

  /**
   * Set the time'd LED's duration.
   * This does not change the mode to TIMED from MANUAL or any other mode. To
   * do that, call startTimer().
   * If duration is zero, the setting is ignored.
   */
  void LedDef::setTimer(unsigned short duration) {
    if (duration > 0) {
      timerDuration = duration;
    }
  }

  /**
   * Turn on the LED for the duration set in setTimer.
   * If setTimer() wasn't called to initialize the duration, then
   * startTimer() doesn't do anything.
   */
  void LedDef::startTimer() {
    if (timerDuration > 0) {
      on();
      lastTime = millis();
      displayMode = TIMED;
    }
  }

  /**
   * Call periodically to update the state.
   */
  void LedDef::update() {
    unsigned long now=0;

    switch (displayMode) {
      case BLINK:
        now = millis();
        if (ledIsOn) {
          // LED is currently on. See if on duration has expired.
          if (now >= (lastTime + blinkOnDuration)) {
            off();
            lastTime = now;
          }
        } else {
          // LED is currently off. See if off duration has expired.
          if (now >= (lastTime + blinkOffDuration)) {
            on();
            lastTime = now;
          }
        }
        break;

      case BLINK_RANDOM:
        now = millis();
        if (ledIsOn) {
          // LED is currently on. See if on duration has expired.
          if (now >= (lastTime + blinkOnDuration)) {
            off();
            lastTime = now;
            randOffDuration = random(randMinOffDuration, randMaxOffDuration);
          }
        } else {
          // LED is currently off. See if off duration has expired.
          if (now >= (lastTime + blinkOffDuration)) {
            on();
            lastTime = now;
            randOnDuration = random(randMinOnDuration, randMaxOnDuration);
          }
        }
        break;

      case TIMED:
        if (ledIsOn) {
          // LED is currently on. See if on duration has expired.
          if (millis() >= (lastTime + timerDuration)) {
            off();
            displayMode = MANUAL;
          }
        }
        break;

      case FADE:
        if (pwm) {
          //TODO
        }
        break;

      case MANUAL:
      default:
        break;
    }
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

/**
 * Update the status of the LEDs under Sparkle control.
 */
void Sparkle::update() {
  for (unsigned short i=0; i<count; i++) {
    leds[i].update();
  }
}
