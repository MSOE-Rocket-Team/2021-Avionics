#include <stddef.h>
#include <stdint.h>

#include "crc.h"

uint8_t crc7(const uint8_t* data, size_t len) {
  // Polynomial: x^7 + x^3 + 1
  uint8_t crc = 0x00;
  uint8_t poly = 0x09;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      crc = (crc & 0x80) ? (crc << 1) ^ (poly << 1) : crc << 1;
    }
  }
  return crc >> 1;
}

uint16_t crc16(const uint8_t* data, size_t len) {
  // Polynomial: x^16 + x^12 + x^5 + 1
  uint16_t crc = 0x0000;
  uint16_t poly = 0x1021;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (int j = 0; j < 8; j++) {
      crc = (crc & 0x8000) ? (crc << 1) ^ poly : crc << 1;
    }
  }
  return crc;
}
