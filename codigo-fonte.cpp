#include <ESP32Servo.h>
#include <DHTesp.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Configurações de WiFi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// Configurações de MQTT
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "E_mqtt";
const char *TOPIC_PUBLISH_DATA = "fiapers/dados";

// Define o servo motor
Servo servoMotor;

// Define as especificações do DHT
#define DHTPIN 15
DHTesp dht;

// Variáveis globais
WiFiClient espClient;
PubSubClient MQTT(espClient);

unsigned long publishUpdate = 0;
const int PUBLISH_DELAY = 500;
const int TAMANHO = 200;

// Variáveis de entrada
const int servoPin = 32;
const int potPin = 33;

const int buzzerBat = 16;
const int buzzerTemp = 17;

// Variáveis de nível
String nivelBat = "";
String nivelTemp = "";
float nivelAgua = 100.0; // Supondo 100% de hidratação no início

// Protótipos de funções
void initWiFi();
void initMQTT();
void callbackMQTT(char *topic, byte *payload, unsigned int length);
void reconnectMQTT();
void reconnectWiFi();
void checkWiFIAndMQTT();

void initWiFi() {
  Serial.print("Conectando com a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(callbackMQTT);
}

void callbackMQTT(char *topic, byte *payload, unsigned int length) {
  String msg = String((char*)payload).substring(0, length);
  Serial.printf("Mensagem recebida via MQTT: %s do tópico: %s\n", msg.c_str(), topic);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar com o Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.println("Falha na conexão com MQTT. Tentando novamente em 2 segundos.");
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Wifi conectado com sucesso");
  Serial.print(SSID);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  dht.setup(DHTPIN, DHTesp::DHT22);

  servoMotor.attach(servoPin);

  pinMode(buzzerBat, OUTPUT);
  pinMode(buzzerTemp, OUTPUT);

  initWiFi();
  initMQTT();

  Serial.begin(115200);  // Ajustado para a taxa padrão da ESP32
}

void loop() {

  checkWiFIAndMQTT();
  MQTT.loop();

  int potValue = analogRead(potPin);
  int batimentoCard = map(potValue, 0, 4095, 60, 200);
  int batimentointervalo = map(batimentoCard, 60, 180, 1000, 333);
  float temp = dht.getTempAndHumidity().temperature;

  // Calcular a perda de peso
  float perdaPeso = 0.0;
  if (nivelAgua > 0) {
    if (temp >= 36.0) {
      perdaPeso = (temp - 35.0) * 0.05; // Perda de peso aumenta com a temperatura
      nivelAgua -= perdaPeso; // Reduzir o nível de água no corpo
    } else {
      perdaPeso = 0.0;
    }
  }

  if (nivelAgua < 0) nivelAgua = 0; // Não permitir valores negativos

  // // Mostra a temperatura no terminal
  // Serial.print("Temperatura: ");
  // Serial.print(temp);
  // Serial.println(" C");
  // Serial.print("Nível: ");
  // Serial.println(nivelTemp);

  // // Mostra os batimentos no terminal
  // Serial.print("Batimento Cardiaco: ");
  // Serial.print(batimentoCard);
  // Serial.println(" BPM");
  // Serial.print("Nível: ");
  // Serial.println(nivelBat);

  // // Mostra a perda de peso no terminal
  // Serial.print("Perda de Peso: ");
  // Serial.print(perdaPeso);
  // Serial.println(" kg/h");
  // Serial.print("Nível de Água: ");
  // Serial.print(nivelAgua);
  // Serial.println(" %");

  // Lógica para LEDs e buzina para temperatura
  if (temp < 36.0) {
    digitalWrite(buzzerTemp, LOW);
    nivelTemp = "Normal";
  } else if (temp >= 36.0 && temp < 38.5) {
    digitalWrite(buzzerTemp, LOW);
    nivelTemp = "Elevado";
  } else {
    digitalWrite(buzzerTemp, HIGH);
    nivelTemp = "Critico";
  }

  // Lógica para LEDs e buzina para batimentos cardíacos
  if (batimentoCard < 150) {
    digitalWrite(buzzerBat, LOW);
    nivelBat = "Normal";
  } else if (batimentoCard >= 150 && batimentoCard < 190) {
    digitalWrite(buzzerBat, LOW);
    nivelBat = "Elevado";
  } else {
    digitalWrite(buzzerBat, HIGH);
    nivelBat = "Critico";
  }


  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();

    StaticJsonDocument<TAMANHO> doc;
    doc["batimento"] = batimentoCard;
    doc["temperatura"] = temp;
    doc["perdaPeso"] = perdaPeso;
    doc["nivelAgua"] = nivelAgua;

    char buffer[TAMANHO];
    serializeJson(doc, buffer);
    MQTT.publish(TOPIC_PUBLISH_DATA, buffer);
    Serial.println(buffer);
  }

  // Separa a atualização do Terminal
  Serial.println("-----------------------");

  // Controlar o servo motor para bater na mesma frequência que o batimento cardíaco
  servoMotor.write(90); 
  delay(batimentointervalo / 2); 
  servoMotor.write(70); 
  delay(batimentointervalo / 2); 
  servoMotor.write(110); 
  delay(batimentointervalo / 2);

  delay(1000);
}
