// Libraries for DNS and AP
#include <WiFi.h>
#include <DNSServer.h>

// Libraries for web server/captive portal site
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// AP Settings
const char* ssid = "Free Trustworthy Wifi!!!!";
const char* password = "password";

// DNS Settings
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

// Start web server
AsyncWebServer server(80);

void onRequest(AsyncWebServerRequest *request){
  request->send(200, "text/html", "<!DOCTYPE html><html><head><title>ESP32</title></head><body><h1>Big</h1><img src=\"rick.gif\"><video controls src=\"astley.mp4\" /></body></html>");
}

void setup() {
  // Start AP with localIP, dnsIP, and gatewayIP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  // Start DNS server with * (will redirect all)
  dnsServer.start(DNS_PORT, "*", apIP);

  // Redirected DNS queries will get sent here
  server.onNotFound(onRequest);
  
  // Route to load G O D M O D E
  server.on("/rick.gif", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/rick.gif");
  });
  server.on("/astley.mp4", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/astley.mp4");
  });
  
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
