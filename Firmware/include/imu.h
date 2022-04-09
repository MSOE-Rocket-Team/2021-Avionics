#ifndef IMU_H
#define IMU_H

/**
 * Inertial measurement data
 */
typedef struct {
    int16_t gyro[3];    /// gyroscope x, y, and z axis
    int16_t accel[3];   /// accelerometer x, y, and z axis
    uint32_t timestamp; /// inertial measurement timestamp
} inertial_measurement_t;

/**
 * Initialize inertial measurement unit
 * Consumes GPIO pins 9, 10, & 11
 */
void imu_init();

/**
 * Read the last imu measurement
 */
void imu_read(inertial_measurement_t* im);

/**
 * Immediately read measurement from IMU
 */
void imu_read_immediate(inertial_measurement_t* im);

#endif // IMU_H
