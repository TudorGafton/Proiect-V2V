#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

#define I2C_ADRESA 0x08
#define LED_PIN 27
#define IN1 26
#define IN2 25
#define IN3 33
#define IN4 32

const char* ssid = "DIGI-Y3Uj";
const char* password = "g4qCPHL3Fd";

WebServer server(80);

String codPrimit = "";
bool motorPornit = false;
bool franareDetectata = false;
unsigned long t_franare = 0;

void inainte() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void oprireMotoare() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void primesteDate(int numarOcteti) {
  codPrimit = "";
  while (Wire.available()) {
    char c = Wire.read();
    codPrimit += c;
  }
  codPrimit.trim();
  Serial.print("📩 Cod I2C primit: "); Serial.println(codPrimit);

  if (codPrimit == "1101") {
    motorPornit = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("✅ Motor pornit – LED față ON");
  } else if (codPrimit == "1011") {
    motorPornit = false;
    oprireMotoare();
    digitalWrite(LED_PIN, LOW);
    franareDetectata = false;
    Serial.println("⛔ Motor oprit – LED față OFF, motoare oprite");
  } else if (codPrimit == "1001") {
    if (motorPornit) {
      franareDetectata = true;
      t_franare = millis();
      oprireMotoare();
      digitalWrite(LED_PIN, HIGH);
      Serial.println("🛑 Frânare de urgență – motoare oprite, LED față rămâne ON");
    } else {
      Serial.println("⚠️ Cod frânare ignorat – motorul este oprit");
    }
  } else {
    Serial.println("⚠️ Cod necunoscut – ignorat");
  }
}

void handleStatus() {
  String status = "";
  if (motorPornit && franareDetectata)
    status = "🟠 ESP2: Motor pornit + Frânare activă";
  else if (motorPornit)
    status = "🟢 ESP2: Motor pornit";
  else
    status = "🔴 ESP2: Motor oprit";

  Serial.println("📡 Cerere HTTP GET /status");
  String mesaj = "{\"eveniment\": \"" + status + "\"}";
  server.send(200, "application/json", mesaj);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  oprireMotoare();
  digitalWrite(LED_PIN, LOW);

  Wire.begin(I2C_ADRESA);
  Wire.onReceive(primesteDate);

  WiFi.begin(ssid, password);
  Serial.print("Conectare la WiFi...");
  int k = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
    if (++k > 40) {
      Serial.println("❌ Eroare WiFi – verifică datele!");
      while (true) delay(1000);
    }
  }
  Serial.print("\n✅ WiFi conectat! IP ESP2.2: ");
  Serial.println(WiFi.localIP());

  server.on("/status", handleStatus);
  server.begin();
  Serial.println("🛰️ Server HTTP /status pornit.");
}

void loop() {
  server.handleClient();
  if (franareDetectata && millis() - t_franare > 5000) {
    franareDetectata = false;
  }
}
