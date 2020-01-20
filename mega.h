// Arduino Mega 
#define digitalPinToInterrupt(p)  ( (p) == 2 ? 0 : ((p) == 3 ? 1 : ((p) >= 18 && (p) <= 21 ? 23 - (p) : -1)) ) 

