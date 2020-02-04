#ifndef ILUMINACAO_H
#define ILUMINACAO_H

#define PINZEROCROSS 20
#define PININFRARED  32
#define PINMOVE	 	 34 
#define PINDARK		 36 	

void acLoadInt ();
void zeroCrossInt ();

class Iluminacao
{
  int iIntensity[DIMMER_CHANNELS + 1];
  bool bOn[DIMMER_CHANNELS + 1];

public:
  Dimmer *dimmer;
  Interruptor *interruptor;

  Iluminacao ();
  void loop ();
  bool isOn ();
  bool getState (int channel);
  int getIntensity (int channel);
  void setState (int channel, int state);
  void setIntensity (int channel, int intensity);
  int channel2Pos (int channel);
  int intensity2Time (int channel, int intensity);
};
#endif

extern Iluminacao *iluminacao;

