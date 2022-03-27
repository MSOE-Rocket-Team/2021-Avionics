#include "pid.h"

int calculate_p(pid*, int);
int calculate_i(pid*, int, long);
int calculate_d(pid*, int, long);

void pid_init(pid* pid, int k_p, int k_i, int k_d) {
  pid->k_p = k_p;
  pid->k_i = k_i;
  pid->k_d = k_d;
  pid->err_sum = 0;
  pid->last_err = 0;
  pid->last_t = 0;
}

int pid_calculate(pid* pid, int target, int position, long time) {
  int error = target - position;
  long delta_t = time - pid->last_t;
  pid->last_t = time;
}

int calculate_p(pid* pid, int error) {
  return pid->k_p * error;
}

int calculate_i(pid* pid, int error, long delta_t) {
  if (error / error != pid->last_err / pid->last_err) {
    pid->err_sum = 0;
  } else {
    pid->err_sum += error * delta_t;
  }
  return pid->k_i * pid->err_sum;
}

int calculate_d(pid* pid, int error, long delta_t) {
  return pid->k_d * (error - pid->last_err) / delta_t;
}
