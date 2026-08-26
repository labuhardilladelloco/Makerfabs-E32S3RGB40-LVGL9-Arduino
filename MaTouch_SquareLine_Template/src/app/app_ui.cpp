#include "app_ui.h"

#include <Arduino.h>

void app_ui_begin() {
    // Intentionally independent from generated widget names. Add permanent
    // application logic here only when a project needs hardware, Wi-Fi, JSON,
    // sensors, or custom callbacks not handled directly by SquareLine.
    Serial.println("SquareLine application hooks ready");
}
