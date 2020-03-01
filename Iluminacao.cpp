#include <arduino.h>

#include "TimerOne.h"
#include "Dimmer.h"
#include "Interruptor.h"
#include "Iluminacao.h"

int pinAcLoad[] = {22, 24, 26, 28, 30}; 

Iluminacao::Iluminacao ()
{
  dimmer = new Dimmer (PINZEROCROSS, pinAcLoad, &zeroCrossInt, &acLoadInt, NULL);
  interruptor = new Interruptor (PINMOVE, PINDARK, PININFRARED);
  for (int i = 0; i <= DIMMER_CHANNELS; i++)
  {
    iIntensity[i] = 0;
    bOn[i] = 0;
  }
}

bool Iluminacao::isOn ()
{
  for (int i = 1; i <= DIMMER_CHANNELS; i++)
    if (bOn[i])
      return true;
  return false;
}

bool Iluminacao::getState (int channel)
{
  return bOn[channel];
}

int Iluminacao::getIntensity (int channel)
{
  return iIntensity[channel];
}

void Iluminacao::setState (int channel, int state)
{
  int c = channel;

  if (c >= 1)
  {
    bOn[c] = state;
    if (bOn[c])
      dimmer->setIntensity (c - 1, intensity2Time (c, iIntensity[c]));
    else
      dimmer->setIntensity (c - 1, intensity2Time (c, 0));
  }
  else
    for (c = 0; c <= DIMMER_CHANNELS; c++)
    {
      bOn[c] = state;
      if (bOn[c])
        dimmer->setIntensity (c - 1, intensity2Time (c, iIntensity[c]));
      else
        dimmer->setIntensity (c - 1, intensity2Time (c, 0));
    }
}

void Iluminacao::setIntensity (int channel, int intensity)
{
  int c = channel;

  if (c >= 1)
  {
    iIntensity[c] = intensity;
    if (intensity == 0)
      bOn[c] = 0;
    else
      bOn[c] = true;
    dimmer->setIntensity (c - 1, intensity2Time (c, intensity));
  }
  else
  {
    for (c = 0; c <= DIMMER_CHANNELS; c++)
    {
      iIntensity[c] = intensity;
      if (intensity == 0)
        bOn[c] = 0;
      else
        bOn[c] = true;
      if (c > 0)
        dimmer->setIntensity (c - 1, intensity2Time (c, intensity));
    }
  }
}

#define DEBUG

void Iluminacao::loop ()
{
  int c;

  if (interruptor->changed ())
  {
#ifdef DEBUG
    Serial.print("int:");
    Serial.print(interruptor->getLightLevel ());
#endif
    switch (interruptor->getLightLevel ())
    {
      case 1: // apenas o sensor de movimento
        if (!isOn ())
        {
          setState (0, 0);
          c = channel2Pos(4);
          setIntensity (c, 70);
          c = channel2Pos(5);
          setIntensity (c, 70);
          interruptor->autoTurnOff (true);
        }
        break;
      case 5:
        setState (0, 0);
        c = channel2Pos(4);
        setIntensity (c, 40);
        c = channel2Pos(5);
        setIntensity (c, 40);
        break;
      case 4:
        setState (0, 0);
        c = channel2Pos(4);
        setIntensity (c, 70);
        c = channel2Pos(5);
        setIntensity (c, 70);
        break;
      case 3:
        setState (0, 0);
        c = channel2Pos(4);
        setIntensity (c, 100);
        c = channel2Pos(5);
        setIntensity (c, 100);
        break;
      case 2:
        for (int i = 1; i <= DIMMER_CHANNELS; i++)
        {
          c = channel2Pos(i);
          setIntensity (c, 100);
        }
        break;
      default:
//       if (isOn ())
       setState (0, 0);
       break;
    }
  }
}

// Fios ligados as lampadas (anilhas)
// 1: preta
// 2: Cinza
// 3: Vermelho
// 4: Verde
// 5: Amarelo
int Iluminacao::channel2Pos (int channel)
{
  return channel;
/*  
  switch (channel)
  {
    case 1:
      return 5;
    case 2:
      return 2;
    case 3:
      return 3;
    case 4:
      return 1;
    case 5:
      return 4;
  }*/
}

int Iluminacao::intensity2Time (int channel, int intensity)
{
  switch (channel)
  {
    case 1:
    case 2:
    case 3: // lampadas não dimerizaveis
      if (intensity < 50)
        return 0; // desliga
      else
        return 1; // liga
    case 4:
    case 5:
      if (intensity == 0)
        return 0;
      if (intensity == 100)
        return 1;
      return 14 - (intensity / 10);
  }
}

