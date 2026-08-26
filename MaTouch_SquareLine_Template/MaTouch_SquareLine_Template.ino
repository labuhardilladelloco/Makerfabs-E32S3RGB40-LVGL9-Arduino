#include <Arduino.h>
#include <lvgl.h>

#include "src/app/app_ui.h"
#include "src/board/matouch_display.h"
#include "src/board/matouch_touch.h"
#include "src/ui/ui.h"

namespace {
uint32_t last_tick_ms;
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\nMaTouch Arduino + LVGL 9.5");

    lv_init();
    if (!matouch_display_begin()) {
        Serial.println("ERROR: display initialization failed");
        return;
    }
    if (!matouch_touch_begin()) {
        Serial.println("ERROR: touch initialization failed");
        return;
    }
    matouch_touch_attach_lvgl(matouch_display_get_lvgl());

    ui_init();
    app_ui_begin();
    last_tick_ms = millis();
    Serial.println("Display, touch, LVGL and SquareLine UI initialized");
}

void loop() {
    const uint32_t now = millis();
    lv_tick_inc(now - last_tick_ms);
    last_tick_ms = now;
    const uint32_t wait_ms = lv_timer_handler();
    delay(constrain(wait_ms, 2U, 10U));
}
