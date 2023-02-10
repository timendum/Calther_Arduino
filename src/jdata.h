#ifndef _JDATA_H_
#define _JDATA_H_

#include "ui_base.h"

#define TASKS_MAX_ITEMS 30
#define TASKS_MAX_STR_LENGTH 50


class UIJDataClass : public PaperdinkUIClass {
  public:
    char tasks[TASKS_MAX_ITEMS][TASKS_MAX_STR_LENGTH + 1];
    uint8_t task_count = 0;
    char weather_string[100];
    char min_max_temp[10];

    int8_t fetch_data();
    void display_minmax(GxEPD2_GFX& display, uint16_t x, uint16_t y);
    void display_weather_icon(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w);
    void display_list(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t rows = TASKS_MAX_ITEMS, uint16_t max_line_width = 400);
};

extern UIJDataClass UIJData;

#endif /* _JDATA_H_ */
