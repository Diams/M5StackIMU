#include "logger.h"

#include <M5Stack.h>

#include <string>

#include "../clocks/wifi_clock.h"

#define COUNTOF(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct {
  unsigned long timestamp;
  ImuData data;
} ImuRecord;

static const char extention[] = ".csv";
static const int kSavingPeriod = 2000;

static ImuRecord buffer[2][512];
static int nBuffers = COUNTOF(buffer);

Logger::Logger() {
  memset(file_path_, 0, sizeof(file_path_));
  is_initialized_ = false;
  multipule_buffer_index_ = 0;
  written_index_ = 0;
  is_loggable_ = false;
  buffer_size_ = 0;
}

Logger::~Logger() {}

void Logger::Initialize(int sampling_period) {
  buffer_size_ = kSavingPeriod / sampling_period;
  if (buffer_size_ == 0) {
    buffer_size_ = 1;
  } else if (buffer_size_ > COUNTOF(buffer[0])) {
    buffer_size_ = COUNTOF(buffer[0]);
  }
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

void Logger::Sample(unsigned long timestamp, ImuData imu_data) {
  ImuRecord record = {.timestamp = timestamp, .data = imu_data};
  buffer[multipule_buffer_index_][written_index_] = record;
  written_index_++;
  if (written_index_ == buffer_size_) {
    multipule_buffer_index_ = (multipule_buffer_index_ + 1) % nBuffers;
    written_index_ = 0;
    is_loggable_ = true;
  }
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
