#ifndef PID_H
#define PID_H

typedef struct {
  int k_p;      // Proportional constant
  int k_i;      // Integration constant
  int k_d;      // Derivative constant
  int last_err; // Instantaneous error at previous calculation
  int err_sum;  // Reimann sum of error
  long last_t;  // Clock time at previous calculation
} pid;

void pid_init(pid* pid, int k_p, int k_i, int k_d);

int pid_calculate(pid* pid, int target, int position, long time);

#endif // !PID_H
