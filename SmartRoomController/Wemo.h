#include <Ethernet.h>

class Wemo {

int wemoPort = 49153;
//old code. get errors if not using const and array length.
const char* wemoIP[4] = {"192.168.1.13","192.168.1.7","192.168.1.8","192.168.1.6"};
EthernetClient WemoClient;

public:
//seems cannot use empty constructor or get error that Wemo::Wemo undefined.
//  Wemo();
  
  void switchON(int device) {
    
    String data1;
    
    Serial.printf("SwitchON %i \n",device);
    data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
    if (WemoClient.connect(wemoIP[device],wemoPort)) {
          WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
          WemoClient.println("Content-Type: text/xml; charset=utf-8");
          WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
          WemoClient.println("Connection: keep-alive");
          WemoClient.print("Content-Length: ");
          WemoClient.println(data1.length());
          WemoClient.println();
          WemoClient.print(data1);
          WemoClient.println();
      }
  
    if (WemoClient.connected()) {
       WemoClient.stop();
    }
  }

  void switchOFF(int device){
    String data1;
    
    Serial.printf("switchOFF %i \n",device);
    data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>0</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
    if (WemoClient.connect(wemoIP[device],wemoPort)) {
          WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
          WemoClient.println("Content-Type: text/xml; charset=utf-8");
          WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
          WemoClient.println("Connection: keep-alive");
          WemoClient.print("Content-Length: ");
          WemoClient.println(data1.length());
          WemoClient.println();
          WemoClient.print(data1);
          WemoClient.println();
      }
     
    if (WemoClient.connected()) {
       WemoClient.stop();
    }
  }
};
