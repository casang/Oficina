//interruptor.h


class Interruptor
{
  int pinMove;
  int pinDark;
  int pinInfrared;
  int stInterruptor;
  int stInterruptorLast;
  int debounceDelayLow;
  int debounceDelayHigh;
  long lastDebounceTime;
  long interruptorOffDelay;
  int maxLightLevels;
  int lightLevel;
  int lastLightLevel;
  int tmOn;
  int tmOff;
  long tmMove;
  long moveWindow;
  long limiarDark;
  int moveAction;

public:

  Interruptor (int pinMove, int pinDark, int pinInfrared);
  void tmInterrupt ();
  int getLightLevel ();
  bool changed ();
  void setMoveAction (int act);
  void reset ();
};


