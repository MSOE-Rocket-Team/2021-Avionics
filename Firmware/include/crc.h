#ifndef CRC_H
#define CRC_H

#include <stddef.h>
#include <stdint.h>

uint8_t crc7(const uint8_t* data, size_t len);

uint16_t crc16(const uint8_t* data, size_t len);

#endif // CRC_H
