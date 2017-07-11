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
}

void Iluminacao::loop ()
{
  return; // por enquanto liga apenas por comando de rede
}

int Iluminacao::channel2Pos (int channel)
{
//teto-channel começando em 1-channel começando em 0/
//1-5-4
//2-2-1
//3-3-2
//4-1-0
//5-4-3
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
  }
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

