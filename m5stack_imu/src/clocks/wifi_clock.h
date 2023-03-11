#ifndef M5STACK_IMU_SRC_CLOCKS_WIFI_CLOCK_H_
#define M5STACK_IMU_SRC_CLOCKS_WIFI_CLOCK_H_

#include <time.h>

class WifiClock {
 public:
  WifiClock() { is_initialized_ = false; }
  ~WifiClock() {}
  void Initialize(void);
  void CurrentTime(struct tm* time_info);
  bool IsInitialized(void);

 private:
  bool is_initialized_;
};

extern WifiClock theWifiClock;

#endif  // M5STACK_IMU_SRC_CLOCKS_WIFI_CLOCK_H_
