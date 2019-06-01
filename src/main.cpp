#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

const char *ssid = "";
const char *password = "";

int D0Pin = 16;
int D0Value;

void handleRoot();
void updatePins();
void handleNotFound();
void connect()
{
  WiFi.begin(ssid, password); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED)
  { //Wait for connection

    delay(500);
    Serial.println("Waiting to connect...");

    //TODO trocar o while por um for
    //TODO se não conseguir conectar reiniciar o nodemcu
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Print the local IP
  //TODO Configurar IP fixo para o nodemc
  server.on("/", HTTP_GET, handleRoot);
  //server.on("/definePins", definePins);
  server.on("/updatePins", HTTP_POST, updatePins); //Associate the handler function to the path
  server.onNotFound(handleNotFound);
  server.begin(); //Start the server
  Serial.println("Server listening");
}

void setup()
{
  Serial.begin(9600);

  pinMode(D0Pin, OUTPUT);

  connect();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connect();
  }
  else
  {
    server.handleClient(); //Handling of incoming requests
  }
}

void updatePins()
{ //Handler for the body path

  if (server.hasArg("plain") == false)
  { //Check if body received
    Serial.println("plain false");
    server.send(200, "text/plain", "Body not received");
    return;
  }

  String json = server.arg("plain");

  const size_t capacity = JSON_OBJECT_SIZE(13) + 80;
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, json);

  if(error)
  {
    return;
  }
  //JsonObject &root = jsonBuffer.parseObject(payload);

  int D0 = doc["D0"]; // 1
  int D1 = doc["D1"]; // 1
  int D2 = doc["D2"]; // 1
  int D3 = doc["D3"]; // 1
  int D4 = doc["D4"]; // 1
  int D5 = doc["D5"]; // 1
  int D6 = doc["D6"]; // 1
  int D7 = doc["D7"]; // 1
  int D8 = doc["D8"]; // 1
  int RX = doc["RX"]; // 1
  int TX = doc["TX"]; // 1
  int S2 = doc["S2"]; // 1
  int S3 = doc["S3"]; // 1

  D0Value = D0;

  digitalWrite(D0Pin, D0Value);
  //digitalWrite(5, D1);

  if(D0Value == 1)
  {
    server.send(200, "text/plain", "1 Ok");
  }
  else
  {
    server.send(200, "text/plain", "0 Ok");

  }

  Serial.println("POST Ok");
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
  Serial.println("404 Ok");
}

void handleRoot()
{
  String message = "0";

  if(D0Value == 1)
  {
    message = "1";
  }

  server.send(200, "text/plain", message);
  Serial.println("GET Ok");
} 
