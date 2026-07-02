
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#ifndef STASSID
#define STASSID "HotspotName"
#define STAPSK "HotspotPassword"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

// assign traffic-light GPIOs
const int redPin    = 4;  // GPIO2
const int yellowPin = 2;  // GPIO4
const int greenPin  = 0;  // GPIO0
// ALL CONNECTED TO 3.3V SUPPLY

void handleRoot() {
  digitalWrite(redPin, 1);
  digitalWrite(yellowPin, 1);
  digitalWrite(greenPin, 1);
  File file = SPIFFS.open("/traffic_website.html","r");
    if (file) {
      server.streamFile(file,"text/html");
      file.close();
    } else {
      server.send(404, "text/plain","File not found");
    }
  }

void handleRedOn() {
  digitalWrite(redPin, 0);
  server.send(200, "text/plain", "red on");
}

void handleYellowOn() {
  digitalWrite(yellowPin, 0);
  server.send(200, "text/plain", "yellow on");
}

void handleGreenOn() {
  digitalWrite(greenPin, 0);
  server.send(200, "text/plain", "green on");
}

void handleRedOff() {
  digitalWrite(redPin, 1);
  server.send(200, "text/plain", "red off");
}

void handleYellowOff() {
  digitalWrite(yellowPin, 1);
  server.send(200, "text/plain", "yellow off");
}

void handleGreenOff() {
  digitalWrite(greenPin, 1);
  server.send(200, "text/plain", "green off");
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);

}

void setup(void) {

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount FS");
    return;
  }
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }
  
  
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin,OUTPUT);
  pinMode(greenPin, OUTPUT);
  digitalWrite(redPin, 1);
  digitalWrite(yellowPin,1);
  digitalWrite(greenPin, 1);


  server.on("/", handleRoot);
  server.on("/redLEDon",handleRedOn); 
  server.on("/yellowLEDon",handleYellowOn); 
  server.on("/greenLEDon", handleGreenOn);
  server.on("/redLEDoff",handleRedOff); 
  server.on("/yellowLEDoff",handleYellowOff); 
  server.on("/greenLEDoff", handleGreenOff);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
