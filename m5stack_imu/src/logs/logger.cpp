#include "logger.h"

#include <M5Stack.h>

#include <string>

static const char extention[] = ".csv";

Logger::Logger() {
  file_number_ = 0;
  is_initialized_ = false;
}

Logger::~Logger() {}

void Logger::Initialize(void) {
  for (int i = 1;; i++) {
    std::string file_path = "/" + std::to_string(i) + std::string(extention);
    if (!SD.exists(file_path.c_str())) {
      file_number_ = i;
      break;
    }
  }
  is_initialized_ = true;
}

void Logger::Save(ImuData imu_data) {
  if (!is_initialized_) return;
  std::string file_path = "/" + std::to_string(file_number_) + std::string(extention);
  bool exists = SD.exists(file_path.c_str());
  File file = SD.open(file_path.c_str(), FILE_APPEND);
  if (!exists) {
    file.printf(",acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,pitch,roll,yaw\n");
  }
  file.printf("%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", millis(), imu_data.acceleration_sensor.x,
              imu_data.acceleration_sensor.y, imu_data.acceleration_sensor.z, imu_data.gyro_sensor.x,
              imu_data.gyro_sensor.y, imu_data.gyro_sensor.z, imu_data.ahrs.pitch, imu_data.ahrs.roll,
              imu_data.ahrs.yaw);
  file.close();
}
