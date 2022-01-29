

#define WI2C 0x54


void wireWrite(uint8_t reg, uint8_t val) {
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)val);
}

void beginI2c() {
  Wire.beginTransmission((uint8_t)WI2C);
}
void endI2c() {
  Wire.endTransmission();
}

void initI2c() {
  beginI2c();
  uint8_t d[] = {0x01};
  Wire.write(0x00);
  Wire.write(d, 1);
  endI2c();

  beginI2c();
  wireWrite(0x13, 0xFF);
  endI2c();

  beginI2c();
  wireWrite(0x14, 0xFF);
  endI2c();

  beginI2c();
  wireWrite(0x15, 0xFF);
  endI2c();
}

void updateI2c() {
  beginI2c();
  wireWrite(0x16, 0xFF);
  endI2c();
}

void cleanupI2c() {
  beginI2c();
  uint8_t d[] = {0x00};
  Wire.write(0x00);
  Wire.write(d, 1);
  endI2c();
}

void ledI2c(uint8_t addr, uint8_t i) {
  beginI2c();
  wireWrite(addr, (byte) i);
  endI2c();
}
