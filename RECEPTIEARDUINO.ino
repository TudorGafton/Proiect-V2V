#include <Wire.h>

#define FOTO_PIN 32             // GPIO conectat la fototranzistor
#define BIT_COUNT 4             // 4 biți de citit
#define BIT_DURATION 400        // ms – identic cu emisia
#define PAUZA_INTRE_CODURI 3000 // pauză între transmisii
#define PRAG_LUMINA 3200    // ajustat în funcție de lumină

#define I2C_SLAVE_ADDR 0x08     // adresa ESP2 (slave)

String cod = "";

void setup() {
  Serial.begin(115200);
  Wire.begin();  // ESP1 este master
  analogReadResolution(12);  // ESP32: valori 0–4095

  Serial.println("📥 ESP1 activ – detectez optic codul 1001 și îl transmit la ESP2 prin I2C...");
}

void loop() {
  cod = "";

  // Așteaptă începutul transmisiei
  while (analogRead(FOTO_PIN) > PRAG_LUMINA) {
    static bool mesajAfisat = false;
    if (!mesajAfisat) {
      Serial.println(" Aștept semnal optic...");
      mesajAfisat = true;
    }
    delay(10);
  }

  Serial.println("📡 Semnal detectat! Încep citirea codului...");

  // Citim 4 biți la intervale fixe
  for (int i = 0; i < BIT_COUNT; i++) {
    int valoare = analogRead(FOTO_PIN);
    Serial.print("🔎 Val citită: ");
    Serial.println(valoare);

    if (valoare < PRAG_LUMINA) {
      cod += "1";
    } else {
      cod += "0";
    }

    delay(BIT_DURATION);
  }

  Serial.print("📥 Cod detectat: ");
  Serial.println(cod);

  // Dacă este codul de frânare
  if (cod == "1001") {
    Serial.println("🛑 Frânare de urgență detectată – trimit codul la ESP2...");

    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write((const uint8_t*)"1001", 4);  // transmisie ca array de caractere
    Wire.endTransmission();

  } else {
    Serial.println("⚠️ Cod necunoscut sau semnal corupt – nu trimit nimic.");
  }

  delay(PAUZA_INTRE_CODURI);
}
