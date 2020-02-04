#include "poco.h"
#include "Time.h"
#define PINSOLENOIDEIRRIGACAOLATERAL 33
#define PINSOLENOIDEIRRIGACAOGRAMA1 31
#define PINSOLENOIDEIRRIGACAOGRAMA2 29
#define PINSENSORCAIXALOW 38
#define PINSENSORCAIXAHIGH 40
#define PINSOLENOIDECAIXA 27
#define MAXCANAISIRRIGACAO 3
#define TEMPOIRRIGACAO (15 * 60) // 15 min
#define MAXTEMPOBOMBAON (15 * 60) // 15 min
#define MINTEMPOBOMBAOFF (2 * 60) // 2 min

class Irrigacao
{
	LevelSensor *sensorPoco;
	LevelSensor *sensorCaixa;
	Solenoide *solIrrigacao[MAXCANAISIRRIGACAO + 1]; // 0 = CAIXA, 1... = IRRIGACOES
	time_t nowLoop;
	int canalOn;
	int tempoIrrigacao[MAXCANAISIRRIGACAO + 1]; 
	int tempoIrrigacaoSet[MAXCANAISIRRIGACAO + 1]; 
	int tempoBomba;
	void ligaBomba (bool liga, int canal);

public:
	Irrigacao ();
	bool setTimer (int canal, int tempo = TEMPOIRRIGACAO);
	bool setState (int canal, bool state = false);
	int getTimer (int canal);
	bool getState (int canal);
	void loop ();	
};

extern Irrigacao *irrigacao;