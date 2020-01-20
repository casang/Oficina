// Oficina.ino
#include <SPI.h>
#include <Ethernet.h>
#include "TimerOne.h"
#include <Time.h> 
#include <EthernetUdp.h>

#include "Dimmer.h" 
#include "Interruptor.h"
#include "LevelSensor.h"
#include "Solenoide.h"
#include "Irrigacao.h"
#include "Iluminacao.h"
#include "Rede.h"
#include "TimeNTP.h"
#include "Verify.h"
#include <idDHT11.h>
#include "mega.h"

Iluminacao *iluminacao = NULL;
Irrigacao *irrigacao = NULL;
Rede *rede = NULL;
NTP *ntp = NULL;
idDHT11 *dht11 = NULL;


#define PINTEMPHUMIDITYSENSOR 19
// This wrapper is in charge of calling 
// mus be defined like this for the lib work

void loopDHT11()
{
  Serial.print("\nRetrieving information from sensor: ");
  Serial.print("Read sensor: ");
  if (!dht11)
    return;

  //int result = IDDHTLIB_OK;
  int result = dht11->acquireAndWait();
  switch (result)
  {
  case IDDHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case IDDHTLIB_ERROR_CHECKSUM: 
    Serial.println("Error\n\r\tChecksum error"); 
    break;
  case IDDHTLIB_ERROR_ISR_TIMEOUT: 
    Serial.println("Error\n\r\tISR time out error"); 
    break;
  case IDDHTLIB_ERROR_RESPONSE_TIMEOUT: 
    Serial.println("Error\n\r\tResponse time out error"); 
    break;
  case IDDHTLIB_ERROR_DATA_TIMEOUT: 
    Serial.println("Error\n\r\tData time out error"); 
    break;
  case IDDHTLIB_ERROR_ACQUIRING: 
    Serial.println("Error\n\r\tAcquiring"); 
    break;
  case IDDHTLIB_ERROR_DELTA: 
    Serial.println("Error\n\r\tDelta time to small"); 
    break;
  case IDDHTLIB_ERROR_NOTSTARTED: 
    Serial.println("Error\n\r\tNot started"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
  Serial.print("Humidity (%): ");
  Serial.println(dht11->getHumidity(), 2);

  Serial.print("Temperature (oC): ");
  Serial.println(dht11->getCelsius(), 2);
}

void interruptorInt ()
{
  if (iluminacao && iluminacao->interruptor)
    iluminacao->interruptor->tmInterrupt ();
}

void acLoadInt ()
{
  if (iluminacao && iluminacao->dimmer)
    iluminacao->dimmer->acLoadInterrupt ();
}

void zeroCrossInt ()
{
  if (iluminacao && iluminacao->dimmer)
    iluminacao->dimmer->zeroCrossInterrupt ();
}

// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dht11_wrapper() {
  if (dht11)
    dht11->isrCallback();
}

void tmInterrupt ()
{
  acLoadInt ();
  interruptorInt ();
}

#define PINBLINK 13

void setup()
{
  delay (2000); // waiting for stabilization

  Serial.begin (19200);
  Serial.println ("SanBikes - Oficina.ino");
  pinMode(PINBLINK, OUTPUT);
  iluminacao = new Iluminacao;
  irrigacao = new Irrigacao;
  rede = new Rede;
  //ntp = new NTP;
  //setSyncProvider(getNtpTimeFunc);
  //dht11 = new idDHT11(PINTEMPHUMIDITYSENSOR, digitalPinToInterrupt (PINTEMPHUMIDITYSENSOR),dht11_wrapper);
  Timer1.initialize(500);
  Timer1.attachInterrupt(tmInterrupt);  // attaches callback() as a timer overflow interrupt
}

void loop ()
{
  Verify verify;
  static bool blink = false;

 /* if (blink)
    digitalWrite(PINBLINK, HIGH);
  else
    digitalWrite(PINBLINK, LOW);
  blink = !blink;
*/
  if (verify.check ())
  {  
    irrigacao->loop ();
  }
  iluminacao->loop ();
  rede->loop ();
  //ntp->loop ();
  //loopDHT11 ();
  delay(1000);
}
