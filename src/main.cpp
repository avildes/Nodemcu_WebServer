#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

const char *ssid = "TP-LINK_1544";
const char *password = "52337260";

enum pMode
{
  mINPUT = 0,
  mOUTPUT = 1,
  mINPUT_PULLUP = 2
};

void sendSuccessMessage(String message)
{
  server.send(200, "text/plain", message);
  Serial.println("Code 200 sent with message: " + message);
}

uint8_t convertPinMode(pMode mode)
{
    uint8_t response;

    switch (mode)
    {
        case mINPUT:
          response = INPUT;
          break;
        case mOUTPUT:
          response = OUTPUT;
          break;
        case mINPUT_PULLUP:
          response = INPUT_PULLUP;
          break;
        
        default:
          response = INPUT;
    }
    return response;
}

boolean hasBody()
{
  boolean hasBody = server.hasArg("plain");

  if(!hasBody)  //Check if body received
  {
    Serial.println("plain false");
    server.send(200, "text/plain", "Body not received");
  }

    return hasBody;
}

void updatePin()
{
  if(!hasBody())
  {
    return;
  }

  String json = server.arg("plain");

  const size_t capacity = JSON_OBJECT_SIZE(2) + 80;
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    return;
  }

  int pin = doc["pin"];
  int value = doc["value"];

  digitalWrite(pin, value);

  sendSuccessMessage("Pin: " + String(pin) + "\n Value: "+  String(value));
}

void updatePinMode()
{
  if(!hasBody())
  {
    return;
  }

  String json = server.arg("plain");

  const size_t capacity = JSON_OBJECT_SIZE(2) + 80;
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    return;
  }

  int pin = doc["pin"];
  int value = doc["pinMode"];

  pMode mode = static_cast<pMode>(value);

  digitalWrite(pin, convertPinMode(mode));

  sendSuccessMessage("Pin "+ String(pin) + " was set to " + String(mode));
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
  Serial.println("404 Error");
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

  sendSuccessMessage(message);
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
  
  //TODO Configurar IP fixo para o nodemcu
  server.on("/", HTTP_GET, handleRoot);
  server.on("/updatePin", HTTP_POST, updatePin);
  server.on("/updatePinMode", HTTP_POST, updatePinMode);
  server.onNotFound(handleNotFound);

  server.begin(); //Start the server
  Serial.println("Server listening");
}

void setup()
{
  Serial.begin(9600);

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

  //TODO Prever a opção de fazer com que o nodemcu mande um post para uma determinada url quando o estado de algum pino mudar
}