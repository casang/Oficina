// Dimmer.cpp

#include "arduino.h"
#include "pins_arduino.h"
#include "TimerOne.h"
#include "dimmer.h"
#include "mega.h"

void Dimmer::acLoadInterrupt()
{
  //  digitalWrite( 13, digitalRead( 13 ) ^ 1 );
  for (int i = 0; i < DIMMER_CHANNELS; i++)
  {
    if ((zeroCrossOn[i] > 1) && dimtime[i])
    {
      if (--dimtime[i] == 0)
      {
        digitalWrite(pinAcLoad[i], HIGH);   // triac firing
        delayMicroseconds(10);         // triac On propagation delay (for 60Hz use 8.33)
        digitalWrite(pinAcLoad[i], LOW);    // triac Off
      }
    }
  }
}

void Dimmer::zeroCrossInterrupt()  // function to be fired at the zero crossing to dim the light
{
  noInterrupts ();
  for (int i = 0; i < DIMMER_CHANNELS; i++)
  {
    if (zeroCrossOn[i] > 1)
      dimtime[i] = dimtimeSet[i];
  }
  interrupts ();
}

Dimmer::Dimmer (int pinZC, int pinAcL[], void (*zcIsr)(void), void (*acLdIsr) (void), int dimt[])
{
  acLoadIsr = acLdIsr;
  zeroCrossIsr = zcIsr;
	pinZeroCross = pinZC;
  for (int i = 0; i < DIMMER_CHANNELS; i++){
    zeroCrossOn[i] = 0;
    pinAcLoad[i] = pinAcL[i];
    dimtime[i] = 0;
    if (!dimt)
      dimtimeSet[i] = 0;
    else
      dimtimeSet[i] = dimt[i];
    bOn[i] = false;
    pinMode(pinAcLoad[i], OUTPUT);// Set AC Load pin as output
  }
  pinMode(pinZeroCross, INPUT);

  attachInterrupt(digitalPinToInterrupt (pinZeroCross), zeroCrossIsr, RISING);  // Choose the zero cross interrupt # from the table above
}

void Dimmer::turnOff ()
{
  for (int i = 0; i < DIMMER_CHANNELS; i++){
    zeroCrossOn[i] = 0;
    digitalWrite(pinAcLoad[i], LOW);    // triac Off
    bOn[i] = false;
  }
}

void Dimmer::setIntensity (int channel, int intensity, bool setOn)
{
  switch (intensity)
  {
    case 0:
      bOn[channel] = false;
      zeroCrossOn[channel] = 0;
      digitalWrite(pinAcLoad[channel], LOW);    // triac Off
      break;
    case 1:
      if (setOn)
        bOn[channel] = true;
      zeroCrossOn[channel] = 1;
      noInterrupts ();
      dimtimeSet[channel] = 0;
      digitalWrite(pinAcLoad[channel], HIGH);    // triac On
      interrupts ();
      break;
    default:
      if (setOn)
        bOn[channel] = true;
      zeroCrossOn[channel] = 2;
      noInterrupts ();
      dimtimeSet[channel] = intensity;
      interrupts ();
      break;
  }
}

bool Dimmer::isOn (int channel)
{
  return bOn[channel];
} 
