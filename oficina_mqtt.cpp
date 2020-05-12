
//Inspirado no módulo abaixo
// This example uses an Arduino Uno together with
// an Ethernet Shield to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include "oficina_mqtt.h"
#include "Dimmer.h"
#include "Interruptor.h"
#include "iluminacao.h"
#include "irrigacao.h"

#define SIZE_OFF_CMND 200
#define SIZE_OFF_STAT 200

void oficinaMqtt::connect() 
{
	//Serial.print("connecting...");
	if (pNet) 
	{
		//Serial.print("connecting pNet not Null...");
	  	if (client.connect("ardn_01")) 
	  	{
	  		for (int i = 1; i <= DIMMER_CHANNELS; i++)
			{
				String topic = "cmnd/ardn_01/lght_0";
				topic.concat (i);
				client.subscribe (topic.c_str ());
			}		
	  		for (int i = 1; i <= MAXCANAISIRRIGACAO; i++)
			{
				String topic = "cmnd/ardn_01/valv_0";
				topic.concat (i);
				client.subscribe (topic.c_str ());
			}		
		}
	}
}

void oficinaMqtt::messageReceived (String &topic, String &payload) 
{
	Serial.println("incoming: " + topic + " - " + payload);

	StaticJsonDocument<SIZE_OFF_CMND> cmnd;

	DeserializationError err = deserializeJson (cmnd, payload.c_str());
	if (!err)
	{
		int channelCmd;
		int valueCmd = -1;
		int l = topic.length ();

		if (l > 1 && !strncmp (topic.c_str(), "cmnd/ardn_01/lght_0", l -1))
		{
			Serial.println ("topic light OK");
			channelCmd = topic.charAt(l - 1) - '0';

			Serial.print ("chanellCmd:");
			Serial.println (channelCmd);

			if (channelCmd >= 1 && channelCmd <= DIMMER_CHANNELS)
			{
				//estado
				JsonVariant aux = cmnd.getMember("state");

				if (!aux.isNull ())
				{
					if (!strcasecmp ((const char*)aux, "ON"))
						valueCmd = 1;
					else if (!strcasecmp ((const char*)aux, "OFF"))
						valueCmd = 0;

					Serial.print ("valeuCmd:");
					Serial.println (valueCmd);

					if (valueCmd >= 0 && iluminacao)
		    		{
		        		Serial.println ("State");
		        		iluminacao->setState (channelCmd, valueCmd);
		        	}
		        }
				
		        //brilho
		        aux = cmnd.getMember("brightness");
				Serial.print ("brightness:");
				Serial.println ((int)aux);

				if (!aux.isNull ())
				{
					valueCmd = (int)aux;

					Serial.print ("valeuCmd:");
					Serial.println (valueCmd);

					if (valueCmd >= 0 && iluminacao)
		    		{
		        		Serial.println ("Brilho");
		        		iluminacao->setIntensity (channelCmd, valueCmd);
		        	}
		        }

		        sendStatIluminacao (channelCmd);

		        //interruptor
		        iluminacao->interruptor->autoTurnOff (false);
		        if (!iluminacao->isOn () && iluminacao->interruptor)
		        	iluminacao->interruptor->reset (); // se apagou tudo reset no interruptor
		    }
		}

		//// Irrigação
		if (l > 1 && !strncmp (topic.c_str(), "cmnd/ardn_01/valv_0", l -1))
		{
			Serial.println ("topic valve OK");
			channelCmd = topic.charAt(l - 1) - '0';

			Serial.print ("chanellCmd:");
			Serial.println (channelCmd);

			if (channelCmd >= 1 && channelCmd <= MAXCANAISIRRIGACAO)
			{
				//estado
				JsonVariant aux;

		        //tempo
		        aux = cmnd.getMember("time");
				Serial.print ("time:");
				Serial.println ((int)aux);

				if (!aux.isNull ())
				{
					valueCmd = (int)aux;

					Serial.print ("valeuCmd:");
					Serial.println (valueCmd);

					if (valueCmd >= 0 && irrigacao)
		    		{
		        		Serial.println ("Time");
		        		irrigacao->setTimer (channelCmd, valueCmd);
		        	}
		        }

				aux = cmnd.getMember("state");
				if (!aux.isNull ())
				{
					if (!strcasecmp ((const char*)aux, "ON"))
						valueCmd = 1;
					else if (!strcasecmp ((const char*)aux, "OFF"))
						valueCmd = 0;

					Serial.print ("valeuCmd:");
					Serial.println (valueCmd);

					if (valueCmd >= 0 && irrigacao)
		    		{
		        		Serial.println ("state");
		        		irrigacao->setState (channelCmd, valueCmd);
		        	}
		        }
				
		        sendStatIrrigacao (channelCmd);
		    }
		}
	}

	// Note: Do not use the client in the callback to publish, subscribe or
	// unsubscribe as it may cause deadlocks when other things arrive while
	// sending and receiving acknowledgments. Instead, change a global variable,
	// or push to a queue and handle it in the loop after calling `client.loop()`.
}

oficinaMqtt::oficinaMqtt ()
{
	pNet = new EthernetClient ();
	setup ();
}

void oficinaMqtt::setup ()
{
	// Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
	// by Arduino. You need to set the IP address directly.
	//client.begin("broker.shiftr.io", net);
	client.begin (MQTT_SERVER_ADDR, *pNet);
	client.onMessage (oficinaMqtt_warpper);

	connect();
}

void oficinaMqtt::sendStatIluminacao (int channel) 
{
	if (!client.connected()) 
	{
		//Serial.print("desconectou->lastError:");
		//Serial.println(client.lastError());
		connect();
	}

	if (!iluminacao)
		return;

	StaticJsonDocument<SIZE_OFF_STAT> stat;

	stat["state"] = iluminacao->getState (channel) ? "ON" : "OFF";
	stat["brightness"] = iluminacao->getIntensity (channel);

	String topic = "stat/ardn_01/lght_0";
	topic.concat (channel);
	topic.concat ("/RESULT");

	char valor[100];
	serializeJson (stat, valor, sizeof (valor));
	client.publish(topic.c_str(), valor);
}

void oficinaMqtt::sendStatIrrigacao (int channel) 
{
	if (!client.connected()) 
	{
		//Serial.print("desconectou->lastError:");
		//Serial.println(client.lastError());
		connect();
	}

	if (!irrigacao)
		return;

	StaticJsonDocument<SIZE_OFF_STAT> stat;

	stat["state"] = irrigacao->getState (channel) ? "ON" : "OFF";
	//stat["brightness"] = irrigacao->getIntensity (channel);

	String topic = "stat/ardn_01/valv_0";
	topic.concat (channel);
	topic.concat ("/RESULT");

	char valor[100];
	serializeJson (stat, valor, sizeof (valor));

/*
	String topic = "stat/ardn_01/valv_0"
	topic.concat (channel);
	String valor = irrigacao->getState (channel) ? "ON" : "OFF";
*/
	client.publish(topic.c_str(), valor);
}

void oficinaMqtt::loop() 
{
	static int c = 0;

	//chamado a cada 200ms
	client.loop();

	if (++c >= 25)  //a cada 5s vai refrescando o stat do mqtt
	{
		c = 0;
		for (int i = 1; i <= DIMMER_CHANNELS; i++)
			sendStatIluminacao (i); 
		for (int i = 1; i <= MAXCANAISIRRIGACAO; i++)
			sendStatIrrigacao (i); 
	}
}
