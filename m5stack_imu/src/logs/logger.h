#ifndef M5STACK_IMU_SRC_LOGS_LOGGER_H_
#define M5STACK_IMU_SRC_LOGS_LOGGER_H_

#include <FS.h>

#include "../imu/imu.h"

class Logger {
 public:
  Logger();
  ~Logger();
  void Initialize(char* file_path, int sampling_period);
  void Sample(unsigned long timestamp, ImuData imu_data);
  void Save(File file);

 private:
  char file_path_[19 + 1];
  bool is_initialized_;
  int multipule_buffer_index_;
  int written_index_;
  bool is_loggable_;
  int buffer_size_;
};

#endif  // M5STACK_IMU_SRC_LOGS_LOGGER_H_
