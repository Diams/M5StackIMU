#ifndef M5STACK_IMU_SRC_LOGS_LOGGER_H_
#define M5STACK_IMU_SRC_LOGS_LOGGER_H_

#include "../imu/imu.h"

class Logger {
 public:
  Logger();
  ~Logger();
  void Initialize(void);
  void Save(ImuData imu_data);

 private:
  char file_path_[19 + 1];
  bool is_initialized_;
};

#endif  // M5STACK_IMU_SRC_LOGS_LOGGER_H_
