
#include "jdata.h"

#include <ArduinoJson.h>
#include <GxEPD2_GFX.h>
#include <fonts.h>
#include <icons.h>

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "config.h"

// Let's encrypt Root certificate
static const char* cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
    "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
    "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
    "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
    "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
    "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
    "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
    "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
    "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
    "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
    "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
    "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
    "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
    "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
    "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
    "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
    "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
    "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
    "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
    "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
    "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
    "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
    "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
    "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
    "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
    "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
    "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
    "-----END CERTIFICATE-----\n";

int8_t UIJDataClass::fetch_data() {
  int httpCode, ret = 0;

  WiFiClientSecure* client = new WiFiClientSecure;
  client->setCACert(cert);

  {
    HTTPClient https;
    https.begin(*client, MY_JSON_URL);
    https.addHeader("Content-Type", "application/json", 0, 0);

    httpCode = https.GET();

    if (httpCode == HTTP_CODE_OK) {  // Check for the return code
      // Parser updates weather_string directly
      DEBUG.printf("[HTTP] GET SUCCESS\r\n");
      DynamicJsonDocument json(35 * 1024);

      // Deserialize the JSON document
      // ReadLoggingStream can be used to debug the stream
      // ReadLoggingStream loggingStream(https.getStream(), Serial);
      // DeserializationError error = deserializeJson(json, loggingStream);
      DeserializationError error = deserializeJson(json, https.getStream());
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        ret = -1;
      } else {
        // convert it to a JsonObject
        JsonObject root = json.as<JsonObject>();
        const char* min_max = root["weather"]["min_max"].as<const char*>();
        if (strlen(min_max) < 5) {
          // small alignement
          snprintf(UIJData.min_max_temp, 10, " %s", min_max);
        } else {
          snprintf(UIJData.min_max_temp, 10, "%s", min_max);
        }
        snprintf(UIJData.weather_string, 100, "%s",
                 root["weather"]["txt"].as<const char*>());
        JsonArray array = root["recurring"].as<JsonArray>();
        for (int i = 0;
             i < array.size() && UIJData.task_count < TASKS_MAX_ITEMS; i++) {
          snprintf(UIJData.tasks[UIJData.task_count], TASKS_MAX_STR_LENGTH,
                   "- %s", array[i].as<const char*>());
          UIJData.task_count = UIJData.task_count + 1;
        }
        array = root["calendar"].as<JsonArray>();
        for (int i = 0;
             i < array.size() && UIJData.task_count < TASKS_MAX_ITEMS; i++) {
          snprintf(UIJData.tasks[UIJData.task_count], TASKS_MAX_STR_LENGTH,
                   "- %s", array[i].as<const char*>());
          UIJData.task_count = UIJData.task_count + 1;
        }
      }
    } else {
      DEBUG.printf("[HTTP] Failed, error: %s\r\n",
                   https.errorToString(httpCode).c_str());
      ret = -1;
    }

    https.end();
  }

  DEBUG.printf("[HTTP] COMPLETED \r\n");

  delete client;
  return ret;
}

void UIJDataClass::display_minmax(GxEPD2_GFX& display, uint16_t x, uint16_t y) {
  int16_t xt, yt;
  uint16_t wt, ht, prev_width = x, prev_height = y;

  display.setFont(font);
  display.setTextColor(primary_color);

  // Show min and max temperature
  display.drawBitmap(prev_width, prev_height, thermometer_sml,
                     thermometer_sml_width, thermometer_sml_height,
                     primary_color);
  prev_width += thermometer_sml_width;

  display.getTextBounds("77", 0, 0, &xt, &yt, &wt, &ht);
  display.setCursor(prev_width + 10, prev_height + ht + 2);
  display.print(UIJData.min_max_temp);
}

void UIJDataClass::display_weather_icon(GxEPD2_GFX& display,
                                        uint16_t x,
                                        uint16_t y,
                                        uint16_t w) {
  const uint8_t* bitmap;

  if (!strcmp(weather_string, "Drizzle")) {
    bitmap = drizzle_xlrg;
  } else if (!strcmp(UIJData.weather_string, "Thunderstorm")) {
    bitmap = lightning_xlrg;
  } else if (!strcmp(UIJData.weather_string, "Rain")) {
    bitmap = rain_xlrg;
  } else if (!strcmp(UIJData.weather_string, "Snow")) {
    bitmap = snow_xlrg;
  } else if (!strcmp(UIJData.weather_string, "Clouds")) {
    bitmap = cloud_xlrg;
  } else {
    bitmap = sun_xlrg;
  }

  display.drawBitmap(x + (w - alert_xlrg_width) / 2, y, bitmap,
                     alert_xlrg_width, alert_xlrg_height, primary_color);
}

void UIJDataClass::display_list(GxEPD2_GFX& display,
                                int16_t x,
                                int16_t y,
                                int16_t rows,
                                uint16_t max_line_width) {
  int16_t xt, yt;
  uint16_t wt, ht, char_ht, prev_height = y, prev_width = x;

  // display.setFont(&PAPERDINK_FONT_LRG);
  // display.setTextColor(primary_color);
  // display.setTextSize(1);

  // display.getTextBounds(F("Per oggi:"), 0, 0, &xt, &yt, &wt, &ht);
  // display.setCursor(prev_width + 5, prev_height + ht);
  // prev_height += ht;
  // prev_width += 5;
  // display.println(F("Per oggi:"));

  display.setFont(&PAPERDINK_FONT_MED);
  display.setTextColor(primary_color);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.getTextBounds("W", 0, 0, &xt, &yt, &wt, &char_ht);

  // Display tasks
  for (uint16_t i = 0; i < task_count && i < rows; i++) {
    String task = tasks[i];
    uint16_t cnt = task.length();
    uint16_t tlength = cnt;
    do {
      task = task.substring(0, cnt);
      display.getTextBounds(task.c_str(), 0, 0, &xt, &yt, &wt, &ht);
    } while (wt > max_line_width && cnt-- > 0);
    if (cnt >= tlength) {
      // It fits
      DEBUG.printf("Task on line (%d): %s\r\n", wt, task.c_str());
      display.setCursor(prev_width, prev_height);
      display.printf("%s", task.c_str());
      prev_height += char_ht + 7;
    } else {
      // Two rows
      DEBUG.printf("Task two line: %s\r\n", task.c_str());
      while (task[cnt] != ' ') {
        cnt--;
      }
      // First row
      task = task.substring(0, cnt);
      DEBUG.printf("first line: %s\r\n", task.c_str());
      display.setCursor(prev_width, prev_height);
      display.printf("%s", task.c_str());
      prev_height += char_ht + 7;
      // Remaining
      task = tasks[i];
      task = String(" " + task.substring(cnt));
      DEBUG.printf("remains : %s\r\n", task.c_str());
      cnt = task.length();
      do {
        task = task.substring(0, cnt);
        display.getTextBounds(task.c_str(), 0, 0, &xt, &yt, &wt, &ht);
      } while (wt > max_line_width && cnt-- > 0);
      display.setCursor(prev_width, prev_height);
      display.printf("%s", task.c_str());
      prev_height += char_ht + 7;
    }
  }
}

UIJDataClass UIJData;
