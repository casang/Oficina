//#define DEBUG

#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

class Rede
{
  EthernetServer *server;
  // A UDP instance to let us send and receive packets over UDP
  EthernetUDP *Udp;
  unsigned int localPort;      // local port to listen for UDP packets
  IPAddress *timeServer;
  byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

  int posStrLine;
  char cmd;
  int channelCmd;
  int valueCmd;
  int stateParseURL;
  bool active;
  int estadoNTP;
  int NTPTimer;
  int NTPTimeOut;

public:
  Rede ();
  void loop ();
  bool start ();
  void parseURL (char c, char strResp[], bool reset = false);
  void procCmd (char strResp[]);
  void getNTP ();
  void startNTP ();
  void sendNTPpacket(IPAddress& address);
};
