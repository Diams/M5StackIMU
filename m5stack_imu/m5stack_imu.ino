#include <M5Stack.h>

#include "src/clocks/wifi_clock.h"
#include "src/imu/imu.h"
#include "src/logs/logger.h"

static const float kSamplingPeriod = 100;

static ImuData currentImuData;
static char filePath[128];
static bool isWritable = false;

static Logger theLogger;

static TaskHandle_t samplingImuTaskHandler;
static TaskHandle_t savingImuTaskHandler;
static SemaphoreHandle_t xMutex = NULL;

static uint8_t GetButtonStatus(void);
static void ExecuteButtonFunction(uint8_t button_status);
static void AButtonFunction(void);
static void BButtonFunction(void);
static void CButtonFunction(void);

static void SamplingImuTask(void* pvParameter);

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
  theLogger.Initialize(filePath, (int)kSamplingPeriod);
  xMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(SamplingImuTask, "SamplingImuTask", 4096, NULL, 2, &samplingImuTaskHandler, 0);
}

void loop(void) {
  M5.update();
  ExecuteButtonFunction(GetButtonStatus());
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
  currentImuData = {
      .acceleration_sensor = {.x = ax, .y = ay, .z = az},
      .gyro_sensor = {.x = gx, .y = gy, .z = gz},
      .ahrs = {.pitch = pitch, .roll = roll, .yaw = yaw},
  };
  isWritable = true;
  File file = SD.open(filePath, FILE_APPEND);
  if (file) {
    if (xSemaphoreTake(xMutex, 0) == pdTRUE) {
      theLogger.Save(file);
      xSemaphoreGive(xMutex);
    }
    file.close();
  }
}

static uint8_t GetButtonStatus(void) {
  uint8_t ret = 0;
  if (M5.BtnA.wasPressed()) {
    ret |= 0b100;
  }
  if (M5.BtnB.wasPressed()) {
    ret |= 0b010;
  }
  if (M5.BtnC.wasPressed()) {
    ret |= 0b001;
  }
  return ret;
}

static void ExecuteButtonFunction(uint8_t button_status) {
  if (button_status & 0b100) {
    AButtonFunction();
  }
  if (button_status & 0b010) {
    BButtonFunction();
  }
  if (button_status & 0b001) {
    CButtonFunction();
  }
}

static void AButtonFunction(void) {}

static void BButtonFunction(void) {}

static void CButtonFunction(void) {}

static void SamplingImuTask(void* pvParameter) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (true) {
    if (isWritable) {
      if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        theLogger.Sample(millis(), currentImuData);
        xSemaphoreGive(xMutex);
      }
    }
    delay(1);
    xTaskDelayUntil(&xLastWakeTime, kSamplingPeriod / portTICK_PERIOD_MS);
  }
}
