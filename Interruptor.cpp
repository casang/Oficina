
#include "TimerOne.h"
#include "arduino.h"
#include "Interruptor.h"
#include "Time.h"


Interruptor::Interruptor (int pMove, int pDark, int pInfrared) 
{        
  pinMove = pMove;
  pinDark = pDark;
  pinInfrared = pInfrared;

//definicao de tempos
  debounceDelayLow = 500; // vezes 500u
  debounceDelayHigh = 10; // vezes 500u
  interruptorOffDelay = 4000; // 2s apertado é luz off
  moveWindow = 60000; // 30 s de tempo quando há movimento
  interruptorOffDelay = 4000; // tempo que evita sensor de move apos desligamento
// fim def de tempos

  limiarDark = 600;
  maxLightLevels = 5;

  moveAction = 1; // inicia indicando que só deve acender por movimento quando está escuro
  //moveAction = 2; // por enquando acende em qualquer horario
  reset ();
}

void Interruptor::tmInterrupt()
{
  int val;
  
// read the state of the switch into a local variable:
  if (!digitalRead(pinInfrared))
  {
  	val = 1; // acionado
    digitalWrite(13, HIGH);    
  }
  else
  {
    val = 0; 
    digitalWrite(13, LOW);
  }

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (val != stInterruptor) 
    // reset the debouncing timer
    lastDebounceTime = 0;
  else
  {  
    if (((val == 0) && (lastDebounceTime + 1 < debounceDelayLow)) ||
        ((val == 1) && (lastDebounceTime + 1 < debounceDelayHigh)))
      lastDebounceTime++;
    else
    {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
      if (val == 0)
        stInterruptorLast = 1; // ficou tempo suficiente em 0
      if ((val == 1) && (stInterruptorLast == 1))
      {
        stInterruptorLast = 0;
        lightLevel++;
        if (lightLevel == 1)
          lightLevel++;
        if (lightLevel > maxLightLevels)
        {
          lightLevel = 0; // desligou. Espera xx segundos antes que sensor de presença acione novamente
          tmOff = interruptorOffDelay;       
        }
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  stInterruptor = val;

  if (lightLevel > 1)
    return;
  if (tmOff > 0)
  {
    tmOff--;
    return;
  }

  //ainda sem LDR
/*  
  if (analogRead(pinDark) < limiarDark) // durante o dia não considera sensor de presença
  {
    lightLevel = 0;
    return;
  }
*/
  if (!digitalRead(pinMove))
  {
    switch (moveAction)
    {
      case 0:
        break; // não acende por movimento

      case 1:
        int hora;

        if (timeStatus () == timeSet)
        {
          hora = hour ();
          if (hora >= 7 && hora <= 16)
            break;  // neste horário não acende por movimento  
        }
        lightLevel = 1; //acende por mais 30s
        if (autoTO)
          tmMove = moveWindow;
        break;

      case 2:
        lightLevel = 1; //acende por mais 30s
        if (autoTO)
          tmMove = moveWindow;
        break;
    }
  }
  else
  {
    if (tmMove > 0)
    {
      if (--tmMove == 0)
        lightLevel = 0;
    }
  }
/*
  if (tmMove)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
*/
}

int Interruptor::getLightLevel ()
{
  return lightLevel;
}

void Interruptor::autoTurnOff (bool valor)
{
  // usado para ajustar quando o comando de acender nao vem pelo inerruptor
  // neste caso não deve apagar automatico
  if (!valor)
  {
    tmOff = 0;
    tmMove = 0;
    lastLightLevel = -1; // proxima vez que acionar o interruptor vai para o primeiro nivel (estado 2)
    lightLevel = 1;
  }
  autoTO = valor;
  if (autoTO)
    tmMove = moveWindow;
}

bool Interruptor::changed ()
{
  if (lastLightLevel != lightLevel)
  {
    lastLightLevel = lightLevel;
    return true;
  }
  return false;
}

void Interruptor::setMoveAction (int act)
{
  moveAction = act;
}

void Interruptor::reset ()
{
  stInterruptor = 0;  
  stInterruptorLast = 0;
  lastDebounceTime = 0;
  lightLevel = 0;
  lastLightLevel = 0;
  lastDebounceTime = 0;
  lightLevel = 0;
  lastLightLevel = 0;
  tmOn = 0;
  tmOff = 0;
  tmMove = 0;  
  autoTO = 0;
}

