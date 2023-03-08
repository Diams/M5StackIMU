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
  M5.Lcd.printf("acc:% 3.2f, % 3.2f, % 3.2f", ax, ay, az);
  delay(1000);
}
