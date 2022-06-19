#include "WiFi.h"
#include "DHT.h"
#include <PubSubClient.h>
#include <TimeAlarms.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "SSID";
const char* password =  "PASSWORD";

const char* mqtt_server = "demo.thingsboard.io";
const char* device_token = "TOKEN";

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void conectar_wifi() {
  delay(500);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado ao AP");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

void reconectar() {
  while (!mqtt_client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      conectar_wifi();
    }
    Serial.print("Tentando conectar ao ThingsBoard via MQTT...   ");
    if (mqtt_client.connect("wemos", device_token, NULL)) {
      Serial.println("Conectado!");
    } 
    else {
      Serial.print("Falha, rc=");
      Serial.print(mqtt_client.state());
      Serial.println("Tentando novamente em 5s");
      delay(5000);
    }
  }
}

void dataCollect() {
  Serial.println("Coletando dados de temperatura e umidade... ");
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Falha ao ler do sensor DHT!"));
    return;
  }
  
  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" % | Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");
  String temperatura = String(t);
  String umidade = String(h);
  String payload = "{";
  payload += "\"temperatura\":"; payload += temperatura; payload += ",";
  payload += "\"umidade\":"; payload += umidade;
  payload += "}";
  char dados_json[100];
  payload.toCharArray(dados_json, 100);
  mqtt_client.publish( "v1/devices/me/telemetry", dados_json);
}
 
void setup() {
  Serial.begin(115200);
  conectar_wifi();
  dht.begin();
  mqtt_client.setServer(mqtt_server, 1883);
  Alarm.timerRepeat(300, dataCollect);
}

void loop() {
  if (!mqtt_client.connected()) {
    reconectar();
  }
  Alarm.delay(100);
  mqtt_client.loop();
}
