#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

#define I2C_ADRESA 0x08        // Adresa la care ESP2 va răspunde
#define LED_PIN 27             // LED de putere
#define IN1 26                 // Motor dreapta
#define IN2 25
#define IN3 33                 // Motor stânga
#define IN4 32
  
const char* ssid = "DIGI-Y3Uj";
const char* password = "g4qCPHL3Fd";

WebServer server(80);

String codPrimit = "";
volatile bool franareDetectata = false;
unsigned long t_franare = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_ADRESA);             
  Wire.onReceive(primesteDate);       

  pinMode(LED_PIN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  oprireMotoare();

  // Pornire WiFi
IPAddress local_IP(192, 168, 1, 135);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFi.config(local_IP, gateway, subnet);

  WiFi.begin(ssid, password);
  Serial.print("Conectare la WiFi...");
  int k = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    k++;
    if (k > 40) {
      Serial.println("\nNu s-a conectat la WiFi!");
      while (true) { delay(1000); }
    }
  }
  Serial.println("\nWiFi conectat! IP ESP2: ");
  Serial.println(WiFi.localIP());

  // Pornim imediat serverul HTTP și endpointul
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("✅ Server HTTP ESP2 pornit. Accesează: http://[IP]/status");

  Serial.println("🔧 ESP2 este pregătit să primească coduri prin I2C...");
}

void loop() {
  server.handleClient();

  // Resetează flagul după 5 secunde
  if (franareDetectata && millis() - t_franare > 5000) {
    franareDetectata = false;
  }
}

void primesteDate(int numarOcteti) {
  codPrimit = "";

  while (Wire.available()) {
    char c = Wire.read();
    codPrimit += c;
  }

  codPrimit.trim();  // eliminăm spațiile
  Serial.print("📩 Cod I2C primit: ");
  Serial.println(codPrimit);

  if (codPrimit == "1001") {
    Serial.println("🛑 Frânare de urgență – reacționez cu LED și motoare");

    // Setează flag pentru status HTTP
    franareDetectata = true;
    t_franare = millis();

    // LED ON-OFF de 3 ori
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_PIN, LOW);
      delay(300);
    }

    // Pornește motoarele înainte timp de 1s
    inainte();
    delay(1000);
    oprireMotoare();
  } else {
    Serial.println("⚠️ Cod necunoscut – ignor");
  }
}

void inainte() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void oprireMotoare() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// HTTP status pentru dashboard
void handleStatus() {
  Serial.println("📡 Cerere HTTP GET /status primită.");
  String status = "Aștept cod de frânare...";
  if (franareDetectata) {
    status = "🟠 ESP2: Frânare de urgență RECEPȚIONATĂ!";
  }
  String mesaj = "{\"eveniment\": \"" + status + "\"}";
  server.send(200, "application/json", mesaj);
}
