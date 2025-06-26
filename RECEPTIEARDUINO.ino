// ESP2.1 – receptor optic + I2C MASTER
#include <Wire.h>

#define FOTO_PIN 32
#define BIT_COUNT 4
#define BIT_DURATION 400
#define PAUZA_INTRE_CODURI 3000
#define PRAG_LUMINA 3200
#define I2C_SLAVE_ADDR 0x08

String cod = "";

void setup() {
  Serial.begin(115200);
  Wire.begin();
  analogReadResolution(12);
  Serial.println("📥 ESP2.1 activ – detectez optic și transmit prin I2C...");
}

void loop() {
  cod = "";
  while (analogRead(FOTO_PIN) > PRAG_LUMINA) {
    static bool afisat = false;
    if (!afisat) { Serial.println("Aștept semnal optic..."); afisat = true; }
    delay(10);
  }

  Serial.println("📡 Încep citirea codului optic...");
  for (int i = 0; i < BIT_COUNT; i++) {
    int val = analogRead(FOTO_PIN);
    Serial.printf("🔍 Bit %d: Valoare = %d → Detectat BIT = %c\n", i + 1, val, val < PRAG_LUMINA ? '1' : '0');
    cod += (val < PRAG_LUMINA) ? '1' : '0';
    delay(BIT_DURATION);
  }

  Serial.print("📥 Cod complet detectat: "); Serial.println(cod);

  if (cod == "1001" || cod == "1101" || cod == "1011") {
    Serial.printf("📤 Cod valid – trimit prin I2C la ESP2.2: %s\n", cod.c_str());
    Wire.beginTransmission(I2C_SLAVE_ADDR);
Wire.write((const uint8_t*)cod.c_str(), 4);  // conversie explicită corectă
Wire.endTransmission();

  } else {
    Serial.println("⚠️ Cod necunoscut – ignor");
  }

  delay(PAUZA_INTRE_CODURI);
}
