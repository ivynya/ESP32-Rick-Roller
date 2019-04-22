// Libraries for DNS and AP
#include <WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>

// Libraries for web server
#include <WebServer.h>
#include <SPIFFS.h>

// AP Settings
const char* ssid = "Free Trustworthy Wifi!!!";

// DNS Settings
const byte DNS_PORT = 53;
IPAddress apIP(69, 69, 69, 69);
DNSServer dnsServer;

// Start web server
WebServer server(80);

String getContentType(String filename) {
  if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".svg")) {
    return "image/svg";
  }
  return "text/plain";
}

bool exists(String path){
  bool yes = false;
  File file = SPIFFS.open(path, "r");
  if(!file.isDirectory()){
    yes = true;
  }
  file.close();
  return yes;
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.html";
  }
  String contentType = getContentType(path);
  if (exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
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

  // Redirected DNS queries may be sent here
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      handleFileRead("/");
    }
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
