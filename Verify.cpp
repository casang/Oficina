#include <arduino.h>
#include "LevelSensor.h"
#include "Solenoide.h"
#include "irrigacao.h"
#include "caixa.h"
#include "verify.h"

Verify::Verify ()
{
	panic = false;
}

bool Verify::check ()
{
	if (!digitalRead (PINBOMBA1)	&& !digitalRead (PINBOMBA2))
	{
		// alguma das solenoides tem que estar ativada
		if (digitalRead (PINSOLENOIDECAIXA) &&
			digitalRead (PINSOLENOIDEIRRIGACAOLATERAL) &&
			digitalRead (PINSOLENOIDEIRRIGACAOGRAMA1) &&
			digitalRead (PINSOLENOIDEIRRIGACAOGRAMA2))
		{
			panic = true;
			digitalWrite (PINBOMBA1, HIGH);
			digitalWrite (PINBOMBA2, HIGH);
		}
	}
	return !panic;
}


