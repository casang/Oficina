#include "poco.h"
#define PINSENSORCAIXALOW 38
#define PINSENSORCAIXAHIGH 40
#define PINSOLENOIDECAIXA 27

class Caixa
{
	LevelSensor *sensorCaixa;
	LevelSensor *sensorPoco;
	Solenoide *solCaixa;
	int stCaixa;

public:
	Caixa ();
	void ligaBomba (bool liga);
	bool turnOn ();
	void turnOff ();
	bool automatic ();
	bool isOn ();
	bool loop ();	
};

extern Caixa *caixa;
