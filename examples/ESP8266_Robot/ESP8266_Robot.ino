#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>   // include file system headers
#include <Hash.h>

#define HTTP_PORT   80
#define WS_PORT     81

#define REC_DELIM '\|'
#define DATA_DELIM '~'

const char* htmlfile = "/index.html";

// WiFi connection configuration
const char* ssid = "RBarbieri 2.4GHz"; // WiFi SSID
const char* password = "1F2A5B4C15";   // Wifi Password

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(HTTP_PORT);
WebSocketsServer webSocket = WebSocketsServer(WS_PORT);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      Serial.write(REC_DELIM);
      Serial.write(DATA_DELIM);
      Serial.write(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}

void handleRoot() {
  // redirect to html web page
  server.sendHeader("Location", "/index.html",true);
  server.send(302, "text/plane","");
}

void handleNotFound() {
  if(loadFromSpiffs(server.uri())) return;
  String message = "File not found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}

void setup() {
  Serial.begin(115200);

  Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("booting, please wait %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  //Initialize File System
  SPIFFS.begin();
  Serial.println("File system initialized");

  WiFiMulti.addAP(ssid, password);
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // start the mDNS responder for esp8266.local
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up mDNS responder");
  }
  Serial.println("mDNS responder started");

  // handle index
  server.on("/", handleRoot);

  // set setver all paths are not found so we can handle as per URI
  server.onNotFound(handleNotFound);

  // start HTTP server
  server.begin();
  Serial.println("HTTP server started");

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

void loop() {
  webSocket.loop();
  server.handleClient();
}

