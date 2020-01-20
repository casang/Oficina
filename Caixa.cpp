#include <arduino.h>

#include "LevelSensor.h"
#include "Solenoide.h"
#include "Caixa.h"

Caixa::Caixa ()
{
	sensorCaixa = new LevelSensor (false, PINSENSORCAIXALOW, PINSENSORCAIXAHIGH);
	sensorPoco = new LevelSensor (true, PINSENSORPOCOLOW, PINSENSORPOCOHIGH);
	solCaixa = new Solenoide (PINSOLENOIDECAIXA);
	solCaixa->turnOff ();

	// por enquanto apenas manual
	stCaixa = 0;
	//stCaixa = 2; // modo automatico
}

void Caixa::ligaBomba (bool liga)
{
	if (liga)
	{
		solCaixa->turnOn ();
		digitalWrite (PINBOMBA1, LOW);
		digitalWrite (PINBOMBA2, LOW);
	}
	else
	{
		digitalWrite (PINBOMBA1, HIGH);
		digitalWrite (PINBOMBA2, HIGH);
		solCaixa->turnOff ();
	}		
}

bool Caixa::turnOn ()
{
	if ((sensorPoco->level () == 0) || // tem que ter minima H2O no poço
		(sensorCaixa->level () == 2)) // caixa cheia
		return false;
	ligaBomba (true);
	stCaixa = 1;
	return true;
}

void Caixa::turnOff ()
{
	ligaBomba (false);
	stCaixa = 0;
}

bool Caixa::automatic ()
{
	stCaixa = 2;
	return (!digitalRead (PINBOMBA1));
}

bool Caixa::isOn ()
{
	return (!digitalRead (PINBOMBA1));
}

bool Caixa::loop ()
{
	switch (stCaixa)
	{
		case 1: // comando de ligada continuamente 
			if ((sensorPoco->level () == 0) || // tem que ter minima H2O no poço
				(sensorCaixa->level () == 2)) // caixa cheia
			{
				stCaixa = 2;
				ligaBomba (false);
				return false;
			}
			return true;
		case 0:
			return false; // comando de desligada continuamente
		case 2:
			if (isOn ())
			{
				if ((sensorPoco->level () == 0) || // tem que ter minima H2O no poço
					(sensorCaixa->level () == 2)) // caixa cheia
				{
					ligaBomba (false);
					return false;
				}
				return true;
			}
			else
			{
				if ((sensorPoco->level () > 1) && // tem que ter H2O no poço maior que High
					(sensorCaixa->level () == 0)) // caixa abaixo de Low
				{
					ligaBomba (true);
					return true;
				}
				return false;
			}
	}
	return false; // erro de codigo pois nunca deve chegar aqui
}