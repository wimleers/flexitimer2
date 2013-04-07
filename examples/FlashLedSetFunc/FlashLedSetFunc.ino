/*
  FlexiTimer2:
  Arduino library to use timer 2 with a configurable resolution.
  Based on MsTimer2 by Javier Valencia. It is called FlexiTimer2 because it
  is based on MsTimer2, but offers more flexibility,
  since it has a configurable timer resolution.
  MsTimer2 library: http://www.arduino.cc/playground/Main/MsTimer2

For more details on FlexiTimer2 see: 
 http://www.arduino.cc/playground/Main/FlexiTimer2
 https://github.com/wimleers/flexitimer2

*/

#include <FlexiTimer2.h>

// Switch on LED on and off each half second with the new function setFunc()

#if defined(ARDUINO) && ARDUINO >= 100
  #define LED_PIN LED_BUILTIN	// 1.0 built in LED pin var
#else
  #if defined(CORE_LED0_PIN)
    #define LED_PIN CORE_LED0_PIN	// 3rd party LED pin define
  #else
    #define LED_PIN 13			// default to pin 13
  #endif
#endif

void switchOn()
{
  digitalWrite(LED_PIN, HIGH);
  FlexiTimer2::setFunc(switchOff);
}

void switchOff()
{
  digitalWrite(LED_PIN, LOW);
  FlexiTimer2::setFunc(switchOn);
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  FlexiTimer2::set(500, 1.0/1000, switchOn); // call every 500 1ms "ticks"
  // FlexiTimer2::set(500, flash); // MsTimer2 style is also supported
  FlexiTimer2::start();
}

void loop()
{
}