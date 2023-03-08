#include <M5Stack.h>

void setup(void) {
  M5.begin();
  M5.Imu.Init();
  M5.Lcd.setTextSize(2);
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
  delay(1000);
}
