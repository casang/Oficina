#ifndef TimeNTP_H
#define TimeNTP_H

#define NTPLOCALPORT 8888  // local port to listen for UDP packets
#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message

// NTP Servers:
static const IPAddress timeServer(200, 160, 0, 8); // a.ntp.br
//IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
//IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
//IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov


extern time_t getNtpTimeFunc();

class NTP
{
	static const int timeZone = -2;      // Brasilia - BR
	//const int timeZone = 1;     // Central European Time
	//const int timeZone = -5;  // Eastern Standard Time (USA)
	//const int timeZone = -4;  // Eastern Daylight Time (USA)
	//const int timeZone = -8;  // Pacific Standard Time (USA)
	//const int timeZone = -7;  // Pacific Daylight Time (USA)

  time_t prevDisplay; // when the digital clock was displayed
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
  EthernetUDP Udp;
  
  void printDigits(int digits);
  void digitalClockDisplay();
  void sendNTPpacket(const IPAddress &address);
  
public:
  NTP();
  void loop();
  time_t getNtpTime();
};

extern NTP *ntp;
#endif//
