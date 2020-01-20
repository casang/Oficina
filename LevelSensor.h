class LevelSensor
{
private:
	bool isNA;
	int pinHigh;
	int pinLow;

public:
	LevelSensor (bool na, int pinL, int pinH);
	int level (); // 0 - abaixo do Low, 1 - entre Low e High e 2 - acima do High
};