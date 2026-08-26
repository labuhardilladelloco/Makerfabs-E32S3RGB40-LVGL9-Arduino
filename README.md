[README.md](https://github.com/user-attachments/files/31488588/README.md)
# MaTouch E32S3RGB40 — Arduino IDE 2 + SquareLine + LVGL 9.5

Plantilla para ESP32-S3-WROOM-1 N16R8, pantalla ST7701S RGB 480x480 y GT911.
Utiliza el driver RGB nativo `esp_lcd` de Arduino-ESP32 3.x; no depende de
Arduino_GFX, LovyanGFX ni de APIs LVGL 8.

## Arduino IDE 2

Selecciona **ESP32S3 Dev Module** y configura:

- USB CDC On Boot: Enabled
- Flash Size: 16MB (128Mb)
- Partition Scheme: 16M Flash (3MB APP/9.9MB FATFS), o cualquier partición APP >= 2MB
- PSRAM: OPI PSRAM
- Upload Mode: UART0 / Hardware CDC

Instala LVGL 9.5.0 desde Library Manager. Abre
`MaTouch_SquareLine_Template.ino`, selecciona el puerto y pulsa Upload.

## SquareLine

Crea el proyecto con LVGL 9.5, C, 480x480 y 16-bit. Exporta solo los archivos
UI directamente en `src/ui`. La lógica permanente debe permanecer en
`src/app`, fuera de la carpeta regenerada.

La plantilla acepta tanto `lvgl.h` como `lvgl/lvgl.h` en la configuración de
SquareLine. Para empezar un proyecto SquareLine distinto, elimina los archivos
generados anteriores de `src/ui`, pero conserva `src/ui/lvgl/lvgl.h`. Después
usa **Export UI Files** (no Template Project) con `src/ui` como destino. No
elimines las carpetas hermanas `src/app` ni `src/board`.

Las acciones visuales configuradas en SquareLine funcionan sin editar código.
Solo es necesario programar `src/app/app_ui.cpp` cuando la interfaz deba llamar
hardware, Wi-Fi, sensores, JSON u otra lógica propia.
