#include "wifi_clock.h"

#include <M5Stack.h>
#include <WiFi.h>

static const char* kSsid = "rx600k-4a41c2-1";
static const char* kPassword = "89a51ad27ae2d";
static const char* kNtpServer = "ntp.nict.jp";

void WifiClock::Initialize(void) {
  WiFi.begin(kSsid, kPassword);
  int wifi_retry_counter = 20;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (--wifi_retry_counter == 0) {
      WiFi.disconnect(true);
      Serial.printf("Failed to connect to the access point.\n");
      WiFi.mode(WIFI_OFF);
      return;
    }
  }
  configTime(9 * 60 * 60, 0, kNtpServer);
  struct tm tmp;
  if (!getLocalTime(&tmp)) {
    Serial.printf("Failed to obtain time.\n");
  }
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

bool WifiClock::CurrentTime(struct tm* time_info) {}

WifiClock theWifiClock;
