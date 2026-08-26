#include "matouch_touch.h"

#include <Arduino.h>
#include <Wire.h>

namespace {
constexpr uint8_t kAddress = 0x5D;
constexpr int kWidth = 480;
lv_indev_t *indev;

bool read_register(uint16_t reg, uint8_t *data, size_t length) {
    Wire.beginTransmission(kAddress);
    Wire.write(reg >> 8); Wire.write(reg & 0xFF);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(kAddress, static_cast<uint8_t>(length)) != length) return false;
    for (size_t i = 0; i < length; ++i) data[i] = Wire.read();
    return true;
}

void clear_status() {
    Wire.beginTransmission(kAddress);
    Wire.write(0x81); Wire.write(0x4E); Wire.write(0);
    Wire.endTransmission();
}

void read_touch(lv_indev_t *, lv_indev_data_t *data) {
    data->state = LV_INDEV_STATE_RELEASED;
    uint8_t status = 0;
    if (!read_register(0x814E, &status, 1)) {
        static uint32_t last_error_ms;
        if (millis() - last_error_ms > 1000) {
            Serial.println("GT911: I2C read error");
            last_error_ms = millis();
        }
        return;
    }
    if (!(status & 0x80)) {
        clear_status();
        return;
    }
    const uint8_t count = status & 0x0F;
    if (count > 0 && count <= 5) {
        uint8_t point[8];
        if (read_register(0x8150, point, sizeof(point))) {
            // Register 0x814F is the track ID. Starting at 0x8150 means
            // point[0..3] are X-low, X-high, Y-low and Y-high.
            const int x = point[0] | (point[1] << 8);
            const int y = point[2] | (point[3] << 8);
            data->point.x = constrain(x, 0, kWidth - 1);
            data->point.y = constrain(y, 0, kWidth - 1);
            data->state = LV_INDEV_STATE_PRESSED;
        }
    }
    clear_status();
}
}  // namespace

bool matouch_touch_begin() {
    pinMode(38, OUTPUT); digitalWrite(38, LOW); delay(10);
    digitalWrite(38, HIGH); delay(50);
    Wire.begin(17, 18, 400000);
    uint8_t product_id[3];
    if (!read_register(0x8140, product_id, sizeof(product_id))) return false;
    Serial.printf("GT911 product ID: %c%c%c at 0x%02X\n",
                  product_id[0], product_id[1], product_id[2], kAddress);
    clear_status();
    return true;
}

void matouch_touch_attach_lvgl(lv_display_t *display) {
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_display(indev, display);
    lv_indev_set_read_cb(indev, read_touch);
}
