#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "settings.h"

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASSWORD;

char serverAdress[] = "192.168.1.92";
int port = 8080;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAdress, port);

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();

  Serial.println("Ansluter till WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }

  Serial.println("Ansluten till WiFi!");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Adress: ");
  Serial.println(ip);
}

void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  Serial.print("Temperatur: ");
  Serial.print(tempC);
  Serial.println(" °C");

  String postData = "{\"celcius\":" + String(tempC, 2) + "}";

  Serial.println("Skickar POST");

  client.beginRequest();
  client.post("/temperature");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", postData.length());

  client.beginBody();
  client.print(postData);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  delay(1800000);
}
