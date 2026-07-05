# 🎛️ Telegram RGB LED Controller

> A learning project for controlling an addressable LED with a Telegram bot, built on ESP32-S3.

Control a WS2812 LED remotely through a Telegram bot with an inline keyboard menu.

---

## About

This project was built for **learning purposes**, to demonstrate how to:

- Connect a Telegram bot to an ESP32 board
- Control an addressable LED using the `FastLED` library
- Build an interactive menu with Telegram's inline keyboard buttons
- Write non-blocking code using `millis()` instead of `delay()`
- Add a simple security layer by checking the `chat_id`

If you're just getting started with ESP32 and Telegram bots, this is a good starting point 🚀

---

## Features

| Command | Action |
|---|---|
| `/start` | Show the main menu with buttons |
| `/red` | Set LED color to red |
| `/green` | Set LED color to green |
| `/blue` | Set LED color to blue |
| `/white` | Set LED color to white |
| `/rainbow` | Run a rainbow color effect |
| `/fade` | Run a fade in/out effect |
| `/led_off` | Turn the LED off |
| `/status` | Show board status (WiFi, memory, uptime) |
| `/restart` | Restart the board |

---

## Hardware Required

- ESP32-S3 board
- One WS2812 addressable LED (connected to pin 48)
- WiFi connection

---

## Libraries Used

```
WiFi.h
WiFiClientSecure.h
UniversalTelegramBot
ArduinoJson
FastLED
```

---

## Setup

### 1. Clone the repo
```bash
git clone https://github.com/yazdan-iot/telegram-rgb-led-esp32.git
```

### 2. Create a Telegram bot
Talk to [@BotFather](https://t.me/BotFather) to create a new bot and get your token.

### 3. Configure your credentials
In `main.cpp`, fill in your own values:

```cpp
const char* WIFI_SSID     = "SSID";
const char* WIFI_PASSWORD = "PASSWORD";
const char* BOT_TOKEN     = "TOKEN";
```

> **Tip:** Leave `ALLOWED_CHAT_ID` empty at first, upload the code, send a message to your bot, then copy your chat ID from the Serial Monitor. This adds a security layer so only you can control the board.

### 4. Upload
Build and upload the code to your board using PlatformIO.

---

## Project Structure

```
learn-telegram-bot
├── include
├── lib
├── src
│   └── main.cpp
├── test
├── .gitignore
└── platformio.ini
```

---

## Security Notes

- Never commit your real bot token or WiFi password.
- The `chat_id` check ensures only you can control the board, even if someone else finds your bot token.
- The Telegram SSL certificate is accepted without strict validation (`setInsecure()`). Fine for learning, but consider certificate pinning for sensitive projects.

---

## Note

This is a learning project, not meant for production use. Feel free to fork it, modify it, and experiment.

---

## License

Free to use for learning purposes.
