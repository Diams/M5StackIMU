#include "logger.h"

#include <M5Stack.h>

#include <string>

#include "../clocks/wifi_clock.h"

static const char extention[] = ".csv";

Logger::Logger() {
  memset(file_path_, 0, sizeof(file_path_));
  is_initialized_ = false;
}

Logger::~Logger() {}

void Logger::Initialize(void) {
  if (theWifiClock.IsInitialized()) {
    struct tm current_time;
    theWifiClock.CurrentTime(&current_time);
    snprintf(file_path_, sizeof(file_path_), "/%04d%02d%02d%02d%02d%02d.csv", current_time.tm_year + 1900,
             current_time.tm_mon + 1, current_time.tm_mday, current_time.tm_hour, current_time.tm_min,
             current_time.tm_sec);
  } else {
    for (int i = 1;; i++) {
      std::string file_path = "/" + std::to_string(i) + std::string(extention);
      if (!SD.exists(file_path.c_str())) {
        memset(file_path_, 0, sizeof(file_path_));
        memcpy(file_path_, file_path.c_str(), strlen(file_path.c_str()));
        break;
      }
    }
  }
  is_initialized_ = true;
}

void Logger::Save(ImuData imu_data) {
  if (!is_initialized_) return;
  bool exists = SD.exists(file_path_);
  File file = SD.open(file_path_, FILE_APPEND);
  if (!exists) {
    file.printf(",acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,pitch,roll,yaw\n");
  }
  file.printf("%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", millis(), imu_data.acceleration_sensor.x,
              imu_data.acceleration_sensor.y, imu_data.acceleration_sensor.z, imu_data.gyro_sensor.x,
              imu_data.gyro_sensor.y, imu_data.gyro_sensor.z, imu_data.ahrs.pitch, imu_data.ahrs.roll,
              imu_data.ahrs.yaw);
  file.close();
}
