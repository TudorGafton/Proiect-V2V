#include <WiFi.h>
#include <WebServer.h>

// SETĂRI WiFi
const char* ssid = "DIGI-Y3Uj";
const char* password = "g4qCPHL3Fd";
const int ledStop = 14;
String codFranare = "1001";
const int durataBit = 400;
const int pauzaIntreCoduri = 3000;
const int dutyCycle = 100;
const int perioadaPWM = 10;
IPAddress local_IP(192, 168, 1, 131);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

void pwmManual(int pin, int durata) {
  unsigned long tStart = millis();
  while (millis() - tStart < durata) {
    digitalWrite(pin, HIGH);
    delay(perioadaPWM * dutyCycle / 100);
    digitalWrite(pin, LOW);
    delay(perioadaPWM * (100 - dutyCycle) / 100);
  }
}

void transmitCod(int ledPin, String cod) {
  Serial.print("Transmisie cod optic: ");
  Serial.println(cod);
  for (int i = 0; i < cod.length(); i++) {
    char bit = cod[i];
    Serial.printf("Bit[%d]=%c\n", i, bit);
    if (bit == '1') {
      pwmManual(ledPin, durataBit);
    } else {
      digitalWrite(ledPin, LOW);
      delay(durataBit);
    }
  }
  digitalWrite(ledPin, LOW);
  Serial.println("Cod transmis complet.");
}

void handleFranareUrgenta() {
  Serial.println("Cerere HTTP /franare primită!");

  // 🔹 Trimitem răspunsul rapid, ca să nu expire Flask
  server.send(200, "text/plain", "Cod frânare transmis!");

  // 🔹 După aceea, rulăm transmiterea codului optic
  transmitCod(ledStop, codFranare);
}


void setup() {
   Serial.begin(115200);
  pinMode(ledStop, OUTPUT);

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  int nrIncercari = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    nrIncercari++;
    if (nrIncercari > 40) { // după 20 secunde renunță
      Serial.println("\nEroare: Nu s-a putut conecta la WiFi!");
      while (true) { delay(1000); }
    }
  }
  Serial.println("\nWiFi conectat cu succes!");
  Serial.print("IP ESP2: ");
  Serial.println(WiFi.localIP());

  server.on("/franare", handleFranareUrgenta);
  server.begin();
  Serial.println("Server HTTP pornit pe portul 80.");
  Serial.println("-------------------------");
  Serial.println("Accesează din browser: http://[IP_AFISAT]/franare");
}

void loop() {
  server.handleClient();
}
