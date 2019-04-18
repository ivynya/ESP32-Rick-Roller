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
IPAddress apIP(69, 69, 69, 69);
DNSServer dnsServer;

// Start web server
AsyncWebServer server(80);

// Gets called on DNS redirect
char* indexHTML = "<!DOCTYPE html><html><head> <title>ESP32 Web Server</title> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head><body> <h1>ESP32 Web Server</h1> <img src=\"rick.gif\"></body></html>";
void onRequest(AsyncWebServerRequest *request){
  // Respond with 200 OK and HTML page
  request->send(200, "text/html", indexHTML);
}

void setup() {
  // Start AP with localIP, dnsIP, and gatewayIP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
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
  // Redirected DNS queries may also be sent here
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", indexHTML);
  });

  // Routes to load media content
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css");
  });
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
