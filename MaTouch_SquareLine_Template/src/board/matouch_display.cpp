#include "matouch_display.h"

#include <Arduino.h>
#include <esp_heap_caps.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

namespace {
constexpr int kWidth = 480;
constexpr int kHeight = 480;
constexpr int kCs = 1;
constexpr int kSck = 12;
constexpr int kSda = 11;
constexpr int kBufferLines = 40;

esp_lcd_panel_handle_t panel;
lv_display_t *display;

struct InitCommand {
    uint8_t command;
    uint8_t data[16];
    uint8_t length;
    uint16_t delay_ms;
};

const InitCommand init_commands[] = {
    {0xFF,{0x77,0x01,0x00,0x00,0x10},5,0}, {0xC0,{0x3B,0x00},2,0},
    {0xC1,{0x0D,0x02},2,0}, {0xC2,{0x31,0x05},2,0}, {0xCD,{0x08},1,0},
    {0xB0,{0x00,0x11,0x18,0x0E,0x11,0x06,0x07,0x08,0x07,0x22,0x04,0x12,0x0F,0xAA,0x31,0x18},16,0},
    {0xB1,{0x00,0x11,0x19,0x0E,0x12,0x07,0x08,0x08,0x08,0x22,0x04,0x11,0x11,0xA9,0x32,0x18},16,0},
    {0xFF,{0x77,0x01,0x00,0x00,0x11},5,0}, {0xB0,{0x60},1,0},
    {0xB1,{0x32},1,0}, {0xB2,{0x07},1,0}, {0xB3,{0x80},1,0},
    {0xB5,{0x49},1,0}, {0xB7,{0x85},1,0}, {0xB8,{0x21},1,0},
    {0xC1,{0x78},1,0}, {0xC2,{0x78},1,0}, {0xE0,{0x00,0x1B,0x02},3,0},
    {0xE1,{0x08,0xA0,0x00,0x00,0x07,0xA0,0x00,0x00,0x00,0x44,0x44},11,0},
    {0xE2,{0x11,0x11,0x44,0x44,0xED,0xA0,0x00,0x00,0xEC,0xA0,0x00,0x00},12,0},
    {0xE3,{0x00,0x00,0x11,0x11},4,0}, {0xE4,{0x44,0x44},2,0},
    {0xE5,{0x0A,0xE9,0xD8,0xA0,0x0C,0xEB,0xD8,0xA0,0x0E,0xED,0xD8,0xA0,0x10,0xEF,0xD8,0xA0},16,0},
    {0xE6,{0x00,0x00,0x11,0x11},4,0}, {0xE7,{0x44,0x44},2,0},
    {0xE8,{0x09,0xE8,0xD8,0xA0,0x0B,0xEA,0xD8,0xA0,0x0D,0xEC,0xD8,0xA0,0x0F,0xEE,0xD8,0xA0},16,0},
    {0xEB,{0x02,0x00,0xE4,0xE4,0x88,0x00,0x40},7,0}, {0xEC,{0x3C,0x00},2,0},
    {0xED,{0xAB,0x89,0x76,0x54,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x20,0x45,0x67,0x98,0xBA},16,0},
    {0xFF,{0x77,0x01,0x00,0x00,0x13},5,0}, {0xE5,{0xE4},1,0},
    {0xFF,{0x77,0x01,0x00,0x00,0x00},5,0}, {0x21,{},0,0},
    {0x3A,{0x60},1,0}, {0x11,{},0,120}, {0x29,{},0,20},
};

void write_9bit(bool data, uint8_t value) {
    digitalWrite(kSck, LOW);
    digitalWrite(kSda, data ? HIGH : LOW);
    digitalWrite(kSck, HIGH);
    for (int bit = 7; bit >= 0; --bit) {
        digitalWrite(kSck, LOW);
        digitalWrite(kSda, (value >> bit) & 1);
        digitalWrite(kSck, HIGH);
    }
}

void send_command(const InitCommand &item) {
    digitalWrite(kCs, LOW);
    write_9bit(false, item.command);
    for (uint8_t i = 0; i < item.length; ++i) write_9bit(true, item.data[i]);
    digitalWrite(kCs, HIGH);
    if (item.delay_ms) delay(item.delay_ms);
}

void flush(lv_display_t *disp, const lv_area_t *area, uint8_t *pixels) {
    esp_lcd_panel_draw_bitmap(panel, area->x1, area->y1, area->x2 + 1, area->y2 + 1, pixels);
    lv_display_flush_ready(disp);
}
}  // namespace

bool matouch_display_begin() {
    if (!psramFound()) return false;

    pinMode(kCs, OUTPUT); pinMode(kSck, OUTPUT); pinMode(kSda, OUTPUT);
    digitalWrite(kCs, HIGH); digitalWrite(kSck, HIGH); digitalWrite(kSda, HIGH);
    for (const auto &item : init_commands) send_command(item);

    esp_lcd_rgb_panel_config_t config = {};
    config.clk_src = LCD_CLK_SRC_DEFAULT;
    config.timings.pclk_hz = 14000000;
    config.timings.h_res = kWidth; config.timings.v_res = kHeight;
    config.timings.hsync_pulse_width = 8; config.timings.hsync_back_porch = 50;
    config.timings.hsync_front_porch = 10; config.timings.vsync_pulse_width = 8;
    config.timings.vsync_back_porch = 20; config.timings.vsync_front_porch = 10;
    config.data_width = 16; config.bits_per_pixel = 16;
    config.num_fbs = 1; config.bounce_buffer_size_px = kWidth * 20;
    config.hsync_gpio_num = 5; config.vsync_gpio_num = 4; config.de_gpio_num = 45;
    config.pclk_gpio_num = 21; config.disp_gpio_num = -1;
    const int pins[16] = {6,7,15,16,8, 0,9,14,47,48,3, 39,40,41,42,2};
    for (int i = 0; i < 16; ++i) config.data_gpio_nums[i] = pins[i];
    config.flags.fb_in_psram = true;
    if (esp_lcd_new_rgb_panel(&config, &panel) != ESP_OK) return false;
    if (esp_lcd_panel_reset(panel) != ESP_OK || esp_lcd_panel_init(panel) != ESP_OK) return false;

    display = lv_display_create(kWidth, kHeight);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(display, flush);
    const size_t bytes = kWidth * kBufferLines * sizeof(lv_color_t);
    void *buffer1 = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    void *buffer2 = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buffer1 || !buffer2) return false;
    lv_display_set_buffers(display, buffer1, buffer2, bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
    return true;
}

lv_display_t *matouch_display_get_lvgl() { return display; }
