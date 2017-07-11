#include "TimerOne.h"
#include "arduino.h"
#include "Interruptor.h"


Interruptor::Interruptor (int pMove, int pDark, int pInfrared) 
{        
  pinMove = pMove;
  pinDark = pDark;
  pinInfrared = pInfrared;

  stInterruptor = 0;  
  stInterruptorLast = 0;
  lastDebounceTime = 0;
  lightLevel = 0;

//definicao de tempos
  debounceDelay = 100; // vezes 500u dá 50 ms
  interruptorOffDelay = 4000; // 2s apertado é luz off
  moveWindow = 10000; // 30 s de tempo quando há movimento
// fim def de tempos

  limiarDark = 600;
  limiarInfrared = 200;
  lastDebounceTime = 0;
  maxLightLevels = 4;
  lightLevel = 0;
  tmOn = 0;
  tmOff = 0;
  tmMove = 0;
}

void Interruptor::tmInterrupt()
{
  int val;
  
  // read the state of the switch into a local variable:
  if (analogRead(pinInfrared) < limiarInfrared)
  	val = 1; // acionado
  else
    val = 0; 

  if (!lightLevel)
  {
  	if (tmOff + 1 <= interruptorOffDelay)
  	{
  	  tmOff++;
  	  stInterruptor = val;
  	  return; // se desligou espera 2 s para religar
    }
  }
  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (val != stInterruptorLast) {
    // reset the debouncing timer
    lastDebounceTime = 0;
  } 
  
  if (lastDebounceTime++ > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (val != stInterruptor) {
      stInterruptor = val;
      if (stInterruptor)
        tmOn = 0;
      else
      {
        if (lightLevel == 0 or lightLevel == 1)
        {
          // ou estava desligado ou estava apenas com sensor de movimento
          lightLevel = 2;
        }
        else if (lightLevel++ >= maxLightLevels)
          lightLevel = 0;
      }
    }
    
    if (stInterruptor){
      if (tmOn++ > interruptorOffDelay)
      {
        lightLevel = 0;
        tmOff = 0;
      }
    }
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  stInterruptorLast = val;

  if (lightLevel > 1)
    return;

  if (analogRead(pinDark) < limiarDark) // durante o dia não considera sensor de presença
  {
    lightLevel = 0;
    return;
  }

  if (digitalRead(pinMove))
  {
    lightLevel = 1; //acende por mais 30s
    tmMove = moveWindow;
  }
  else
  {
    if (tmMove > 0)
    {
      if (--tmMove == 0)
        lightLevel = 0;
    }
  }

  int getLightLevel ()
  {
    return lightLevel;
  }
}

