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
#define PIN_CS   5
#define PIN_DC   17
#define PIN_RST  16
#define PIN_BUSY 4

// --- Sleep Intervals (in microseconds) ---
// 4 Hours = 4 * 60 * 60 * 1,000,000
const uint64_t SLEEP_DURATION_OK = 14400000000ULL;
// 10 Minutes = 10 * 60 * 1,000,000
const uint64_t SLEEP_DURATION_ERROR = 600000000ULL;

#endif