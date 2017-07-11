//interruptor.h


class Interruptor
{
  int pinMove;
  int pinDark;
  int pinInfrared;
  int stInterruptor;
  int stInterruptorLast;
  int debounceDelay;
  long lastDebounceTime;
  long interruptorOffDelay;
  int maxLightLevels;
  int lightLevel;
  int tmOn;
  int tmOff;
  int tmMove;
  long moveWindow;
  long limiarDark;
  long limiarInfrared;

public:

  Interruptor (int pinMove, int pinDark, int pinInfrared);
  void tmInterrupt ();
  int getLightLevel ();
};


