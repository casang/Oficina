#include <arduino.h>
#include "LevelSensor.h"

LevelSensor::LevelSensor (bool na, int pinL, int pinH)
{
	isNA = na;
	pinLow = pinL;
	pinHigh = pinH;
}

int LevelSensor::level ()
{
	int h = digitalRead (pinHigh);
	int l = digitalRead (pinLow);
	if (isNA)
	{
	  if (h && l)
	  	return 0;
	  else if (h && !l)
	  	return 1;
	  else
	  	return 2;
	}
	else
	{
	  if (!h && !l)
	  	return 0;
	  else if (!h && l)
	  	return 1;
	  else
	  	return 2;
	}
}
