# Wi-Fi egg bot

[![Donate original author](https://img.shields.io/badge/Donate-PayPal-green.svg?style=flat-square&logo=paypal)](https://paypal.me/andreitatar)

Based on the M5stack core platform (ESP32), this project aims to provide a refresh to the original [Egg Bot](https://egg-bot.com/) giving it the ability to load/slice/save/print files without the use of a PC.

If you like the project, find it useful and want to support the original author, consider donating [PayPal](https://paypal.me/andreitatar).

## Demo

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/auzUlywlYKI/0.jpg)](https://www.youtube.com/watch?v=auzUlywlYKI)

## Development work

### Remaining things for a first release

- [x] WiFi setup - start firmware (or switch if unable to connect to a network) in AP mode. Allow changing the mode/wi-fi settings from the web client
- [ ] M5 UI - update M5 code to include UI to: start/pause/stop print, view progress and Wi-Fi settings (AP settings and/or IP in STA mode)
- [ ] OTA - easier firmware update (removing dependencies for platformio or other tools after the first flash)

### Lower priority

- [ ] Move image processing in web worker
- [ ] Make project compatible with generic ESP32 boards

## HW Connections to M5stack core

| Function                | Pin  |
|-------------------------|------|
| Rotation Motor DIR      | 2    |
| Rotation Motor STEP     | 17   |
| Rotation Motor RESET    | 16   |
| Pen Motor DIR           | 5    |
| Pen Motor STEP          | 26   |
| Pen Motor RESET         | 22   |
| Servo                   | 21   |

![M5Stack Back](doc/m5-back.jpg)

## Building

### Web client (angular based)

Dependencies: [node.js](https://nodejs.org/en/)

```
cd eggduino-client
npm install 
npm run build
```

### M5 code

Dependencies: [PlatformIO](https://platformio.org/platformio-ide)

I personally use [Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) extension.

```
platformio run                             - build code
platformio run --target upload             - upload code
platformio run --target uploadfs           - upload web client files
```

## Change Log

20220225: (rysmario)

- Modded to work on a plain ESP32
- Slow down the servo to avoid pen hammering on the egg

20220506: (revnull-src)

- Added support for Wemos D1 "R32" Uno style clone for use with the popular Arduino CNC shield
- Fixed file name parsing when reading from SD card
- Added simple feature options via "#define" in the printer.h
- Modularized platform.ini build environments to account for the currently tested platforms
- Added WiFi networking debug statements to console output
- Setup config defaults on first flash.
