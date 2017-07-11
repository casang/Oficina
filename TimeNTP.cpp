/*
 * TimeNTP.cpp
 * time sync to NTP time source
 *
 * This sketch uses the Ethernet library
 */
 
//#define DEBUG

#include <Time.h> 
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "TimeNtp.h"

time_t getNtpTimeFunc()
{
  if (ntp)
  {
    return ntp->getNtpTime ();
  }
  return NULL;
}

NTP::NTP() 
{
  prevDisplay = 0;
  Udp.begin(NTPLOCALPORT);
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Udp.begin(NTPLOCALPORT);
  Serial.println("waiting for sync");
}

void NTP::loop()
{  
#ifdef DEBUG
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }
#endif
}

void NTP::digitalClockDisplay(){
  // digital clock display of the time
#ifdef DEBUG
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
#endif
}

void NTP::printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
#ifdef DEBUG
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
#endif
}

time_t NTP::getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
#ifdef DEBUG
  Serial.println("Transmit NTP Request");
#endif
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
#ifdef DEBUG
      Serial.println("Receive NTP Response");
#endif
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
#ifdef DEBUG
  Serial.println("No NTP Response :-(");
#endif
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void NTP::sendNTPpacket(const IPAddress &address)
{
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
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

