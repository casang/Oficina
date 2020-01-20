// Dimmer.cpp

#include "TimerOne.h"


#define SecaoDimmer

#ifdef SecaoDimmer  

#define DIMMER_CHANNELS 5

class Dimmer {
  int pinAcLoad[DIMMER_CHANNELS]; //ignite the TRIAC
  int pinZeroCross;
  bool bOn[DIMMER_CHANNELS];

  volatile int zeroCrossOn[DIMMER_CHANNELS];
  volatile int dimtimeSet[DIMMER_CHANNELS];
  int dimtime[DIMMER_CHANNELS];
  void (*zeroCrossIsr) (void);
  void (*acLoadIsr) (void);

public:
  void acLoadInterrupt();
  void zeroCrossInterrupt();  // function to be fired at the zero crossing to dim the light
  void turnOff ();
  void setIntensity (int channel, int intensity, bool setOn = true); 
  Dimmer (int pinZC, int pinAcL[], void (*zcIsr)(void), void (*acLIsr) (void), int dimt[] = NULL);
  bool isOn (int channel);
};

#endif