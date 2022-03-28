#ifndef MOTOR_H
#define MOTOR_H

/**
 * Initialize angular velocity control motor
 * Consumes GPIO pins 14 & 15
 */
void motor_init();

/**
 * Write speed to angular velocity control motor
 * @param value Duty cycle between 0 (always off) and 255 (always on)
 * @param direction Rotation direction of 0 (forwards) or 1 (backwards)
 */
void motor_write(uint16_t value, bool direction);

#endif // MOTOR_H
