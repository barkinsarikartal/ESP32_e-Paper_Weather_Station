/**
 * ESP32 e-Paper Weather Station
 * Hardware: ESP32-WROOM-32U + Waveshare 1.54" V2 (SSD1680)
 * Source: OpenWeatherMap API
 * Notes: Please do not forget to change WiFi Credentials and OpenWeather API URL from config.h
 */

#include <GxEPD2_3C.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include "config.h"
#include "bitmaps.h"

// Display Constructor for Waveshare 1.54" V2 (https://www.waveshare.com/1.54inch-e-paper-module-b.htm)
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY));

// Data Holders
String cityName, country, weatherDesc, iconCode;
float temp, feelsLike, tempMin, tempMax, windSpeed;
int humidity;
long dt, sunrise, sunset;
int timezone;
bool dataFetched = false;

// Function Prototypes
void fetchWeatherData();
void drawDashboardUI();
void drawLabelValue(int x, int y, String label, String value);
void drawOWMIcon(String code, int x, int y, int h, int w);
String getFormattedTime(long unixTime, int timezoneOffset);
String getFormattedDate(long unixTime, int timezoneOffset);

void setup() {
  Serial.begin(115200);
  
  // 1. Connect to WiFi
  // We try to connect. If it fails, we won't even turn on the display.
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int timeout = 0;
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  Serial.println();
  
  // 2. Try Fetching Data
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected. Fetching data...");
    fetchWeatherData();

    // Disconnect WiFi to save power immediately after fetching
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
  else {
    Serial.println("WiFi connection failed.");
  }

  // 3. Decision Logic
  uint64_t sleepTimer;
  
  if (dataFetched) {
    Serial.println("Data fetched successfully. Updating display.");

    display.init(115200);
    display.setRotation(1);

    drawDashboardUI();

    display.powerOff();

    sleepTimer = SLEEP_DURATION_OK; // 4 Hours
  }
  else {
    Serial.println("Failed to fetch data. Keeping old screen content.");
    Serial.println("Going to short sleep.");

    // The e-Paper will keep showing whatever was on it before (the old weather data).
    sleepTimer = SLEEP_DURATION_ERROR; // 10 Minutes
  }

  // 4. Enter Deep Sleep
  Serial.printf("Entering deep sleep for %llu microseconds...\n", sleepTimer);
  esp_sleep_enable_timer_wakeup(sleepTimer);
  esp_deep_sleep_start();
}

void loop() {
  //empty loop - unreachable
  Serial.println("if you are seeing this on serial monitor, something went wrong...");
  delay(1000);
}

void fetchWeatherData() {
  HTTPClient http;
  http.begin(OPENWEATHER_URL);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      cityName = doc["name"].as<String>();
      weatherDesc = doc["weather"][0]["description"].as<String>();
      iconCode = doc["weather"][0]["icon"].as<String>();
      country = doc["sys"]["country"].as<String>();
      
      temp = doc["main"]["temp"];
      feelsLike = doc["main"]["feels_like"];
      tempMin = doc["main"]["temp_min"];
      tempMax = doc["main"]["temp_max"];
      humidity = doc["main"]["humidity"];
      windSpeed = doc["wind"]["speed"];
      
      dt = doc["dt"];
      sunrise = doc["sys"]["sunrise"];
      sunset = doc["sys"]["sunset"];
      timezone = doc["timezone"];
      
      if (weatherDesc.length() > 0) {
        weatherDesc.setCharAt(0, toupper(weatherDesc.charAt(0)));
      }
      
      dataFetched = true;
    }
    else {
      Serial.print("JSON Error: "); Serial.println(error.c_str());
    }
  }
  else {
    Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void drawDashboardUI() {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    // Header
    display.fillRect(0, 0, 200, 25, GxEPD_RED);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(5, 18);
    display.print(cityName);

    String dateStr = getFormattedDate(dt, timezone);
    String timeStr = getFormattedTime(dt, timezone);
    String headerRight = dateStr + ", " + timeStr;

    int16_t x1, y1; uint16_t w, h;
    display.getTextBounds(headerRight, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(200 - w - 5, 18);
    display.print(headerRight);

    // Main Icon & Temp
    drawOWMIcon(iconCode, 18, 19, 75, 75);

    display.setTextColor(GxEPD_RED);
    display.setFont(&FreeSansBold18pt7b);
    String tempNum = String((int)temp);
    display.getTextBounds(tempNum, 0, 0, &x1, &y1, &w, &h);
    int centerX = 90 + (110 - w) / 2; 
    display.setCursor(centerX - 10, 70);
    display.print(tempNum);
    
    // Manual Degree Symbol and "C"
    int circleX = display.getCursorX() + 5;
    int circleY = 70 - 18;
    display.drawCircle(circleX, circleY, 4, GxEPD_RED);
    display.drawCircle(circleX, circleY, 3, GxEPD_RED);
    display.setCursor(circleX + 10, 70);
    display.print("C");

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(5, 100); 
    display.print(weatherDesc);

    // Grid
    display.fillRect(0, 105, 200, 2, GxEPD_BLACK);
    display.fillRect(0, 136, 200, 2, GxEPD_BLACK);
    display.fillRect(0, 168, 200, 2, GxEPD_BLACK);
    display.fillRect(100, 105, 2, 95, GxEPD_BLACK);

    display.setTextColor(GxEPD_BLACK);
    drawLabelValue(5, 108, "Min / Max", String((int)tempMin) + " / " + String((int)tempMax));
    drawLabelValue(105, 108, "Real Feel", String((int)feelsLike));
    drawLabelValue(5, 140, "Humidity", "%" + String(humidity));
    drawLabelValue(105, 140, "Wind", String(windSpeed, 1));
    drawLabelValue(5, 172, "Sunrise", getFormattedTime(sunrise, timezone));
    drawLabelValue(105, 172, "Sunset", getFormattedTime(sunset, timezone));
  }
  while (display.nextPage());
}

void drawLabelValue(int x, int y, String label, String value) {
  display.setFont(NULL);
  display.setCursor(x, y); display.print(label);
  display.setFont(&FreeSansBold9pt7b);
  int yValue = y + 22;
  display.setCursor(x, yValue); display.print(value);
  
  // Degree Symbol and "C" for Real Feel
  if(label == "Real Feel") {
    int cx = display.getCursorX() + 3;
    int cy = yValue - 10;
    display.drawCircle(cx, cy, 2, GxEPD_BLACK);
    display.setFont(NULL);
    display.setCursor(cx + 4, yValue - 7);
    display.print("C");
  }
  
  // Degree Symbol and "C" for Min/Max
  else if(label == "Min / Max") {
    int cx = display.getCursorX() + 3;
    int cy = yValue - 10;
    display.drawCircle(cx, cy, 2, GxEPD_BLACK);
    display.setFont(NULL);
    display.setCursor(cx + 4, yValue - 7);
    display.print("C");
  }

  // Unit for Wind
  else if(label == "Wind") {
    display.setFont(NULL);
    display.setCursor(display.getCursorX() + 2, yValue - 5);
    display.print("m/s");
  }
}

String getFormattedTime(long unixTime, int timezoneOffset) {
  time_t localTime = unixTime + timezoneOffset; 
  struct tm * ti = gmtime(&localTime);
  char buffer[6];
  sprintf(buffer, "%02d:%02d", ti->tm_hour, ti->tm_min);
  return String(buffer);
}

String getFormattedDate(long unixTime, int timezoneOffset) {
  time_t localTime = unixTime + timezoneOffset;
  struct tm * ti = gmtime(&localTime);
  char buffer[10];
  strftime(buffer, sizeof(buffer), "%d %b", ti);
  return String(buffer);
}

void drawOWMIcon(String code, int x, int y, int h, int w) { // based on https://openweathermap.org/weather-conditions
  bool isNight = (code.indexOf('n') != -1);
  String mainCode = code.substring(0, 2);

  if (mainCode == "01") { // Clear Sky
    if (isNight) display.drawBitmap(x, y, moon, h, w, GxEPD_BLACK);
    else display.drawBitmap(x, y, sun, h, w, GxEPD_RED);
  }
  else if (mainCode == "02" || mainCode == "03" || mainCode == "04") { // Few/Scattered/Broken Clouds
    if (!isNight){
      display.drawBitmap(x, y, clouds, h, w, GxEPD_BLACK);
      display.drawBitmap(x, y, sun_over_clouds, h, w, GxEPD_RED);
    }
    else {
      display.drawBitmap(x, y, clouds, h, w, GxEPD_BLACK);
      display.drawBitmap(x, y, moon_over_clouds, h, w, GxEPD_BLACK);
    }
  } 
  else if (mainCode == "09" || mainCode == "10") { // Rain
    display.drawBitmap(x, y, rainy, h, w, GxEPD_BLACK);
  } 
  else if (mainCode == "11") { // Thunderstorm
    display.drawBitmap(x, y, thunder, h, w, GxEPD_BLACK);
  } 
  else if (mainCode == "13") { // Snow
    display.drawBitmap(x, y, snow, 50, 50, GxEPD_BLACK);
  } 
  else if (mainCode == "50") { // Mist
    display.drawBitmap(x, y, fog, h, w, GxEPD_BLACK);
  }
  else { // Unknown
    display.drawCircle(x, y, 15, GxEPD_BLACK);
    display.setCursor(x-5, y+5); display.print("?");
  }
}