#include "pico/stdlib.h"
#include "pico/printf.h"
#include "hardware/spi.h"

#include "crc.h"
#include "diskio.h"

#define SD_DET    3   // Card detect
#define SDIO_DAT0 4   // Data Out
#define SDIO_DAT3 5   // Chip select
#define SDIO_CLK  6   // Clock
#define SDIO_CMD  7   // Data In

#define CMD0    (0x40 + 0)  // GO_IDLE_STATE:     Software reset
#define CMD1    (0x40 + 1)  // SEND_OP_COND:      Send host capacity info and activate init process
#define CMD8    (0x40 + 8)  // SEND_IF_COND:      Send host voltage info and gets operational range
#define CMD16   (0x40 + 16) // SET_BLOCKLEN:      Set card block length; fixed to 512 for SDHC & XC
#define CMD17   (0x40 + 17) // READ_SINGLE_BLOCK: Read block of size selected by SET_BLOCKLEN
#define CMD24   (0x40 + 24) // WRITE_BLOCK:       Write block of size selected by SET_BLOCKLEN
#define CMD55   (0x40 + 55) // APP_CMD:           Next command is application specific
#define CMD58   (0x40 + 58) // READ_OCR:          Read OCR register of card
#define ACMD41  (0xC0 + 41) // SD_SEND_OP_COND:   Send host capacity info and activate init process

enum CARD_TYPE {
  UNKNOWN,    // Unknown device
  MMCV3,      // MMC version 3
  SDV1,       // SD version 1
  SDV2_BYTE,  // SD version 2 byte addressing
  SDV2_BLOCK, // SD version 2 block addressing
};

static enum CARD_TYPE card_type = UNKNOWN;

// Chip select active low
static inline void chip_select(uint gpio) {
  asm volatile("nop \n nop \n nop");
  gpio_put(gpio, 0);
  asm volatile("nop \n nop \n nop");
}

// Chip deselect active low
static inline void chip_deselect(uint gpio) {
  asm volatile("nop \n nop \n nop");
  gpio_put(gpio, 1);
  asm volatile("nop \n nop \n nop");
}

// Send SPI command
void send_cmd(BYTE cmd, DWORD arg, uint8_t* dst, size_t len) {
  if (cmd & 0x80) { // Handle ACMD# command sequence
    cmd &= 0x7F;  // Command variable falls thru
    uint8_t data[] = {0};
    send_cmd(CMD55, 0, data, 1);  // Prelude by CMD55
    if (data[0]) {  // Error on CMD55; return immediate
      if (len != 0) dst[0] = data[0];
      return;
    }
  }

  uint8_t buf[] = {
      cmd,
      (BYTE)(arg >> 24),
      (BYTE)(arg >> 16),
      (BYTE)(arg >> 8),
      (BYTE)arg,
      0
  };
  buf[5] = (crc7(buf, 5) << 1) + 1; // Calculate CRC and end bit

  printf("send_cmd: send");
  for (int i = 0; i < 6; i++) {
    printf(" %x", buf[i]);
  }
  printf("\n");

  chip_select(SDIO_DAT3);
  spi_write_blocking(spi0, buf, 6);
  if (len != 0) spi_read_blocking(spi0, 0xFF, dst, len);
  chip_deselect(SDIO_DAT3);

  printf("send_cmd: recv");
  while (len--) {
    printf(" %x", dst[len]);
  }
  printf("\n");
}

// Initialize Disk Drive
DSTATUS disk_initialize() {
  DSTATUS stat = STA_NOINIT;
  printf("Using correct diskio lib\n");

  // Initialize SPI clock and data pins
  sleep_ms(1);
  spi_init(spi0,100000); // 100kHz baudrate
  spi_set_format(spi0,8,1,1,SPI_MSB_FIRST);
  gpio_set_function(SDIO_DAT0, GPIO_FUNC_SPI);
  gpio_set_function(SDIO_CLK, GPIO_FUNC_SPI);
  gpio_set_function(SDIO_CMD, GPIO_FUNC_SPI);

  // Chip select active low
  gpio_init(SDIO_DAT3);
  gpio_set_dir(SDIO_DAT3, GPIO_OUT);
  chip_deselect(SDIO_DAT3);
  sleep_ms(1);  // Dummy clock

  // Detect if SD card inserted
  gpio_init(SD_DET);
  gpio_set_dir(SD_DET, GPIO_IN);
  if (gpio_get(SD_DET)) {
    return STA_NODISK;
  }

  // 80 dummy clock cycles
  chip_select(SDIO_DAT3);
  spi_write_blocking(spi0, (uint8_t[]){
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    }, 10);
  chip_deselect(SDIO_DAT3);

  uint8_t data[] = {0, 0, 0, 0, 0};
  send_cmd(CMD0, 0, data, 1);
  if (data[0] == 0x01) {  // Software reset
    send_cmd(CMD8, 0x1AA, data, 5);
    if (data[0] == 0x01) {  // SDv2
      if (data[3] == 0x01 && data[4] == 0xAA) { // Works at VDD range of 2.7-3.6V
        uint tmr = 10000;
        do {  // Wait for leaving idle state (ACMD41 with High Capacity Support)
          sleep_us(100);
          send_cmd(ACMD41, 1 << 30, data, 1);
        } while (tmr-- && data[0]);

        send_cmd(CMD58, 0, data, 5);
        if (tmr && data[0] == 0) {  // Check CCS bit in the OCR
          card_type = (data[1] & 0x40) ? SDV2_BLOCK : SDV2_BYTE;	// SD version 2 block/byte addr.
          if (card_type == SDV2_BYTE) {
            send_cmd(CMD16, 512, data, 1);  // Force block size to 512 bytes
            if (data[0] == 0) {
              stat = 0x00;  // Successfully initialized
            }
          } else {
            stat = 0x00;  // Successfully initialized
          }
        }
      }
    } else {  // SDv1 or MMCv3
      uint tmr = 10000;
      send_cmd(ACMD41, 0, data, 1);
      if (data[0] <= 1) {
        card_type = SDV1; // SD version 1
        do {  // Wait for leaving idle state
          sleep_us(100);
          send_cmd(ACMD41, 0, data, 1);
        } while (--tmr && data[0]);
      } else {
        card_type = MMCV3;  // MMC version 3
        do {  // Wait for leaving idle state
          sleep_us(100);
          send_cmd(CMD1, 0, data, 1);
        } while (--tmr && data[0]);
      }

      send_cmd(CMD16, 512, data, 1);  // Force block size to 512 bytes
      if (tmr && data[0] == 0) {
        stat = 0x00;  // Successfully initialized
      }
    }
  }

  printf("card status: %d\ncard type: %d\n", stat, card_type);
  return stat;
}

// Read Partial Sector
DRESULT disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count) {
  DRESULT res = RES_ERROR;

  if (card_type == SDV2_BLOCK) sector *= 512; // Convert to byte address if needed

  uint8_t data[] = {0};
  send_cmd(CMD17, sector, data, 1); // Read data block
  if (data[0] == 0) {
    chip_select(SDIO_DAT3); // Ready chip for consecutive read

    uint tmr = 40000;
    do {  // Wait for data token
      spi_read_blocking(spi0, 0xFF, data, 1);
    } while (--tmr && data[0] == 0xFF);

    if (tmr && data[0] == 0xFE) {  // Data token arrived
      // Skip leading bytes from offset
      while (offset--) spi_read_blocking(spi0, 0xFF, data, 1);

      // Receive part of the sector
      spi_read_blocking(spi0, 0xFF, buff, count);

      // Skip trailing bytes and CRC
      uint trailing = 512 + 2 - offset - count; // Number of trailing bytes to skip
      uint8_t discard[trailing];
      spi_read_blocking(spi0, 0xFF, discard, trailing);

      res = RES_OK;
    }
  }

  chip_deselect(SDIO_DAT3);
  return res;
}

// Write Partial Sector
DRESULT disk_writep(const BYTE* buff, DWORD sc) {
  DRESULT res = RES_ERROR;
  static uint wc; // Sector write counter

  if (!buff) {
    if (sc) { // Initiate a sector write transaction
      if (card_type == SDV2_BLOCK) sc *= 512; // Convert to byte address if needed

      uint8_t data[] = {0};
      send_cmd(CMD24, sc, data, 1);
      if (data[0] == 0) {
        chip_select(SDIO_DAT3);  // Ready chip for consecutive write

        spi_write_blocking(spi0, (uint8_t[]){0xFF, 0xFE}, 2); // write padding + data block header
        wc = 512; // Set write counter
        res = RES_OK;
      }
    } else {  // Finalize a sector write transaction
      wc += 2;  // Fill remaining bytes and CRC with zeroes
      while (wc--) spi_write_blocking(spi0, (uint8_t[]){0x00}, 1);
      uint8_t data[] = {0};
      spi_read_blocking(spi0, 0xFF, data, 1);
      if ((data[0] & 0x1F) == 0x05) { // Data response token status "accepted"
        uint tmr = 5000;
        do {  // Block while card busy
          sleep_us(100);
          spi_read_blocking(spi0, 0xFF, data, 1);
        } while (--tmr && data[0] != 0xFF);
        if (tmr) res = RES_OK;
      }

      chip_deselect(SDIO_DAT3); // Release chip from consecutive write
    }
  } else {  // Perform sector write transaction
    while (wc-- && sc--) {
      spi_write_blocking(spi0, buff++, 1);  // Write each byte in buff while wc >= 0
    }
    res = RES_OK;
  }

  return res;
}
