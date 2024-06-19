#include <WiFi.h>
#include <ESP32Servo.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ThingSpeak.h>

const char* SSID = "Wokwi-GUEST";
const char* PWD = "";
unsigned long int ChannelNumber = 2579993;
const char* writeAPIKey = "YSRRAER2SWJ593WE";
const char* readAPIKey = "J7VOV2VA42EHXIZ4";

WiFiClient client;

#define echoPin 18
#define trigPin 19
#define buzzerPin 32 

int echoDuration;
int distance;

Servo doorServo;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  
  Serial.begin(115200);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  WiFi.begin(SSID, PWD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi Connected.");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);

  doorServo.attach(23);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,30);
  display.println("Smart Door Lock");
  display.display();

  doorServo.write(0);
}

void writeOled(char* str) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(0,30);
  display.println(str);
  display.display();
}

int findDistance() {
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  echoDuration = pulseIn(echoPin,HIGH);
  distance = (0.034 * echoDuration)/2;

  return distance;
}

float avg(float *lst, int len) {
    float s = 0;
    for (int i = 0; i < len; i++) {
        s += lst[i];
    }
    return s / len;
}

float calcDist() {
    int i = 0;
    int rep = 5;
    float dists[rep+1];
    while (i <= rep) {
        float distance1 = findDistance();
    
        dists[i] = distance1;
        delay(200);
        i++;
    }
    return avg(dists, rep+1);
}

bool checkGuest() {
    float distlst[4];
    for (int i = 0; i < 4; i++) {
        distlst[i] = calcDist();
        delay(500);
    }
    float confirm = avg(distlst, 4);
    if (confirm <= 40) {
      Serial.print("Guest Alert at ");
      Serial.println(confirm);
      return true;
    } else {
      return false;
    }
}

void ringDoorBell() {
  for(int i=1;i<=3;i++){
        
      writeOled("Ringing");
      tone(buzzerPin,2500,1000);
      delay(1000);

      writeOled("Ringing.");
      tone(buzzerPin,1000,2000);
      delay(2000);

      writeOled("Ringing..");

      digitalWrite(buzzerPin,LOW);
      delay(2000);

      writeOled("Ringing...");
      }
  display.clearDisplay();
  writeOled("Smart Door Lock");
}

void loop() {
  int checkDist = findDistance();
  if (checkDist <= 40) {
  float dist = calcDist();
  if (dist <= 40) {
    bool check = checkGuest();
    if (check) {

      ThingSpeak.setField(1,dist);
      ThingSpeak.writeFields(ChannelNumber,writeAPIKey);

      ringDoorBell();

      delay(1000);
    }
  }
  }
  int A = ThingSpeak.readLongField(ChannelNumber, 2, readAPIKey);
  Serial.println(A);
  if (A==1) {
    doorServo.write(0);
    writeOled("Smart Door Lock");
  }
  else if (A==0) {
    doorServo.write(90);
    writeOled("Door Open");
  }
  
  delay(1500);
  
}