#ifndef CONFIG_H
#define CONFIG_H

// --- Wi-Fi Credentials ---
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASS = "WIFI_PASS";

// --- OpenWeatherMap Settings ---
// Zip code is generally more accurate than city name alone
// More information at: https://openweathermap.org/current

// For zip code queries
// ZZZZZ = Zip Code, CC = Country (e.g. TR, GB), ABCDEFGHIJKLMNOPQRSTUVWXYZ= = Your API Key
// String OPENWEATHER_URL = "https://api.openweathermap.org/data/2.5/weather?zip=ZZZZZ,CC&units=metric&appid=ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// For city queries
// AAA = City (e.g. Ankara, London), CC = Country (e.g. TR, GB), ABCDEFGHIJKLMNOPQRSTUVWXYZ= = Your API Key
String OPENWEATHER_URL = "https://api.openweathermap.org/data/2.5/weather?q=AAA,CC&units=metric&appid=ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// --- Pin Definitions (ESP32 -> Waveshare e-Paper Driver Hat) ---
#if defined(BOARD_ESP32_DEVKIT)
  // NodeMCU-ESP32S / ESP32-WROOM-32U
  #define EPD_CS    5
  #define EPD_DC    17
  #define EPD_RST   16
  #define EPD_BUSY  4
  #define SPI_SCK   18
  #define SPI_MISO  19
  #define SPI_MOSI  23

#elif defined(BOARD_XIAO_ESP32C3)
  // Seeed Studio XIAO ESP32-C3
  #define EPD_CS    20   // D7
  #define EPD_DC    21   // D6
  #define EPD_RST   3    // D1
  #define EPD_BUSY  2    // D0
  #define SPI_SCK   8    // D8
  #define SPI_MISO  9    // D9
  #define SPI_MOSI  10   // D10

#else
  #error "No board selected! Select a BOARD_ definition in the config.h file."
#endif

// --- Sleep Intervals (in microseconds) ---
// 4 Hours = 4 * 60 * 60 * 1,000,000
const uint64_t SLEEP_DURATION_OK = 14400000000ULL;
// 10 Minutes = 10 * 60 * 1,000,000
const uint64_t SLEEP_DURATION_ERROR = 600000000ULL;

#endif