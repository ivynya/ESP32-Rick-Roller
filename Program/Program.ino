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
    request->send(200);
}

void setup() {
  // Start AP with localIP, dnsIP, and gatewayIP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);

  // Start DNS server with * (will redirect all)
  dnsServer.start(DNS_PORT, "*", apIP);

  // Redirected DNS queries will get sent here
  server.onNotFound(onRequest);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
