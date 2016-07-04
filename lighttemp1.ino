#include <EtherCard.h>
#include <DHT.h>

#define LED1PIN 2
#define LED2PIN 3

static byte mymac[] = {0x74,0x69,0x69,0x2D,0x30,0x31};
static byte myip[] = {192,168,1,20};
byte Ethernet::buffer[900];

#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11

word   rh, rt;      // make global

DHT dht(DHTPIN, DHTTYPE);

boolean led1Status;
boolean led2Status;


void Temp(){
  rh = dht.readHumidity();
  rt = dht.readTemperature();
}
void setup () {
 
  Serial.begin(57600);
  Serial.println("WebLed Demo");
 
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 53))
    Serial.println( "Failed to access Ethernet controller");
 else
   Serial.println("Ethernet controller initialized");
 
  if (!ether.staticSetup(myip))
    Serial.println("Failed to set IP address");

  Serial.println();
  
  pinMode(LED1PIN, OUTPUT);
  pinMode(LED2PIN, OUTPUT);
  
  digitalWrite(LED1PIN, LOW);
  digitalWrite(LED2PIN, LOW);
  
 
  led1Status = false;
  led2Status = false;
  

  dht.begin();
}
  
void loop() {
   
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if(pos) {

    if(strstr((char *)Ethernet::buffer + pos, "GET /?LED1_on") != 0) {
        led1Status = true;
      }
    if(strstr((char *)Ethernet::buffer + pos, "GET /?LED1_off") != 0) {
        led1Status = false;
      }

      if(led1Status) {
      digitalWrite(LED1PIN, HIGH);
    } else {
      digitalWrite(LED1PIN, LOW);
    }
      
     if(strstr((char *)Ethernet::buffer + pos, "GET /?LED2_on") != 0) {
        led2Status = true;                 
      }
       if(strstr((char *)Ethernet::buffer + pos, "GET /?LED2_off") != 0) {
        led2Status = false;                 
      }
     if(led2Status) {
      digitalWrite(LED2PIN, HIGH);
    } else {
      digitalWrite(LED2PIN, LOW);
    }

 

    if(strstr((char *)Ethernet::buffer + pos, "GET /?Temp_on") != 0) {
             Temp();
    }   
  
      
    BufferFiller bfill = ether.tcpOffset();
      bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\nPragma: no-cache\r\n\r\n"
        "<html><head><title>Arduino Smart Home</title></head><body bgcolor=(#ADDFFF)><center>"
        "<h1>ARDUINO SMART HOME</h1> <br><br>"
      
      "<a href=\"/?LED1_on\"><input type=\"button\" value=\"Fan_on\"></a>"
      "<a href=\"/?LED1_off\"><input type=\"button\" value=\"Fan_off\"></a><br><br>"
      
      "<a href=\"/?LED2_on\"><input type=\"button\" value=\"Light_on\"></a>"
      "<a href=\"/?LED2_off\"><input type=\"button\" value=\"Light_off\"></a><br><br>"  ));    

  
      bfill.emit_p(PSTR("<a href=\"/?Temp_on\"><input type=\"button\" value=\"Temp_on\"></a><br><br>"
      "<h2>Temperature : </h2>"
      "<input type=\"text\" value=\"$D\">"),rt);
      bfill.emit_p(PSTR("<h2>Humidity : </h2>"
      "<input type=\"text\" value=\"$D\">"),rh);    
      bfill.emit_p(PSTR("</center></body></html>"));

      
      ether.httpServerReply(bfill.position());
  }
}


