#include <arduino.h>
#include "Solenoide.h"

Solenoide::Solenoide (int pSol)
{
	pinSolenoide = pSol;
	pinMode (pinSolenoide, OUTPUT);
}

void Solenoide::turnOn ()
{
	digitalWrite (pinSolenoide, LOW);
}

void Solenoide::turnOff ()
{
	digitalWrite (pinSolenoide, HIGH);
}

bool Solenoide::isOn ()
{
	return !digitalRead (pinSolenoide);
}