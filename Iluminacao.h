#ifndef ILUMINACAO_H
#define ILUMINACAO_H

#define PINZEROCROSS 20
#define PININFRARED  7
#define PINDARK		 6 	
#define PINMOVE	 52 
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
  int intensity2Time (int channel, int intensity);
};
#endif

extern Iluminacao *iluminacao;