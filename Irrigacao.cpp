#include <arduino.h>

#include "LevelSensor.h"
#include "Solenoide.h"
#include "Irrigacao.h"

Irrigacao::Irrigacao ()
{
	sensorPoco = new LevelSensor (true, PINSENSORPOCOLOW, PINSENSORPOCOHIGH);
	sensorCaixa = new LevelSensor (false, PINSENSORCAIXALOW, PINSENSORCAIXAHIGH);
	solIrrigacao[0] = new Solenoide (PINSOLENOIDECAIXA);
	solIrrigacao[1] = new Solenoide (PINSOLENOIDEIRRIGACAOLATERAL);
	solIrrigacao[2] = new Solenoide (PINSOLENOIDEIRRIGACAOGRAMA1);
	solIrrigacao[3] = new Solenoide (PINSOLENOIDEIRRIGACAOGRAMA2);
	pinMode (PINBOMBA1, OUTPUT);
	pinMode (PINBOMBA2, OUTPUT);
 
	canalOn = -1; // nada ligado
    for (int i = 0; i <= MAXCANAISIRRIGACAO; i++)
    {
		ligaBomba (false, i);
		tempoIrrigacao[i] = 0;
		tempoIrrigacaoSet[i] = 0;
	}
	tempoBomba = 0;
	nowLoop = now ();
}

void Irrigacao::ligaBomba (bool liga, int canal)
{
	if (liga)
	{
		if (canal != canalOn)
		{
			solIrrigacao[canal]->turnOn ();
			if (digitalRead (PINBOMBA1) != LOW)
			{
				tempoBomba = 0;
				digitalWrite (PINBOMBA1, LOW);
				digitalWrite (PINBOMBA2, LOW);
			}
		}
	}
	else
	{
		digitalWrite (PINBOMBA1, HIGH);
		digitalWrite (PINBOMBA2, HIGH);
		if (canal >= 0)
		{
			solIrrigacao[canal]->turnOff ();
		}
	}		
}

#define DEBUG

bool Irrigacao::setState (int canal, bool state)
{
	if (sensorPoco->level () == 0) // tem que ter minima H2O no poço
		return false;
	if (state)
		tempoIrrigacao[canal] = tempoIrrigacaoSet[canal];
	else
		tempoIrrigacao[canal] = 0;
	return true;
}

bool Irrigacao::getState (int canal)
{
	return (solIrrigacao[canal]->isOn ());
}

bool Irrigacao::setTimer (int canal, int Tempo)
{
	tempoIrrigacaoSet[canal] = Tempo;
	//setState (canal, true);
}

int Irrigacao::getTimer (int canal)
{
	return (tempoIrrigacao[canal]);
}

void Irrigacao::loop ()
{
	unsigned int deltaT = now () - nowLoop; // verifica quantos segundos passou entre a ultima entrada no loop

	nowLoop = now ();

	if (deltaT > 10)
		return;			// now deve estar errado pois deve passar aqui a cada 1s em média
#ifdef DEBUG
Serial.print ("CanalOn=");
Serial.println (canalOn);
for (int i = 0; i <= MAXCANAISIRRIGACAO; i++)
{
	Serial.print (tempoIrrigacao[i]);
	Serial.print ("-");
	Serial.print (tempoIrrigacaoSet[i]);	
	Serial.print ("-");
}
Serial.print ("-TB:");
Serial.print (tempoBomba);
Serial.print ("-DT:");
Serial.print (deltaT);
#endif

	if (canalOn >= 0)
	{
		tempoBomba += deltaT;
		// bomba está ligada. Se excedeu tempo máximo, desliga
		if (tempoBomba > MAXTEMPOBOMBAON)
		{
			ligaBomba (false, canalOn);
			tempoBomba = MINTEMPOBOMBAOFF;
			canalOn = -1;
			return;
		}
	}
	else
	{
		tempoBomba -= deltaT;
		if (tempoBomba < 0)
			tempoBomba = 0;
		if (tempoBomba != 0)
			return; // não faz nada enquanto a bomba não ficou tempo minimo desligada
	}
	if (sensorPoco->level () == 0) // tem que ter minima H2O no poço
	{
		ligaBomba (false, canalOn);
		if (canalOn >= 0)
			tempoIrrigacao[canalOn] = 0;
		return;
	}
	if (canalOn == 0 && sensorCaixa->level () == 2) // caixa cheia
	{
		ligaBomba (false, canalOn);
		tempoIrrigacao[canalOn] = 0;
		canalOn = -1;
		return;
	}
	if (sensorCaixa->level () == 0) // caixa vazia
	{
// sem sensor de caixa vazia
//		tempoIrrigacao[0] = -1; // liga a bomba com tempo padrao
	}
	if (canalOn >= 0)
	{
		if (tempoIrrigacao[canalOn] > 0)
		{ 
			tempoIrrigacao[canalOn] -= deltaT;
			if (tempoIrrigacao[canalOn] < 0)
				tempoIrrigacao[canalOn] = 0; // o tempo do canal que estava ativa terminou, deve desligar
		}
		if (tempoIrrigacao[canalOn] == 0)
		{
			ligaBomba (false, canalOn);
			canalOn = -1; // nada ligado
		}
	}
	for (int canal = 0; canal <= MAXCANAISIRRIGACAO; canal++)
	{
		if (tempoIrrigacao[canal] != 0) // este canal deve ser ligado
		{
			if (canalOn >= 0 && canal != canalOn)
				ligaBomba (false, canalOn);
			ligaBomba (true, canal);
			canalOn = canal;
			break;
		}
	}
}