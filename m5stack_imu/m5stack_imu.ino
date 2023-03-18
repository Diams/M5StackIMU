#include <M5Stack.h>

#include <string>

#include "src/clocks/wifi_clock.h"
#include "src/imu/imu.h"
#include "src/logs/logger.h"

static const float kSamplingPeriod = 100;

static ImuData currentImuData;
static char filePath[128];
static bool isWritable = false;
static bool isSavable = false;

static Logger theLogger;

static TaskHandle_t samplingImuTaskHandler;
static TaskHandle_t savingImuTaskHandler;
static SemaphoreHandle_t xMutex = NULL;

static uint8_t GetButtonStatus(void);
static void ExecuteButtonFunction(uint8_t button_status);
static void AButtonFunction(void);
static void BButtonFunction(void);
static void CButtonFunction(void);
static void ShowAButtonLabel(void);

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
  if (isSavable) {
    File file = SD.open(filePath, FILE_APPEND);
    if (file) {
      if (xSemaphoreTake(xMutex, 0) == pdTRUE) {
        theLogger.Save(file);
        xSemaphoreGive(xMutex);
      }
      file.close();
    } else {
      isSavable = false;
    }
  }
  ShowAButtonLabel();
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

static void AButtonFunction(void) {
  isSavable = !isSavable;
  if (isSavable) {
    if (SD.begin(TFCARD_CS_PIN)) {
      theLogger.Initialize(filePath, (int)kSamplingPeriod);
    }
  }
}

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

static void ShowAButtonLabel(void) {
  int32_t x_margin = 5;
  int32_t y_margin = 5;
  int32_t rect_width = (M5.Lcd.width() / 3) - (x_margin * 2);
  int32_t rect_height = 40;
  int32_t rect_x = 0 + x_margin;
  int32_t rect_y = M5.Lcd.height() - y_margin - rect_height;
  M5.Lcd.drawRect(rect_x, rect_y, rect_width, rect_height, WHITE);
  std::string status_text = isSavable ? " STOP " : "START";
  M5.Lcd.setTextDatum(4);
  M5.Lcd.drawString(status_text.c_str(), rect_x + rect_width / 2, rect_y + rect_height / 2, 1);
}
