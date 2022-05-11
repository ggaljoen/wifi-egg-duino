# Wi-Fi Egg Bot

[![Donate original author](https://img.shields.io/badge/Donate-PayPal-green.svg?style=flat-square&logo=paypal)](https://paypal.me/andreitatar)

Initially based on the M5stack core platform (ESP32), this project aims to provide a refresh to the original [Egg Bot](https://egg-bot.com/) giving it the ability to load/slice/save/print files without the use of a PC.

If you like the project, find it useful and want to support the original author, consider donating [PayPal](https://paypal.me/andreitatar).

## Demo

[![Demo Video](https://img.youtube.com/vi/auzUlywlYKI/0.jpg)](https://www.youtube.com/watch?v=auzUlywlYKI)

## Development work

### Remaining things for a first release

- [x] WiFi setup - start firmware (or switch if unable to connect to a network) in AP mode. Allow changing the mode/wi-fi settings from the web client
- [ ] M5 UI - update M5 code to include UI to: start/pause/stop print, view progress and Wi-Fi settings (AP settings and/or IP in STA mode)
- [ ] OTA - easier firmware update (removing dependencies for platformio or other tools after the first flash)

### Lower priority

- [ ] Move image processing in web worker
- [ ] Make project compatible with generic ESP32 boards

## Hardware Connections

- [M5Stack Core](doc/m5stack/README.md)
- [Wemos D1 R32 + CNC Shield](doc/esp32_r32_cnc/README.md)

## Building

### Web client (angular based)

Dependencies: [node.js](https://nodejs.org/en/)

```text
cd eggduino-client
npm install 
npm run build
```

### ESP32 code

Dependencies: [PlatformIO](https://platformio.org/platformio-ide)

I personally use [Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) extension.

```text
platformio run                             - build code
platformio run --target upload             - upload code
platformio run --target uploadfs           - upload web client files
```

## Contributors

- [Andrei Tatar](https://github.com/andrei-tatar/wifi-egg-duino): Original Author
- [Mario](https://github.com/rysmario/wifi-egg-duino): Generic ESP32 Adaptation
- [Ray Ramirez](https://github.com/revnull-src/wifi-egg-duino): Wemos D1 R32 Adaptation
