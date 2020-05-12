//Inspirado no módulo abaixo
// This example uses an Arduino Uno together with
// an Ethernet Shield to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <Ethernet.h>
#include <MQTT.h>

void oficinaMqtt_warpper (String &topic, String &payload);

#define MQTT_SERVER_ADDR "192.168.1.178"

class oficinaMqtt
{
	EthernetClient *pNet;
	MQTTClient client;

	void connect();
	void sendStatIluminacao (int channel) ;
	void sendStatIrrigacao (int channel) ;

public:
	void messageReceived(String &topic, String &payload);
	oficinaMqtt ();
	void setup ();
	void loop ();
};
