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
}

void Iluminacao::setIntensity (int intensity)
{
  for (int i = 0; i < DIMMER_CHANNELS; i++)
  {
    if (!intensity)
      dimmer->turnOff ();
    else
    {
#ifdef DEBUG
      Serial.print (intensity);
#endif
      dimmer->setIntensity (i, intensity);
    }
  }
  if (!intensity)
    interruptor->reset ();
}

bool Iluminacao::isOn ()
{
  for (int i = 0; i < DIMMER_CHANNELS; i++)
  {
    if (dimmer->isOn (i))
      return true;
  }
  return false;
}

void Iluminacao::loop ()
{
  int c;

  if (interruptor->changed ())
  {
    switch (interruptor->getLightLevel ())
    {
      case 1: // apenas o sensor de movimento
        if (!isOn ())
        {
          dimmer->setIntensity (channel2Pos(4) - 1, iluminacao->intensity2Time (4, 7), false);
          dimmer->setIntensity (channel2Pos(5) - 1, iluminacao->intensity2Time (5, 7), false);
        }
        break;
      case 5:
        for (int i = 1; i <= DIMMER_CHANNELS; i++)
        {
          c = iluminacao->channel2Pos(i);
          iluminacao->dimmer->setIntensity (c - 1, iluminacao->intensity2Time (i, 4));
        }
        break;
      case 4:
        dimmer-> turnOff ();
        c = iluminacao->channel2Pos(4);
        iluminacao->dimmer->setIntensity (c - 1, iluminacao->intensity2Time (4, 7));
        c = iluminacao->channel2Pos(5);
        iluminacao->dimmer->setIntensity (c - 1, iluminacao->intensity2Time (5, 7));
        break;
      case 3:
        dimmer-> turnOff ();
        c = iluminacao->channel2Pos(4);
        iluminacao->dimmer->setIntensity (c - 1, iluminacao->intensity2Time (4, 10));
        c = iluminacao->channel2Pos(5);
        iluminacao->dimmer->setIntensity (c - 1, iluminacao->intensity2Time (5, 10));
        break;
      case 2:
        for (int i = 1; i <= DIMMER_CHANNELS; i++)
        {
          c = iluminacao->channel2Pos(i);
          iluminacao->dimmer->setIntensity (c - 1, iluminacao->intensity2Time (i, 10));
        }
        break;
      default:
//        if (isOn ())
        dimmer-> turnOff ();
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
      if (intensity < 5)
        return 0; // desliga
      else
        return 1; // liga
    case 4:
    case 5:
      if (intensity == 0)
        return 0;
      if (intensity == 10)
        return 1;
      return 14 - intensity;
  }
}

