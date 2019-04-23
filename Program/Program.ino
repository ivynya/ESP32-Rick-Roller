// Libraries for DNS and AP
#include <WiFi.h>
#include <DNSServer.h>

// Libraries for web server/captive portal site
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// AP Settings
const char* ssid = "Free Trustworthy Wifi!!!!";

// DNS Settings
const byte DNS_PORT = 53;
IPAddress apIP(69, 69, 69, 69);
DNSServer dnsServer;

// Start web server
AsyncWebServer server(80);

// Gets called on DNS redirect
char* indexHTML = "<!dOcTyPe html><html><head><title>Network Login</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"stylesheet\" type=\"text/css\" href=\"css/site.css\"><link rel=\"stylesheet\" type=\"text/css\" href=\"css/animate.css\"><link rel=\"stylesheet\" type=\"text/css\" href=\"css/loading.css\"><script src=\"js/site.js\"></script><script src=\"js/loading.min.js\"></script></head><body><div class=\"loading\"><div class=\"ldBar\"data-type=\"fill\"data-fill-dir=\"ltr\"data-img=\"media/loading.svg\"></div><p id=\"loading-text\">Initializing service</p></div><img id=\"rick\" src=\"media/rick.gif\" /><div class=\"overlay\"><p id=\"message\">Gottem.</p><p id=\"notification\">Scroll for more details.</p></div><div class=\"contentBlock\"><div class=\"content\"><h1>\"ARE YOU STEALING MY DATA?!\"</h1><hr><div class=\"half right\"><p>Rest assured, your personally identifiable data is not being stolen by us, and this site and WiFi network operate purely off of principles that you would find daily as you browse the web with no shady business like IP or location logging, and nothing to do with advertising on this site. If you are interested in how this site and WiFi network function, you are welcome to contact us with the information provided at the bottom of this page. In terms of data collected, we simply log your visit onto this page and how long you stay, for our wonderous statistics page you can find below! If you choose to leave a reaction below, that will also get saved, obviously. No other information (such as background network tasks, etc.) get taken. This site does not use cookies.</p></div><div class=\"half left\"><div class=\"feature\"><h1>No</h1><h2>Personal Information Taken</h2></div><div class=\"feature\"><h1>100%</h1><h2>Anonymized Statistics</h2></div><div class=\"feature\"><h1>0</h1><h2>Cookies Used</h2></div></div><br><h1>Statistics!</h1><hr><div class=\"feature\"><h1 id=\"target\">700</h1><h2>People Rick Rolled</h2></div><div class=\"feature\"><h1>3:36</h1><h2>Average Time on Site</h2></div><div class=\"feature\"><h1>200</h1><h2>Who Scrolled Down</h2></div></div></div></body></html>";
void onRequest(AsyncWebServerRequest *request){
  // Respond with 200 OK and HTML page
  request->send(SPIFFS, "/index.html", "text/html");
}

void setup() {
  Serial.begin(115200);

  // Start AP with localIP, dnsIP, and gatewayIP
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(ssid);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  // Start DNS server with * (will redirect all)
  if(!dnsServer.start(DNS_PORT, "*", apIP)) {
    Serial.println("An error has occurred while starting DNS");
    return;
  }

  // Redirected DNS queries will get sent here
  server.onNotFound(onRequest);
  // Redirected DNS queries may also be sent here
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html","text/html");
  });

  // Routes to load dependencies
  server.on("/css/animate.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/animate.css", "text/css");
  });
  server.on("/css/loading.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/loading.css", "text/css");
  });
  server.on("/js/loading.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/loading.min.js", "text/js");
  });

  // Routes to load site content
  server.on("/css/site.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/site.css", "text/css");
  });
  server.on("/js/site.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/site.js", "text/js");
  });

  // Routes to load media content
  server.on("/media/loading.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/media/loading.svg");
  });
  server.on("/media/rick.gif", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/media/rick.gif", "image/gif");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
