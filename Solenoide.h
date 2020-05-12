#ifndef SOLENOIDE_H
#define SOLENOIDE_H

class Solenoide
{
	int pinSolenoide;

public:
	Solenoide (int pSol);
    void turnOn ();
    void turnOff ();
    bool isOn ();
};

#endif