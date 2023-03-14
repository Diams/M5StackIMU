#include <M5Stack.h>

#include "src/clocks/wifi_clock.h"
#include "src/imu/imu.h"
#include "src/logs/logger.h"

static Logger theLogger;

static TaskHandle_t samplingImuTaskHandler;
static TaskHandle_t savingImuTaskHandler;

static void SamplingImuTask(void* pvParameter);
static void SavingLogTask(void* pvParameter);

void setup(void) {
  M5.begin();
  M5.Imu.Init();
  M5.Lcd.setTextSize(2);
  while (!SD.begin(TFCARD_CS_PIN)) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("TF card isn't mounted.\n");
  }
  M5.Lcd.clear();
  theWifiClock.Initialize();
  theLogger.Initialize();
  xTaskCreatePinnedToCore(SamplingImuTask, "SamplingImuTask", 4096, NULL, 2, &samplingImuTaskHandler, 1);
  xTaskCreatePinnedToCore(SavingLogTask, "SavingLogTask", 4096, NULL, 0, &savingImuTaskHandler, 0);
}

void loop(void) {
  float ax, ay, az;
  M5.Imu.getAccelData(&ax, &ay, &az);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("acc:% 3.2f,% 3.2f,% 3.2f", ax, ay, az);
  float gx, gy, gz;
  M5.Imu.getGyroData(&gx, &gy, &gz);
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.printf("gyro:% 4d,% 4d,% 4d", (int)gx, (int)gy, (int)gz);
  float pitch, roll, yaw;
  M5.Imu.getAhrsData(&pitch, &roll, &yaw);
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.printf("arhs:% 4d,% 4d,% 4d", (int)pitch, (int)roll, (int)yaw);
  ImuData imu_data = {
      .acceleration_sensor = {.x = ax, .y = ay, .z = az},
      .gyro_sensor = {.x = gx, .y = gy, .z = gz},
      .ahrs = {.pitch = pitch, .roll = roll, .yaw = yaw},
  };
}

static void SamplingImuTask(void* pvParameter) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (true) {
    delay(1);
    xTaskDelayUntil(&xLastWakeTime, 100.0 / portTICK_PERIOD_MS);
  }
}

static void SavingLogTask(void* pvParameter) {
  while (true) {
    delay(1);
  }
}
