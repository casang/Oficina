//#define DEBUG

#include <arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#ifdef DEBUG
//#include <Serial.h>
#endif
#include "Rede.h"
#include "Solenoide.h"
#include "LevelSensor.h"
#include "Irrigacao.h"
#include "Dimmer.h"
#include "Interruptor.h"
#include "iluminacao.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
unsigned char mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,1,177);
//IPAddress ip(192,168,0,177);

bool Rede::start ()
{
  Ethernet.begin(mac, ip);
  if (ip == Ethernet.localIP())
    active = true;
  else
    active = false;
  if (active)
  {
    //Ethernet.begin (mac);
    server = new EthernetServer (8088);
    // start the Ethernet connection and the server:
    server->begin();
  #ifdef DEBUG
//    Serial.print("server is at ");
//    Serial.println(Ethernet.localIP());
  #endif
    startNTP ();
  }
  return active;
}

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
Rede::Rede ()
{
  active = false;
  start ();
}

//012345
//C1=12& onde C  é o comando, 1 o canal, 12 o valor
//Comandos:
//A: set dimmer intensity
//B: set dimmer state
//C: get dimmer intensity
//D: get dimmer state
//E: set movement sensor mode
//H: set sprinkler time
//I: set sprinkler state
//J: get sprinkler time
//K: get sprinkler state
//

void Rede::procCmd (char strResp[])
{
#ifdef DEBUG
  Serial.print("Cmd=");
  Serial.print(cmd);
  Serial.print("channelCmd=");
  Serial.print(channelCmd);
  Serial.print("Value=");
  Serial.println(valueCmd);
#endif
  switch (cmd)
  {
    case 'A':
      if (iluminacao)
      {
        if (channelCmd >= 0 && channelCmd <= DIMMER_CHANNELS)
        {
          iluminacao->setIntensity (channelCmd, valueCmd);
          if (strResp)
            strcpy (strResp, "1");
        }
        iluminacao->interruptor->autoTurnOff (false);
        if (!iluminacao->isOn () && iluminacao->interruptor)
          iluminacao->interruptor->reset (); // se apagou tudo reset no interruptor
      }
      break;
    case 'B':
      if (iluminacao)
      {
        if (channelCmd >= 0 && channelCmd <= DIMMER_CHANNELS)
        {
          iluminacao->setState (channelCmd, valueCmd);
          if (strResp)
            strcpy (strResp, "1");
        }
        iluminacao->interruptor->autoTurnOff (false);
        if (!iluminacao->isOn () && iluminacao->interruptor)
          iluminacao->interruptor->reset (); // se apagou tudo reset no interruptor
      }
      break;
    case 'C':
      if (iluminacao)
      {
        if (channelCmd >= 0 && channelCmd <= DIMMER_CHANNELS)
          if (channelCmd == 0)
            sprintf (strResp, "C1=%03d&C2=%03d&C3=%03d&C4=%03d&C5=%03d", 
              iluminacao->getIntensity (1),
              iluminacao->getIntensity (2),
              iluminacao->getIntensity (3),
              iluminacao->getIntensity (4),
              iluminacao->getIntensity (5));
          else
            sprintf (strResp, "C%d=%03d", 
              channelCmd,  
              iluminacao->getIntensity (channelCmd));
      }
      break;
    case 'D':
      if (iluminacao)
      {
        if (channelCmd >= 0 && channelCmd <= DIMMER_CHANNELS)
          if (channelCmd == 0)
            sprintf (strResp, "D1=%d&D2=%d&D3=%d&D4=%d&D5=%d", 
              iluminacao->getState (1) ? 1 : 0,
              iluminacao->getState (2) ? 1 : 0,
              iluminacao->getState (3) ? 1 : 0,
              iluminacao->getState (4) ? 1 : 0,
              iluminacao->getState (5) ? 1 : 0);
          else
            sprintf (strResp, "D%d=%d", 
              iluminacao->getState (channelCmd) ? "1" : "0");
      }
      break;
    case 'E':
      iluminacao->interruptor->setMoveAction (valueCmd);
      if (strResp)
        strcpy (strResp, "1");
      break;
    case 'H':
      if (channelCmd >= 1 && channelCmd <= MAXCANAISIRRIGACAO)
      {
          if (irrigacao)
          {
            irrigacao->setTimer (channelCmd, valueCmd);
            if (strResp)
              strcpy (strResp, "1");
          }
      }
      break;
    case 'I':
      if (channelCmd >= 1 && channelCmd <= MAXCANAISIRRIGACAO)
      {
          if (irrigacao)
          {
            irrigacao->setState (channelCmd, valueCmd);
            if (strResp)
              strcpy (strResp, "1");
          }
      }
      break;
    case 'J':
      if (channelCmd >= 0 && channelCmd <= MAXCANAISIRRIGACAO)
      {
        if (channelCmd == 0)
          sprintf (strResp, "J1=%03d&J2=%03d&J3=%03d", 
              irrigacao->getTimer (1),
              irrigacao->getTimer (2),
              irrigacao->getTimer (3));
        else
          sprintf (strResp, "J%d=%03d", 
              channelCmd, irrigacao->getTimer (channelCmd));
      }
      break;
    case 'K':
      if (channelCmd >= 0 && channelCmd <= MAXCANAISIRRIGACAO)
      {
        if (channelCmd == 0)
          sprintf (strResp, "K1=%d&K2=%d&K3=%d", 
              irrigacao->getState (1) ? 1 : 0,
              irrigacao->getState (2) ? 1 : 0,
              irrigacao->getState (3) ? 1 : 0);
        else
          sprintf (strResp, "K%d=%d", 
              channelCmd, irrigacao->getState (channelCmd) ? "1" : "0");
      }
      break;
  /*        case 'i':
      if (channelCmd >= 1 && channelCmd <= MAXCANAISIRRIGACAO) // termino automatico
      {
        if (irrigacao)
          irrigacao->turnOn (channelCmd, valueCmd * 60);
        break;
      }
      if (channelCmd == 0) // liga todos, com desligamento automatico
      {
        if (irrigacao)
        {
          for (int i = 1; i < MAXCANAISIRRIGACAO + 1; i++)
            irrigacao->turnOn (i, valueCmd * 60);
          if (valueCmd == 0)
            irrigacao->turnOn (0, 0);
        }
        break;
      }
      break;
    case 'B':
      if (channelCmd == 1)
      {
        switch (valueCmd)
        {
          case 0:
            if (irrigacao)
              irrigacao->turnOff (0);
            break;
          case 1:
            if (irrigacao)
              irrigacao->turnOn (0, -1);
            break;
        }
      }
      break;
    case 'b':
      if (channelCmd == 1) // termino automatico
      {
        if (irrigacao)
          irrigacao->turnOn (0, valueCmd * 60);
        break;
      }
      break;*/
    case 'S':
      if (timeStatus () == timeSet)
      {
        time_t t = now ();
        sprintf (strResp, "%02d/%02d/%02d %02d:%02d:%02d",
            day(t), month(t), year(t), hour(t), minute(t), second(t));
      }  
      break;
    default:  //invalid parameter
      return;
  }
}

void Rede::parseURL (char c, char strResp[], bool reset)
{
  static char inicio[] = "GET /?";  

  if (reset)
  {
    stateParseURL = 0;
    posStrLine = 0;
  }

  switch (stateParseURL)
  {
    case 0:
      if (c != inicio[posStrLine])
      {
        posStrLine = 0;
      }
      if (!inicio[++posStrLine])
        stateParseURL++;  // achou o HEADER
      break;
    
    case 1:
      cmd = c;
      stateParseURL++;
      strcpy (strResp, "-1"); //código de erro por default
      break;

    case 2:
      channelCmd = (int)(c - '0');
      stateParseURL++;
      break;
   
    case 3:
      if (c == '=')
      {
        stateParseURL++;
        break;
      }
      else
      {
        procCmd (strResp);        
        if (c == '&')
        {
          stateParseURL = 1;
        }
      }
      posStrLine = 0;
      stateParseURL = 0;
      break;

    case 4:
      valueCmd = c - '0';
      stateParseURL++;
      break;

    case 5:
    case 6:
      if ((c >= '0') && (c <= '9'))
      {
        valueCmd = valueCmd * 10 + (c - '0');
        stateParseURL++;
      }
      else
      {
        if (c == '&')
        {
          stateParseURL = 1;
        }
        else
        {
          posStrLine = 0;
          stateParseURL = 0;
        }
      }

      procCmd (strResp);
  }
}

void Rede::ntpLoop ()
{
  static int cont = 0;

  if (!active)
  {
    if (!start ())
      return;
  }
  if (cont >= 3600)
  {
    cont = 0;
    // executado mais ou menos a cada hora
    getNTP ();
  }
  else
  {
    if (!(cont % 60))
    {
      // executado mais ou menos a cada minuto
      if (timeStatus () != timeSet)
      {

        getNTP ();
      }
    }
  }

  if (estadoNTP > 0)
    getNTP ();
}

#define DEBUG1

void Rede::loop() 
{
  static int cont = 0;
  char strResp[100];

  if (!active)
  {
    if (!start ())
      return;
  }

  int tOut = 0;
  // listen for incoming clients
  EthernetClient client = server->available();
  if (client) {
#ifdef DEBUG
//    Serial.println("new client");
#endif
    // an http request ends with a blank line
    int state = 0;
    tOut = 0;
    parseURL (' ', strResp, true);
    while (client.connected()) 
    {
      if (tOut++ > 10)
        break;  // muito tempo connected sem caracter available
#ifdef DEBUG
//Serial.print ("-");
#endif
      if (client.available()) 
      {
        tOut = 0;
        char c = client.read();
#ifdef DEBUG
//        Serial.write(c);
#endif
        switch (state)
        {
          case 0:
            parseURL (c, strResp);
            if (c == '\n')
            {
              state = 1;
            }
            break;

          case 1:
            if ((c != '\r') && (c != '\n'))
            {
              state = 0;
              parseURL (c, strResp, true);
              break;
            }
            if (c == '\n')
            {
              state = 2;
              // send a standard http response header
              //client.println("HTTP/1.1 200 OK");
              //client.println("Content-Type: text/html");
              //client.println("Connection: close");  // the connection will be closed after completion of the response
    //          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              //client.println();
              //client.println("<!DOCTYPE HTML>");
              //client.println("<html>");
              //client.println("cmd=");
              //client.println(cmd);
              //client.println("channelCmd=");
              //client.println(channelCmd);
              //client.println("valueCmd=");
              //client.println(valueCmd);
              client.println(strResp);
#ifdef DEBUG1
              Serial.print(strResp);
#endif              
              ////client.println("teste");     
              //client.println("</html>");
              break;
            }
            break;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
#ifdef DEBUG
    Serial.println("client disconnected");
#endif
  }
}

// A UDP instance to let us send and receive packets over UDP

void Rede::startNTP () 
{
  localPort = 8888;      // local port to listen for UDP packets
  timeServer = new IPAddress (200, 160, 7, 186);  //200.160.7.186
  Udp = new EthernetUDP ();
  Udp->begin(localPort);
  estadoNTP = 0;
  NTPTimer = 0;
}

void Rede::getNTP ()
{
  if (!Udp)
    return;

  switch (estadoNTP)
  {
    case 0: // 
      sendNTPpacket(*timeServer); // send an NTP packet to a time server
      estadoNTP++;
      NTPTimer = 0;
      break;

    case 1:  
      // wait to see if a reply is available
      if ( !Udp->parsePacket() )
      {
        //checa timeout 
        if (NTPTimer++ > NTPTimeOut)
          estadoNTP = 0;
      }
      else 
      {  
      // We've received a packet, read the data from it
        Udp->read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        //secsSince1900 -= (6048000L+10800L); // acerta fuso de -3h e leva para 1900
        secsSince1900 -= 10800L; // acerta fuso de -3h e leva para 1900
        setTime (secsSince1900 - 2208988800L); // setTime is since 1970 
      }
      break;
    }
}

// send an NTP request to the time server at the given address 
void Rede::sendNTPpacket(IPAddress& address)
{
  if (!Udp)
    return;
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:       
  Udp->beginPacket(address, 123); //NTP requests are to port 123
  Udp->write(packetBuffer,NTP_PACKET_SIZE);
  Udp->endPacket(); 
}









