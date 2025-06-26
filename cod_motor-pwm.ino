#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "DIGI-Y3Uj";
const char* password = "g4qCPHL3Fd";

const int ledFata = 27;       // LED-uri față (corect)
const int ledStop = 14;       // LED frânare (pentru transmisie)
const int IN1 = 26, IN2 = 25, IN3 = 33, IN4 = 32;

WebServer server(80);

String codFranare = "1001";
String codPornire = "1101";
String codOprire = "1011";

bool motorPornit = false;
volatile bool inainteFlag = false;
unsigned long inainteStart = 0;
int inainteDuration = 4000;  // ms

void inainte() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void oprireMotoare() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// PWM software pentru transmisie LED
void pwmManual(int pin, int durata) {
  int perioadaPWM = 10;
  int dutyCycle = 100;
  unsigned long tStart = millis();
  while (millis() - tStart < durata) {
    digitalWrite(pin, HIGH);
    delay(perioadaPWM * dutyCycle / 100);
    digitalWrite(pin, LOW);
    delay(perioadaPWM * (100 - dutyCycle) / 100);
  }
}

void transmitCod(int ledPin, String cod) {
  for (int i = 0; i < cod.length(); i++) {
    if (cod[i] == '1') pwmManual(ledPin, 400);
    else { digitalWrite(ledPin, LOW); delay(400); }
  }
  digitalWrite(ledPin, LOW);
}

void handlePornireMotor() {

  digitalWrite(ledFata, HIGH);
  transmitCod(ledStop, codPornire);
  motorPornit = true;
  server.send(200, "text/plain", "ESP1.2: Motor pornit, LED fata ON, cod 1101 transmis.");
  Serial.println("[INFO] Motor pornit, LED fata aprins.");
}

void handleOprireMotor() {
  digitalWrite(ledFata, LOW);
  transmitCod(ledStop, codOprire);
  motorPornit = false;
  server.send(200, "text/plain", "ESP1.2: Motor oprit, LED fata OFF, cod 1011 transmis.");
  Serial.println("[INFO] Motor oprit, LED fata stins.");
}

void handleFranare() {
  if (!motorPornit) {
    server.send(200, "text/plain", "NU se poate frana: motorul este oprit.");
    Serial.println("[WARN] Franare ignorata: motorul nu este pornit.");
    return;
  }
  inainte();
  inainteStart = millis();
  inainteFlag = true;
  server.send(200, "text/plain", "ESP1.2: Merge inainte si va frana automat in 4s.");
  Serial.println("[INFO] Frana initiata: robot merge inainte 4s, apoi transmite 1001.");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledStop, OUTPUT);
  pinMode(ledFata, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  oprireMotoare();
  digitalWrite(ledFata, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectare la WiFi...");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi conectat!");
  Serial.print("ESP1.2 IP: "); Serial.println(WiFi.localIP());

  server.on("/pornire_motor", handlePornireMotor);
  server.on("/oprire_motor", handleOprireMotor);
  server.on("/franare", handleFranare);
  server.begin();
  Serial.println("[HTTP] Server pornit pe ESP1.2.");
}

void loop() {
  server.handleClient();

  if (inainteFlag && millis() - inainteStart >= inainteDuration) {
    oprireMotoare();
    transmitCod(ledStop, codFranare);
    inainteFlag = false;
    Serial.println("[INFO] ESP1.2: Oprit si transmis cod de franare 1001.");
  }
}#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "DIGI-Y3Uj";
const char* password = "g4qCPHL3Fd";

const int ledFata = 27;       // LED-uri față (corect)
const int ledStop = 14;       // LED frânare (pentru transmisie)
const int IN1 = 26, IN2 = 25, IN3 = 33, IN4 = 32;

WebServer server(80);

String codFranare = "1001";
String codPornire = "1101";
String codOprire = "1011";

bool motorPornit = false;
volatile bool inainteFlag = false;
unsigned long inainteStart = 0;
int inainteDuration = 4000;  // ms

void inainte() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void oprireMotoare() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// PWM software pentru transmisie LED
void pwmManual(int pin, int durata) {
  int perioadaPWM = 10;
  int dutyCycle = 100;
  unsigned long tStart = millis();
  while (millis() - tStart < durata) {
    digitalWrite(pin, HIGH);
    delay(perioadaPWM * dutyCycle / 100);
    digitalWrite(pin, LOW);
    delay(perioadaPWM * (100 - dutyCycle) / 100);
  }
}

void transmitCod(int ledPin, String cod) {
  for (int i = 0; i < cod.length(); i++) {
    if (cod[i] == '1') pwmManual(ledPin, 400);
    else { digitalWrite(ledPin, LOW); delay(400); }
  }
  digitalWrite(ledPin, LOW);
}

void handlePornireMotor() {

  digitalWrite(ledFata, HIGH);
  transmitCod(ledStop, codPornire);
  motorPornit = true;
  server.send(200, "text/plain", "ESP1.2: Motor pornit, LED fata ON, cod 1101 transmis.");
  Serial.println("[INFO] Motor pornit, LED fata aprins.");
}

void handleOprireMotor() {
  digitalWrite(ledFata, LOW);
  transmitCod(ledStop, codOprire);
  motorPornit = false;
  server.send(200, "text/plain", "ESP1.2: Motor oprit, LED fata OFF, cod 1011 transmis.");
  Serial.println("[INFO] Motor oprit, LED fata stins.");
}

void handleFranare() {
  if (!motorPornit) {
    server.send(200, "text/plain", "NU se poate frana: motorul este oprit.");
    Serial.println("[WARN] Franare ignorata: motorul nu este pornit.");
    return;
  }
  inainte();
  inainteStart = millis();
  inainteFlag = true;
  server.send(200, "text/plain", "ESP1.2: Merge inainte si va frana automat in 4s.");
  Serial.println("[INFO] Frana initiata: robot merge inainte 4s, apoi transmite 1001.");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledStop, OUTPUT);
  pinMode(ledFata, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  oprireMotoare();
  digitalWrite(ledFata, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectare la WiFi...");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi conectat!");
  Serial.print("ESP1.2 IP: "); Serial.println(WiFi.localIP());

  server.on("/pornire_motor", handlePornireMotor);
  server.on("/oprire_motor", handleOprireMotor);
  server.on("/franare", handleFranare);
  server.begin();
  Serial.println("[HTTP] Server pornit pe ESP1.2.");
}

void loop() {
  server.handleClient();

  if (inainteFlag && millis() - inainteStart >= inainteDuration) {
    oprireMotoare();
    transmitCod(ledStop, codFranare);
    inainteFlag = false;
    Serial.println("[INFO] ESP1.2: Oprit si transmis cod de franare 1001.");
  }
}
