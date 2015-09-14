#include <SPI.h>
#include <Adafruit_CC3000.h>
#include <Adafruit_CC3000_Server.h>
#include <ccspi.h>
#include <string.h>
#include "utility/debug.h"

#define WiDo_IRQ  7
#define WiDo_VBAT 5
#define WiDo_CS 10

Adafruit_CC3000 WiDo = Adafruit_CC3000(WiDo_CS, WiDo_IRQ, WiDo_VBAT, SPI_CLOCK_DIVIDER);

#define WLAN_SSID "TPLINK-50CE50RT"
#define WLAN_PASS "13656676494"

#define WLAN_SECURITY WLAN_SEC_WPA2
#define TIMEOUT_MS  3000

#define TOKEN "gsdfg"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println(F("Hello, Wido!\n"));

  displayDriverMode();
  // Measure the free Ram
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC); 

  Serial.println(F("\nInitialising the CC3000..."));
  if (!WiDo.begin()) {
    Serial.println(F("Unable to initialise the CC3000!"));
    while(1);
  }

  listSSIDResults();

//  Serial.println(F("\nDeleting old connection profiles"));
//  if (!WiDo.deleteProfiles()) {
//    Serial.println(F("Failed!"));
//    while(1);
//  }

  char *ssid = WLAN_SSID;
  Serial.print(F("\nAttempting to connect to ")); 
  Serial.println(ssid);
  if (!WiDo.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }

  Serial.println(F("Connected!"));
  Serial.println(F("Request DHCP"));
  while(!WiDo.checkDHCP()) {
    delay(100);
  }
  
  while (! displayConnectionDetails()) {
    delay(1000);
  }
  
  Serial.println(F("Initialising end"));
}

void loop() {
  // put your main code here, to run repeatedly:
  static Adafruit_CC3000_Client tcpClient;
  
//  if(tcpClient.connected()) {
//    Serial.println("start...");
//    
//    char clientString[70];
////    int sensValue = 100;
////    sprintf(clientString,"%s%s%s%d%s","GET /data?token=",TOKEN,"&param=",sensValue," HTTP/1.1");
////    Serial.println(clientString);
//    sprintf(clientString, "%s=%s", "token", TOKEN);
//    Serial.println(clientString);
//
//    tcpClient.fastrprintln(clientString);
//
//    Serial.println("============");
//
//    unsigned long lastRead = millis();
//    while(tcpClient.connected() && (millis() - lastRead < TIMEOUT_MS)) {
//      while(tcpClient.available()) {
//        char c = tcpClient.read();
//        Serial.print(c);
//        lastRead = millis();
//      }
//    }
//    
////    tcpClient.close();
//    delay(1000);
//  }
//  else {
//    delay(1000);
//    Serial.println("Connecting to Server...");
//    uint32_t ip = WiDo.IP2U32(192,168,0,109);
//    tcpClient = WiDo.connectTCP(ip, 5000);
//    
//  }


  Serial.println("Connecting to Server...");
  uint32_t ip = WiDo.IP2U32(192,168,0,109);
  tcpClient = WiDo.connectTCP(ip, 5000);
  delay(1000);

  if(tcpClient.connected()) {
    Serial.println("connected...");
    
    char clientString[70];
//    int sensValue = 100;
//    sprintf(clientString,"%s%s%s%d%s","GET /data?token=",TOKEN,"&param=",sensValue," HTTP/1.1");
//    Serial.println(clientString);
    sprintf(clientString, "%s=%s", "token", TOKEN);
    Serial.println(clientString);

    tcpClient.fastrprintln(clientString);

    Serial.println("============");

    unsigned long lastRead = millis();
    String command;
    while(tcpClient.connected() && (millis() - lastRead < TIMEOUT_MS)) {
      while(tcpClient.available()) {
        char c = tcpClient.read();
        Serial.print(c);
        command += char(c);
//        if(c == '*') {
//          while(c != '#' && tcpClient.available()) {
//            command += char(c);
//            char c = tcpClient.read();
//            Serial.print(c);
//            delay(2);
//          }
//        }
        lastRead = millis();
      }
    }
    Serial.print("command: ");
    Serial.println(command);

    int commandId = getCommandId(command);
    Serial.print("commandId:");
    Serial.println(commandId);
    
//    tcpClient.close();
    delay(1000);
  }
    
tcpClient.close();
  
}

int getCommandId(String command)
{
  int commandId = 0;
  int index = 0;
  while(command[index] != '#')
  {
    if(command[index] >= '0' && command[index] <= '9')
    {
//      Serial.print(command[index]);
      commandId = commandId * 10 + command[index] - '0';
    }
    index++;
  }
  return commandId;
}

void displayDriverMode(void)
{
  #ifdef CC3000_TINY_DRIVER
    Serial.println(F("CC3000 is configure in 'Tiny' mode"));
  #else
    Serial.print(F("RX Buffer : "));
    Serial.print(CC3000_RX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
    Serial.print(F("TX Buffer : "));
    Serial.print(CC3000_TX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
  #endif
}


/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!WiDo.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); WiDo.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); WiDo.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); WiDo.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); WiDo.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); WiDo.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = WiDo.startSSIDscan();

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = WiDo.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  WiDo.stopSSIDscan();
}

