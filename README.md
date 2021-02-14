# esp8266-blockchain-tracing-with-display

Shows Real Time Price of Cryptocurrencies on several Screens

## Hardware

- Display 2004A (16x4)
- Converter i2c
- Bi-Directional Logic Level Converter (Display at 5v, i2c at 3v)
- Esp8266 NodeMcu V3
- Voltage Input 3V for esp8266 and 5V for display

## Libreries and dependencies

In my case I use vscodium with the package for platformio.

Before start, need add to IDE:

- arduino-libraries/WiFi
- arduino-libraries/ArduinoHttpClient@^0.4.0
- marcoschwartz/LiquidCrystal_I2C@^1.1.4
- bblanchon/ArduinoJson@^6.17.2
