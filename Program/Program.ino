// Libraries for DNS and AP
#include <WiFi.h>
#include <DNSServer.h>

// Libraries for web server/captive portal site
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// AP Settings
const char* ssid = "Free Public WiFi";

// DNS Settings
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

// Start web server
AsyncWebServer server(80);

// Non-persistent helper variables
int visitors = 0;
int totalSeconds = 0;
int avgSeconds = 0;
int timesInfoViewed = 0;

// Gets called on DNS redirect
void onRequest(AsyncWebServerRequest *request) {
  // Respond with 200 OK and HTML page
  request->send(SPIFFS, "/index.html", "text/html");
}

// Adds a visitor, recalculates average time spent
void incrementVisitorCount(int secondsSpent, bool scrolledDown) {
  visitors += 1;
  totalSeconds += secondsSpent;
  avgSeconds = totalSeconds / visitors;
  if (scrolledDown) {
    timesInfoViewed += 1;
  }

  savePersistentData();
  saveIndividualData(secondsSpent, scrolledDown);
}

// Save data from local variables to file (preserve upon reboot)
void savePersistentData() {
  File file = SPIFFS.open("/persistent.txt", FILE_WRITE);
  if (!file) { return; }
  file.print("{\"v\":");
  file.print(visitors);
  file.print(",\"s\":");
  file.print(avgSeconds);
  file.print(",\"i\":");
  file.print(timesInfoViewed);
  file.print("}");
  file.close();
}

void saveIndividualData(int seconds, bool scrolledDown) {
  File file = SPIFFS.open("/stats.txt", FILE_APPEND);
  if (!file) { return; }
  file.print("v: ");
  file.print(seconds);
  file.print(", ");
  file.println(scrolledDown);
  file.close();
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

  // Load saved persistent data
  File file = SPIFFS.open("/persistent.txt");
  if (!file) {
    Serial.println("An error has occured when loading persistent data");
    return;
  }
  String json;
  while (file.available()) {
    json += char(file.read());
  }
  // Log the data loaded
  Serial.println(json);

  // Parse saved persistent data
  StaticJsonDocument<54> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  visitors = doc["v"];
  avgSeconds = doc["s"];
  totalSeconds = visitors * avgSeconds;
  timesInfoViewed = doc["i"];
  file.close();

  // Redirected DNS queries will get sent here
  server.onNotFound(onRequest);
  // Redirected DNS queries may also be sent here
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html","text/html");
  });

  // Routes to load site content
  server.on("/css/site.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/site.css", "text/css");
  });
  server.on("/js/site.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/site.js", "text/js");
  });

  // Routes to load media content
  server.on("/media/loading.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/loading.svg");
  });
  server.on("/media/rick.gif", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/rick.gif", "image/gif");
  });

  // Routes to get/process data
  server.on("/persistent.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/persistent.txt", "text/plain");
  });
  server.on("/stats.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/stats.txt", "text/plain");
  });
  server.on("/addvisit", HTTP_ANY, [](AsyncWebServerRequest *request) {
    int s = atoi(request->getParam(0)->value().c_str());
    bool v = (atoi(request->getParam(1)->value().c_str()) == 1) ? true : false;
    incrementVisitorCount(s, v);
    request->send(200);
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
