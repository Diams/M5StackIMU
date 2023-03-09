#include "logger.h"

#include <M5Stack.h>

#include <string>

static const char extention[] = ".csv";

Logger::Logger() { file_number_ = 0; }

Logger::~Logger() {}

void Logger::Initialize(void) {
  for (int i = 1;; i++) {
    std::string file_path = "/" + std::to_string(i) + std::string(extention);
    if (!SD.exists(file_path.c_str())) {
      file_number_ = i;
      break;
    }
  }
}

void Logger::Save(ImuData imu_data) {}
