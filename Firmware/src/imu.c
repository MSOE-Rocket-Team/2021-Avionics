#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"

#include "imu.h"

#define IMU_INT1 9
#define I2C1_SDA 10
#define I2C1_SCL 11

#define INT1_CTRL   0x0D    // Interrupt 1 pin control register
#define STATUS_REG  0x1E    // TODO Read only necessary values
#define OUTX_L_G    0x22    // Angular rate sensor pitch x-axis
#define OUTX_L_A    0x28    // Linear acceleration sensor x-axis
#define TIMESTAMP0  0x40

#define INT1_DRDY_G 0x2     // Gyroscope data-ready interrupt flag
#define INT1_DRDY_A 0x1     // Accelerometer data-ready interrupt flag

#define IMU_SAD     0b1101010    // IMU slave address, last bit is 1 for second IMU

inertial_measurement_t last_measurement = {
        {0, 0, 0},
        {0, 0, 0},
        0
};

void imu_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        // Read new IMU measurements
        // "The interrupt is reset when the higher part of one of the enabled channels is read."
        // Note: Addresses of consecutive read operations are incremented by 1 byte
        uint8_t g_data = OUTX_L_G;
        uint8_t g_buffer[6];
        i2c_write_blocking(i2c1, IMU_SAD, &g_data, 1, true);
        i2c_read_blocking(i2c1, IMU_SAD, g_buffer, 6, false);

        uint8_t a_data = OUTX_L_A;
        uint8_t a_buffer[6];
        i2c_write_blocking(i2c1, IMU_SAD, &a_data, 1, true);
        i2c_read_blocking(i2c1, IMU_SAD, a_buffer, 6, false);

        uint8_t ts_data = TIMESTAMP0;
        uint8_t ts_buffer[4];
        i2c_write_blocking(i2c1, IMU_SAD, &ts_data, 1, true);
        i2c_read_blocking(i2c1, IMU_SAD, ts_buffer, 4, false);

        // Convert buffers to measurement
        for (int i = 0; i < 3; i++) {
            uint16_t g_axis = g_buffer[i * 2] | (g_buffer[i * 2 + 1] << 8);
            last_measurement.gyro[i] = (int16_t) ~(g_axis - 1); // Undo two's complement

            uint16_t a_axis = a_buffer[i * 2] | (a_buffer[i * 2 + 1] << 8);
            last_measurement.accel[i] = (int16_t) ~(a_axis - 1);
        }

        last_measurement.timestamp = ts_buffer[0] | (ts_buffer[1] << 8) | (ts_buffer[2] << 16) | (ts_buffer[3] << 24);
    }
}

void imu_init() {
    i2c_init(i2c1, 100000); // 100kHz baudrate
    gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SDA);
    gpio_pull_up(I2C1_SCL);
    bi_decl(bi_2pins_with_func(I2C1_SDA, I2C1_SCL, GPIO_FUNC_I2C));

    // Enable interrupts for gyroscope and accelerometer
    uint8_t data[] = { INT1_CTRL, INT1_DRDY_G | INT1_DRDY_A };
    i2c_write_blocking(i2c1, IMU_SAD, data, 2, false);
    gpio_set_irq_enabled_with_callback(IMU_INT1, GPIO_IRQ_EDGE_RISE , true, &imu_callback);
}

void imu_read(inertial_measurement_t* im) {
    memcpy(im->accel, last_measurement.accel, 3 * sizeof(int16_t));
    memcpy(im->gyro, last_measurement.gyro, 3 * sizeof(int16_t));
    im->timestamp = last_measurement.timestamp;
}
