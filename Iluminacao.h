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
public:
  Dimmer *dimmer;
  Interruptor *interruptor;

  Iluminacao ();
  void loop ();
  void setIntensity (int intensity);
  int channel2Pos (int channel);
  bool isOn ();
  int intensity2Time (int channel, int intensity);
};
#endif

extern Iluminacao *iluminacao;