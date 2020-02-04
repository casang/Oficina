// Dimmer.cpp

#include "TimerOne.h"


#define SecaoDimmer

#ifdef SecaoDimmer  

// 
// canal 0 é utilizado para comandar os outros n canais
// não possui dispositivo fisico associado
//
#define DIMMER_CHANNELS 5

class Dimmer {
  int pinAcLoad[DIMMER_CHANNELS]; //ignite the TRIAC
  int pinZeroCross;

  volatile int zeroCrossOn[DIMMER_CHANNELS];
  volatile int dimtimeSet[DIMMER_CHANNELS];
  int dimtime[DIMMER_CHANNELS];
  void (*zeroCrossIsr) (void);
  void (*acLoadIsr) (void);

public:
  void acLoadInterrupt();
  void zeroCrossInterrupt();  // function to be fired at the zero crossing to dim the light
  void setIntensity (int channel, int intensity); 
  Dimmer (int pinZC, int pinAcL[], void (*zcIsr)(void), void (*acLIsr) (void), int dimt[] = NULL);
};

#endif