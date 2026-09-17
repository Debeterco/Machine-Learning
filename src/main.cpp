#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>
#include "credentials.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqtt_user = MQTT_USER;
const char* mqtt_password = MQTT_PASS;

// HiveMQ Cloud Config
const char* mqtt_server = "e0722a79b31148c9b57155e48211f476.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define PIN_DHT 15 // DHTSensor PIN
#define PIN_POT1 34 // Potentiometer PIN1
#define PIN_POT2 35 // Potentiometer PIN2
#define LED_G 25  // Green LED PIN

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Configurate the LCD to the address 0x27

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensagem recebida no tópico: ");
    Serial.println(topic);

    // Convert the payload (bytes) for String
    String msg = "";
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    Serial.println("Conteúdo: " + msg);

    if (String(topic) == "ctw/joao/led") {
        if (msg == "1" || msg == "ON") {
            digitalWrite(LED_G, HIGH);
            Serial.println("LED Verde LIGADO remotamente!");
        } 
        else if (msg == "0" || msg == "OFF") {
            digitalWrite(LED_G, LOW);
            Serial.println("LED Verde DESLIGADO remotamente!");
        }
    }
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando em ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Wi-Fi conectado!");
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Tentando conexão MQTT...");

        String clientId = "ESP32Client-" + String(random(0xffff), HEX); // ClientID unique based in MAC
        
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
            Serial.println("conectado ao HiveMQ Cloud!");
            
            client.subscribe("ctw/joao/led");
        } else {
            Serial.print("falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();

    // IMPORTANT: Ignores the strict validation of the root certificate(Root CA) 
    // to allow the criptographed TLS connection, do not using much memory from ESP32
    espClient.setInsecure(); 

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    dht.setup(PIN_DHT, DHTesp::DHT22);

    lcd.init();
    lcd.backlight();

    pinMode(LED_G, OUTPUT);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Example: Send a message in 10 seconds
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 10000) {
        lastMsg = millis();

        TempAndHumidity data = dht.getTempAndHumidity();
        String tempStr = String(data.temperature, 1);
        String humStr = String(data.humidity, 0);

        int pot1_valor = analogRead(PIN_POT1);
        int pot2_valor = analogRead(PIN_POT2);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T:" + tempStr + "C P1:" + String(pot1_valor));
        lcd.setCursor(0, 1);
        lcd.print("U:" + humStr + "% P2:" + String(pot2_valor));

        client.publish("ctw/joao/temperature", tempStr.c_str());
        client.publish("ctw/joao/humidity", humStr.c_str());
        client.publish("ctw/joao/pot1", String(pot1_valor).c_str());
        client.publish("ctw/joao/pot2", String(pot2_valor).c_str());

        Serial.println("Dados enviados para o HiveMQ Cloud!");
    }
}