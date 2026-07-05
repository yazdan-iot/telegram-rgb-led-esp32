#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <FastLED.h>

// ---------- FastLED Settings for ESP32-S3 ----------
#define LED_PIN 48
#define NUM_LEDS 1
#define BRIGHTNESS 50
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// ---------- Fill in your own settings here ----------
const char* WIFI_SSID     = "SSID";
const char* WIFI_PASSWORD = "PASSWORD";
const char* BOT_TOKEN     = "TOKEN";

// Your personal chat ID. If you don't know it yet:
// Upload the code with this left empty, send any message to
// the bot, then copy your chat ID from the Serial Monitor
// and paste it here.
// This check is a security measure: without it, anyone who
// discovers your bot token could control your board.
String ALLOWED_CHAT_ID = "";

// ---------- Global variables ----------
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// We use millis() instead of delay() so the program never
// gets "blocked". This is the most important technique
// you'll need in every real-world project.
unsigned long lastCheckTime = 0;
const unsigned long CHECK_INTERVAL = 1000; // check for new messages every 1 second

bool ledState = false;

// ---------- RGB Funtions for LED ----------
void setSolidColor(CRGB color) {
  leds[0] = color;
  FastLED.show();
}

void rainbowEffect() {
  for(int i = 0; i < 256; i++) {
    leds[0] = CHSV(i, 255, 255); 
    FastLED.show();
    delay(10); 
  }
  FastLED.clear(); 
  FastLED.show();
}

void fadeEffect() {
  for(int i = 0; i <= 255; i++) {
    leds[0] = CRGB::Blue;
    leds[0].fadeLightBy(255 - i); 
    FastLED.show();
    delay(10);
  }
  for(int i = 255; i >= 0; i--) {
    leds[0] = CRGB::Blue;
    leds[0].fadeLightBy(255 - i);
    FastLED.show();
    delay(10);
  }
  FastLED.clear();
  FastLED.show();
}

// ---------- WiFi connection function ----------
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); // fine here, since the main loop hasn't started yet
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());
}

// ---------- Send the inline keyboard (glass buttons) ----------
void sendMainMenu(String chat_id) {
  // منوی جدید با دکمه‌های رنگی و افکتی
  String keyboardJson = "[[{\"text\":\"🔴 Red\",\"callback_data\":\"/red\"},"
                         "{\"text\":\"🟢 Green\",\"callback_data\":\"/green\"}],"
                         "[{\"text\":\"🔵 Blue\",\"callback_data\":\"/blue\"},"
                         "{\"text\":\"⚪ White\",\"callback_data\":\"/white\"}],"
                         "[{\"text\":\"🌈 Rainbow\",\"callback_data\":\"/rainbow\"},"
                         "{\"text\":\"💡 Fade\",\"callback_data\":\"/fade\"}],"
                         "[{\"text\":\"⬛ Turn OFF\",\"callback_data\":\"/led_off\"},"
                         "{\"text\":\"📊 Status\",\"callback_data\":\"/status\"}]]";

  bot.sendMessageWithInlineKeyboard(chat_id, "🎭 RGB Control Panel (FastLED):", "", keyboardJson);
}

// ---------- Build the status report ----------
String buildStatusReport() {
  String report = "Board Status:\n";
  report += "WiFi signal (RSSI): " + String(WiFi.RSSI()) + " dBm\n";
  report += "Free heap memory: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
  report += "Uptime: " + String(millis() / 1000) + " seconds\n";
  report += "LED state: " + String(ledState ? "ON" : "OFF");
  return report;
}

// ---------- Main command handler ----------
// Used both for plain text messages and for button clicks
void handleCommand(String text, String chat_id) {

  // Security check: only respond to the allowed chat_id
  if (ALLOWED_CHAT_ID != "" && chat_id != ALLOWED_CHAT_ID) {
    bot.sendMessage(chat_id, "Access denied.", "");
    return;
  }

  if (text == "/start") {
    sendMainMenu(chat_id);

  } else if (text == "/red") {
    setSolidColor(CRGB::Red);
    bot.sendMessage(chat_id, "LED Color: Red 🔴", "");

  } else if (text == "/green") {
    setSolidColor(CRGB::Green);
    bot.sendMessage(chat_id, "LED Color: Green 🟢", "");

  } else if (text == "/blue") {
    setSolidColor(CRGB::Blue);
    bot.sendMessage(chat_id, "LED Color: Blue 🔵", "");

  } else if (text == "/white") {
    setSolidColor(CRGB::White);
    bot.sendMessage(chat_id, "LED Color: White ⚪", "");

  } else if (text == "/rainbow") {
    bot.sendMessage(chat_id, "Running Rainbow... 🌈", "");
    rainbowEffect();
    bot.sendMessage(chat_id, "Rainbow finished.", "");

  } else if (text == "/fade") {
    bot.sendMessage(chat_id, "Running Fade Effect... 💡", "");
    fadeEffect();
    bot.sendMessage(chat_id, "Fade finished.", "");

  } else if (text == "/led_off") {
    FastLED.clear(); 
    FastLED.show();  
    ledState = false;
    bot.sendMessage(chat_id, "LED turned OFF. ⬛", "");

  } else if (text == "/status") {
    bot.sendMessage(chat_id, buildStatusReport(), "");

  } else if (text == "/restart") {
    bot.sendMessage(chat_id, "Restarting...", "");
    delay(1000);
    ESP.restart();

  } else {
    bot.sendMessage(chat_id, "Unknown command. Send /start to see the menu.", "");
  }
}

// ---------- Check for and process new messages ----------
void checkTelegramMessages() {
  int newMsgCount = bot.getUpdates(bot.last_message_received + 1);

  while (newMsgCount) {
    for (int i = 0; i < newMsgCount; i++) {
      String chat_id = bot.messages[i].chat_id;

      // If chat_id hasn't been set yet, print it to Serial so you can copy it
      if (ALLOWED_CHAT_ID == "") {
        Serial.println("Received chat ID: " + chat_id);
      }

      // A message can be either plain text or a button click (callback_query)
      String text = bot.messages[i].text;
      handleCommand(text, chat_id);
    }
    newMsgCount = bot.getUpdates(bot.last_message_received + 1);
  }
}

// ---------- setup ----------
void setup() {
  Serial.begin(115200);
  connectWiFi();

  Serial.print("Free Heap Before Telegram: ");
  Serial.println(ESP.getFreeHeap());

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  leds[0] = CRGB::White; 
  FastLED.show();
  delay(500);
  FastLED.clear();
  FastLED.show();


  // Allows the client to trust Telegram's SSL certificate without
  // strict validation. Fine for a learning project, but for a
  // sensitive project you should pin the actual certificate.
  secured_client.setInsecure();

  Serial.println("Bot is ready. Send it a message.");
}

// ---------- loop ----------
void loop() {
  // Check WiFi connection and reconnect if it drops
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    connectWiFi();
  }

  // Instead of delay(), we only check when it's actually time to.
  // This means the program is never fully "blocked".
  if (millis() - lastCheckTime > CHECK_INTERVAL) {
    checkTelegramMessages();
    lastCheckTime = millis();
  }

  // You can add other code here (e.g. reading a sensor).
  // Since we used millis(), there's room for other tasks too.
}