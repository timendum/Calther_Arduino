/* config.h should be before Paperdink.h */
#include "config.h"
#include <Paperdink.h>
#include <icons.h>
#include <fonts.h>

#include "jdata.h"
#include "date_time.h"

/* Create paperdink object */
PAPERDINK_DEVICE Paperdink;

#define RETRY_SECONDS 60 * 60

void setup() {
  DEBUG.begin(115200);

  /* Initialize paperd.ink device */
  Paperdink.begin();
  int pin_id = get_GPIO_wake_up();
  switch (pin_id) {
    default:
      update_screen();
      break;
  }

  deep_sleep_timer_button_wakeup(0, BUTTON_1_PIN);
}

void update_screen() {
  /* Enable power to the display */
  Paperdink.enable_display();
  /* Clear the background */
  Paperdink.epd.fillScreen(GxEPD_WHITE);

  /* Connect to wifi network */
  if (Paperdink.connect_wifi(SSID, PASSWORD) < 0) {
    // Display error and retry
    DEBUG.println("Unable to connect to WiFi");
    Paperdink.epd.drawBitmap(370, 4, wifi_off_sml, wifi_off_sml_width,
                             wifi_off_sml_height, GxEPD_BLACK);
    Paperdink.epd.display();
    deep_sleep_timer_button_wakeup(RETRY_SECONDS, BUTTON_1_PIN);
    // retry in 10 minutes
    return;
  }

  if (UIJData.fetch_data() < 0) {
    // Display error and retry
    DEBUG.println("Unable to fetch my data");
    Paperdink.epd.drawBitmap(370, 4, wifi_off_sml, wifi_off_sml_width,
                             wifi_off_sml_height, GxEPD_BLACK);
    Paperdink.epd.display();
    deep_sleep_timer_button_wakeup(RETRY_SECONDS, BUTTON_1_PIN);
    return;
  }

  UIJData.display_list(Paperdink.epd, 5, 110 + 30, 6, 390);
  // Draw separator
  Paperdink.epd.fillRect(0, 110, 400, 2, GxEPD_BLACK);
  display_datetime();
  display_weather();
  UIJData.display_moon(Paperdink.epd, 310, 20);

  /* Send data to display for the update */
  Paperdink.epd.display();
}

/*
Method to print the GPIO that triggered the wakeup
*/
int get_GPIO_wake_up() {
  uint64_t GPIO_reason;
  int pin_id;
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      DEBUG.println("Wakeup caused by external signal using RTC_IO");
      return -wakeup_reason;
    case ESP_SLEEP_WAKEUP_EXT1:
      DEBUG.println("Wakeup caused by external signal using RTC_CNTL");
      GPIO_reason = esp_sleep_get_ext1_wakeup_status();
      if (GPIO_reason > 0) {
        pin_id = round(log(GPIO_reason) / log(2));
        DEBUG.printf("Button PIN that triggered the wake up: %d\n", pin_id);
        return pin_id;
      }
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      DEBUG.println("Wakeup caused by timer");
      return -wakeup_reason;
    default:
      DEBUG.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      return -wakeup_reason;
  }
  return 0;
}

// hex(2**14+2**27+2**4+2**2) 14=BUTTON_1_PIN, 27=BUTTON_2_PIN
#define BUTTON_PIN_BITMASK 0x8004014

int8_t deep_sleep_timer_buttons_wakeup(uint64_t sleep_time) {
  /* Sleep till update time.
   * Align updates to 12am so that date change aligns
   * with actual day change.
   */
  if (sleep_time < 1) {
    sleep_time = (86400 / (UPDATES_PER_DAY)) -
                 (((Paperdink_Date.mil_hour * 3600) +
                   (Paperdink_Date.min * 60) + (Paperdink_Date.sec)) %
                  (86400 / UPDATES_PER_DAY));
    sleep_time += 1;
  }
  DEBUG.println("Turning off everything");
  delay(3000);
  // Turn off everything
  Paperdink.disable_everything();

  esp_sleep_enable_timer_wakeup(sleep_time * S_TO_uS_FACTOR);
  esp_sleep_enable_ext1_wakeup((uint64_t)BUTTON_PIN_BITMASK,
                               ESP_EXT1_WAKEUP_ANY_HIGH);  // 1 = High, 0 = Low
  DEBUG.printf("Timer wakeup after %lld s or button", sleep_time);
  // Go to sleep
  esp_deep_sleep_start();

  return 0;
}

int8_t deep_sleep_timer_button_wakeup(uint64_t sleep_time, uint8_t gpio_num) {
  /* Sleep till update time.
   * Align updates to 12am so that date change aligns
   * with actual day change.
   */
  if (sleep_time < 1) {
    sleep_time = (86400 / (UPDATES_PER_DAY)) -
                 (((Paperdink_Date.mil_hour * 3600) +
                   (Paperdink_Date.min * 60) + (Paperdink_Date.sec)) %
                  (86400 / UPDATES_PER_DAY));
    sleep_time += 1;
  }
  DEBUG.println("Turning off everything");
  delay(3000);
  // Turn off everything
  Paperdink.disable_everything();

  esp_sleep_enable_timer_wakeup(sleep_time * S_TO_uS_FACTOR);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)gpio_num, 0);  // 1 = High, 0 = Low
  DEBUG.printf("Timer wakeup after %lld us or button on pin %d", sleep_time,
               gpio_num);
  // Go to sleep
  esp_deep_sleep_start();

  return 0;
}

void display_weather() {
  /* Fetch weather data*/

  /* Display weather information */
  UIJData.font = &Gobold_Thin9pt7b;
  UIJData.primary_color = GxEPD_BLACK;

  UIJData.display_minmax(Paperdink.epd, 10, 11);
  UIJData.display_weather_icon(Paperdink.epd, 12, 45, 90);
}

void display_datetime() {
  /* Display background */
  Paperdink.epd.fillRect(110, 0, 170 + 5, 110,
                         Paperdink.color);  // bottom colored rect

  /* Fetch date data */
  if (Paperdink_Date.fetch_data(TIME_ZONE) < 0) {
    DEBUG.println("Unable to fetch Date Time data");
  }

  /* Set colors for the calendar */
  Paperdink_Date.font = &PAPERDINK_FONT_LRG;
  Paperdink_Date.primary_color = GxEPD_WHITE;
  Paperdink_Date.secondary_color = GxEPD_BLACK;
  Paperdink_Date.display_day_date(Paperdink.epd, 110, 5, 170);
  Paperdink_Date.font = &PAPERDINK_FONT_SML;
  Paperdink_Date.primary_color = GxEPD_BLACK;
  Paperdink_Date.secondary_color = GxEPD_WHITE;
  Paperdink_Date.display_last_updated_time(Paperdink.epd, 400, 300, 100);
}

void loop() {}
