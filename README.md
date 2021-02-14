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

## Prepare project

Copy file **src/api.cpp.example** to **src/api.cpp**.

```bash
cd esp8266-blockchain-tracing-with-display
cp src/api.cpp.example src/api.cpp
```

And set your values:

```c++
#define AP_NAME "AccesPointName"
#define AP_PASSWORD "Wireless_password"
#define API_DOMAIN "https://myweb.com"
#define API_PORT "443"
#define API_PATH "api/v1"
#define API_TOKEN_BEARER ""
```

No need bearer token, but need internet AP configuration and API for request data.

You can use external API for example API Binance or other public API.
