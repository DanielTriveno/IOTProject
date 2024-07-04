#include <WiFi.h>
#include <Wire.h>	
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Credenciales WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Dirección del broker MQTT
const char* mqtt_server = "broker.hivemq.com";

// Declaración del topic y clientID
const char* topic_control = "home/door/control";
const char* topic_status = "home/door/status";
const char* topic_distance = "home/door/distance";
const char* clientID = "ESP32Client";

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
int thresholdDistance = 10; // Umbral de distancia en cm
bool doorLocked = true;

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Inicializar WiFi
  setup_wifi();

  // Inicializar MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Inicializar pines del sensor ultrasónico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializar el buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicializar el servo
  servo.attach(SERVO_PIN);

  // Inicializar el display OLED
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Asegúrate de usar la dirección I2C correcta
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Bucle infinito si la pantalla no se pudo inicializar
  }
  display.display(); // Mostrar cualquier cosa que esté en el buffer (debería ser una pantalla en blanco)
  delay(2000); // Esperar dos segundos
}


void writeOled(char* str) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); 
  display.clearDisplay();
  display.setCursor(0,30);
  display.println(str);
  display.display();
}

void loop() {
   display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Smart Door Lock");
  display.display(); // Asegúrate de llamar a display.display() después de dibujar/limpiar
  
  if (!client.connected()) {
    reconnect();
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

  // Publicar la distancia medida
  String distanceStr = String(distance);
  client.publish(topic_distance, distanceStr.c_str());

  // Depuración: Imprimir distancia en el serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Actualizar display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Distance: ");
  display.print(distance);
  display.print(" cm");
  display.setCursor(0, 10);
  display.print("Door: ");
  display.print(doorLocked ? "Locked" : "Unlocked");
  display.display();

  // Verificar si la distancia es menor al umbral
  if (distance < thresholdDistance) {
    // Activar buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    tone(BUZZER_PIN, 2500, 1000);  // Emitir tono de 2500Hz durante 1 segundo
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1000);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID)) {
      Serial.println("connected");
      client.subscribe(topic_control);
      // Publicar el estado inicial de la cerradura
      client.publish(topic_status, doorLocked ? "locked" : "unlocked");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  if (String(topic) == "home/door/control") {
    if (messageTemp == "unlock") {
      servo.write(90); // Abrir cerradura
      doorLocked = false;
    } else if (messageTemp == "lock") {
      servo.write(0); // Cerrar cerradura
      doorLocked = true;
    }
    // Publicar el nuevo estado de la cerradura
    client.publish(topic_status, doorLocked ? "locked" : "unlocked");
  }
}