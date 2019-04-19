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
char* indexHTML = "<!dOcTyPe html><html><head> <title>Network Login</title> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"stylesheet\" type=\"text/css\" href=\"css/site.css\"> <link rel=\"stylesheet\" type=\"text/css\" href=\"css/animate.css\"> <script src=\"js/site.js\"></script></head><body> <div class=\"loading\"> <img src=\"media/loading.gif\" alt=\"[Loading Image]\"> <p id=\"loading-text\">Initializing service</p></div><img id=\"rick\" src=\"media/rick.gif\"> <div class=\"content\"> <h1></h1> <p></p></div></body></html>";
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

  // Routes to load dependencies
  server.on("/css/animate.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/animate.css");
  });

  // Routes to load site content
  server.on("/css/site.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/site.css");
  });
  server.on("/js/site.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/site.js");
  });

  // Routes to load media content
  server.on("/media/loading.gif", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/media/loading.gif");
  });
  server.on("/media/rick.gif", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/media/rick.gif");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
