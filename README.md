# MaTouch ESP32-S3 4" — LVGL 9.5 + SquareLine + Arduino IDE 2

Plantilla moderna para utilizar la **Makerfabs MaTouch ESP32-S3 Parallel TFT with Touch 4.0"** con **Arduino IDE 2**, **Arduino-ESP32 3.x**, **LVGL 9.5** y **SquareLine Studio**.

Compatible con:

- **MCU:** ESP32-S3-WROOM-1 N16R8
- **Pantalla:** ST7701S RGB 480×480
- **Touch:** GT911
- **LVGL:** 9.5
- **Arduino IDE:** 2.x
- **Arduino-ESP32:** 3.x

Utiliza directamente el driver RGB nativo `esp_lcd` de Arduino-ESP32 y no depende de **Arduino_GFX**, **LovyanGFX**, forks modificados de LVGL ni APIs de LVGL 8.

<p align="center">
  <img src="docs/images/matouch-lvgl9.jpg"
       alt="Makerfabs MaTouch ESP32-S3 ejecutando LVGL 9.5"
       width="650">
</p>

> La plantilla ha sido probada desde una instalación limpia en un segundo equipo: Arduino IDE 2 + Arduino-ESP32 + LVGL 9.5 → abrir → compilar → subir.


## Características

- LVGL **9.5**
- Arduino IDE **2.x**
- Arduino-ESP32 **3.x**
- Driver RGB nativo `esp_lcd`
- Soporte para pantalla **ST7701S 480×480**
- Soporte táctil **GT911**
- PSRAM
- Integración con **SquareLine Studio**
- Separación entre hardware, interfaz y lógica de aplicación
- No requiere modificar los archivos del driver para crear un nuevo proyecto
- Preparada para utilizar Wi-Fi, MQTT, sensores, JSON y hardware externo desde `src/app`


## Instalación rápida

### 1. Instala Arduino IDE 2

Instala una versión actual de **Arduino IDE 2.x**.


### 2. Instala el soporte ESP32

Desde **Boards Manager**, instala:

`esp32 by Espressif Systems`

Selecciona posteriormente:

**ESP32S3 Dev Module**


### 3. Instala LVGL

Desde **Library Manager**, instala:

**LVGL 9.5.0**


### 4. Descarga este repositorio

Puedes clonar el repositorio o descargarlo como ZIP.

Abre:

```text
MaTouch_SquareLine_Template/MaTouch_SquareLine_Template.ino
```


### 5. Configura la placa

Utiliza la siguiente configuración en Arduino IDE:

| Opción | Valor |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enabled |
| Flash Size | 16MB (128Mb) |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| PSRAM | OPI PSRAM |
| Upload Mode | UART0 / Hardware CDC |

También puede utilizarse otra partición siempre que la partición **APP sea ≥ 2MB**.


### 6. Upload

Selecciona el puerto correspondiente y pulsa **Upload**.

Si todo está correctamente configurado, la interfaz incluida en la plantilla aparecerá directamente en la MaTouch y el controlador táctil estará operativo.

No es necesario instalar Arduino_GFX, LovyanGFX ni una versión modificada de LVGL.


## Configuración probada

La plantilla se ha probado desde una instalación limpia en un equipo diferente al utilizado durante su desarrollo.

| Componente | Versión |
|---|---|
| Arduino IDE | 2.x |
| Arduino-ESP32 | 3.x |
| LVGL | 9.5.0 |
| SquareLine Studio | *Añadir versión probada* |
| MCU | ESP32-S3-WROOM-1 N16R8 |
| Display | ST7701S RGB 480×480 |
| Touch | GT911 |

> Se recomienda utilizar las versiones indicadas en esta tabla para reproducir la configuración probada.


## SquareLine Studio

La plantilla está preparada para que la interfaz pueda diseñarse en **SquareLine Studio** y exportarse directamente al proyecto.

<p align="center">
  <img src="docs/images/squareline-project.png"
       alt="Interfaz para MaTouch diseñada con SquareLine Studio"
       width="750">
</p>


### Crear el proyecto

Configura el proyecto de SquareLine con:

| Opción | Valor |
|---|---|
| LVGL | 9.5 |
| Language | C |
| Resolution | 480×480 |
| Color depth | 16-bit |


### Exportar la interfaz

Utiliza:

**Export UI Files**

No utilices **Template Project**.

Selecciona como destino:

```text
MaTouch_SquareLine_Template/src/ui
```

<p align="center">
  <img src="docs/images/squareline-export.png"
       alt="Exportación de la interfaz SquareLine a src/ui"
       width="750">
</p>

La plantilla acepta tanto:

```text
lvgl.h
```

como:

```text
lvgl/lvgl.h
```

en la configuración de SquareLine.


### Utilizar un proyecto SquareLine diferente

Para sustituir la interfaz incluida por una nueva:

1. Elimina los archivos generados anteriormente dentro de `src/ui`.
2. Conserva `src/ui/lvgl/lvgl.h`.
3. En SquareLine utiliza **Export UI Files**.
4. Selecciona `src/ui` como destino.
5. Compila nuevamente el proyecto desde Arduino IDE.
6. Pulsa **Upload**.

No elimines:

```text
src/app
src/board
```

Estas carpetas no son generadas por SquareLine.


## ¿Dónde programo mi aplicación?

La plantilla separa deliberadamente tres partes:

```text
BOARD → Hardware y drivers
UI    → Interfaz generada por SquareLine
APP   → Lógica de tu aplicación
```

La lógica permanente de tu proyecto debe escribirse en:

```text
src/app
```

Por ejemplo:

- Wi-Fi
- MQTT
- sensores
- relés
- LEDs
- peticiones HTTP
- JSON
- Home Assistant
- comunicación con otros ESP32
- callbacks personalizados


### Ejemplo

SquareLine puede crear visualmente un botón o un `Switch`.

La interfaz generada permanece en:

```text
src/ui
```

Si ese `Switch` necesita controlar hardware real, la lógica correspondiente debe implementarse desde:

```text
src/app/app_ui.cpp
```

De esta forma puedes volver a exportar la interfaz desde SquareLine sin mezclar la lógica principal de la aplicación con los archivos generados.


## Esquema del flujo

```text
       SquareLine Studio
              │
              ▼
          Export UI
              │
              ▼
            src/ui
              │
              ▼
       Arduino IDE 2
              │
              ▼
           Compile
              │
              ▼
            Upload
              │
              ▼
      MaTouch ESP32-S3
```


## Arquitectura

```text
MaTouch_SquareLine_Template/
│
├── MaTouch_SquareLine_Template.ino
│
└── src/
    │
    ├── board/
    │   └── ST7701S + GT911 + LVGL display/input
    │
    ├── ui/
    │   └── Interfaz generada por SquareLine
    │
    └── app/
        └── Lógica permanente de la aplicación
```

La separación permite modificar o regenerar la interfaz sin tener que reescribir los drivers de la pantalla y el controlador táctil.


## Hardware soportado

Este proyecto ha sido desarrollado específicamente para:

**Makerfabs MaTouch ESP32-S3 Parallel TFT with Touch 4.0"**

Hardware utilizado:

```text
ESP32-S3-WROOM-1 N16R8
ST7701S RGB 480×480
GT911 Capacitive Touch
```

> Esta plantilla utiliza la configuración de hardware específica de esta placa. No se garantiza compatibilidad directa con otras pantallas MaTouch o placas ESP32-S3 aunque utilicen componentes similares.


## Estructura recomendada

```text
repository/
│
├── README.md
├── LICENSE
│
├── docs/
│   └── images/
│       ├── matouch-lvgl9.jpg
│       ├── squareline-project.png
│       └── squareline-export.png
│
└── MaTouch_SquareLine_Template/
    ├── MaTouch_SquareLine_Template.ino
    └── src/
        ├── board/
        ├── ui/
        └── app/
```


## Disclaimer

This is an independent community project by **LaBuhardillaDelLoco** and is not an official Makerfabs library or officially supported Makerfabs software.

Makerfabs and MaTouch are trademarks or product names of their respective owners.


## License

This project is licensed under the **MIT License**.

Copyright © 2026 **LaBuhardillaDelLoco**.

You are free to use, modify, and redistribute this software under the terms of the MIT License. The original copyright and license notice must be preserved in copies or substantial portions of the software.

See the [LICENSE](LICENSE) file for the full license text.
