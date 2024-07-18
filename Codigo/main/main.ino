#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "secrets.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// Pines del sensor ultrasónico HC-SR04
#define TRIG_PIN 19
#define ECHO_PIN 18

// Pines del servo motor
Servo servo;
#define SERVO_PIN 23

// Buzzer
#define BUZZER_PIN 32

// Display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables
long duration;
int distance;
int thresholdDistance = 5; // Umbral de distancia en cm
bool doorLocked = true;
int closedAngle = 0;   // Ajusta este valor según la calibración
int openedAngle = 90;  // Ajusta este valor según la calibración

// Modos
enum Mode { AUTOMATIC, SECURE };
Mode currentMode = AUTOMATIC;

// AWS IoT
WiFiClientSecure net;
PubSubClient client(net);

#define AWS_IOT_PUBLISH_TOPIC   "hcsr04data"
#define AWS_IOT_SUBSCRIBE_TOPIC "hcsr04/mode"

// NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Actualiza cada minuto

void setup() {
  Serial.begin(115200);

  // Inicializar pines del sensor ultrasónico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializar el buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicializar el servo
  servo.attach(SERVO_PIN);
  lockDoor(); // Cerradura cerrada

  // Inicializar la pantalla OLED
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Conectar a AWS IoT
  connectAWS();

  // Inicializar NTP
  timeClient.begin();
}

void loop() {
  // Actualizar la hora
  timeClient.update();

  // Mantener la conexión con AWS IoT
  if (!client.connected()) {
    reconnectAWS();
  }
  client.loop();

  // Medir distancia
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // Depuración: Imprimir distancia en el serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Actualizar display
  updateDisplay();

  // Verificar si la distancia es menor al umbral
  if (distance <= thresholdDistance) {
    if (currentMode == AUTOMATIC) {
      servo.attach(SERVO_PIN);
      unlockDoor();
      delay(3000); // Esperar 3 segundos antes de cerrar la puerta
      lockDoor();
    } else if (currentMode == SECURE) {
      servo.detach();
      tone(BUZZER_PIN, 2500, 500);  // Emitir tono de 2500Hz durante 0.5 segundos
    }
  } else {
    noTone(BUZZER_PIN);
  }

  // Publicar datos en AWS IoT
  publishMessage();

  delay(1000); // Retraso de 1 segundo entre lecturas
}

void lockDoor() {
  if (currentMode == AUTOMATIC) {
    servo.write(closedAngle); // Posición de cerradura cerrada
  }
  doorLocked = true;
  Serial.println("Door locked");
}

void unlockDoor() {
  if (currentMode == AUTOMATIC) {
    servo.write(openedAngle); // Posición de cerradura abierta
  }
  doorLocked = false;
  Serial.println("Door unlocked");
}

void updateDisplay() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Door Lock");
  
  // Separator line
  display.drawLine(0, 20, SCREEN_WIDTH, 20, SSD1306_WHITE);

  // Distance
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.print("Distance: ");
  display.setTextSize(2);
  display.setCursor(70, 25);
  display.print(distance);
  display.print(" cm");

  // Mode
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("Mode: ");
  display.setTextSize(2);
  display.setCursor(70, 50);
  display.print(currentMode == AUTOMATIC ? "Auto" : "Secure");

  display.display();
}


void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setCallback(messageReceived);

  Serial.println("Connecting to AWS IoT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("AWS IoT Connected!");
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
}

void reconnectAWS() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(THINGNAME)) {
      Serial.println("connected");
      client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishMessage() {
  StaticJsonDocument<200> doc;
  doc["distance"] = distance;
  doc["doorLocked"] = doorLocked;
  doc["timestamp"] = timeClient.getEpochTime(); // Añadir timestamp
  doc["mode"] = currentMode == AUTOMATIC ? "Automatic" : "Secure";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  StaticJsonDocument<200> doc;
  deserializeJson(doc, message);
  const char* mode = doc["mode"];

  if (strcmp(mode, "Automatic") == 0) {
    currentMode = AUTOMATIC;
  } else if (strcmp(mode, "Secure") == 0) {
    currentMode = SECURE;
  }

  Serial.print("Mode changed to: ");
  Serial.println(mode);
}
