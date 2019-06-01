#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

const char *ssid = "TP-LINK_1544";
const char *password = "52337260";

int D0Pin = 16;
int D0Value;

void updatePins()
{
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

  int D0 = doc["D0"];
  int D1 = doc["D1"];
  int D2 = doc["D2"];
  int D3 = doc["D3"];
  int D4 = doc["D4"];
  int D5 = doc["D5"];
  int D6 = doc["D6"];
  int D7 = doc["D7"];
  int D8 = doc["D8"];
  int RX = doc["RX"];
  int TX = doc["TX"];
  int S2 = doc["S2"];
  int S3 = doc["S3"];

  D0Value = D0;

  digitalWrite(D0Pin, D0Value);
  //digitalWrite(5, D1);

  server.send(200, "text/plain", String(D0Value));
  Serial.println("POST Ok");
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
  Serial.println("404 Ok");
}

void handleRoot()
{
    String message = "{\n\"D0\": " + String(digitalRead(D0)) + ",\n" +
                      "\"D1\": " + String(digitalRead(D1)) + ",\n" +
                      "\"D2\": " + String(digitalRead(D2)) + ",\n" +
                      "\"D3\": " + String(digitalRead(D3)) + ",\n" +
                      "\"D4\": " + String(digitalRead(D4)) + ",\n" +
                      "\"D5\": " + String(digitalRead(D5)) + ",\n" +
                      "\"D6\": " + String(digitalRead(D6)) + ",\n" +
                      "\"D7\": " + String(digitalRead(D7)) + ",\n" +
                      "\"D8\": " + String(digitalRead(D8)) + ",\n" +
                      "\"A0\": " + String(analogRead(A0)) + "\n" +
                     "}";

    server.send(200, "application/json", message);
    Serial.println("GET Ok");
} 

void connect()
{
  WiFi.begin(ssid, password); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED)
  { //Wait for connection

    delay(500);
    Serial.println("Waiting to connect...");

    //TODO trocar o while por um for
    //TODO se nao conseguir conectar reiniciar o nodemcu
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
    server.handleClient();
  }
}