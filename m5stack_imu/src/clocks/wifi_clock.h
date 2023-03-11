#ifndef M5STACK_IMU_SRC_CLOCKS_WIFI_CLOCK_H_
#define M5STACK_IMU_SRC_CLOCKS_WIFI_CLOCK_H_

#include <time.h>

class WifiClock {
 public:
  WifiClock() {}
  ~WifiClock() {}
  void Initialize(void);
  bool CurrentTime(struct tm* time_info);
};

extern WifiClock theWifiClock;

#endif  // M5STACK_IMU_SRC_CLOCKS_WIFI_CLOCK_H_
