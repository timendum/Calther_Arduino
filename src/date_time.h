#ifndef _PAPERDINK_TIME_H
#define _PAPERDINK_TIME_H

#include "ui_base.h"

#define NTP_SERVER "pool.ntp.org"
#define CONFIG_TIME_RETRY_COUNT (5)

class PaperdinkUIDateClass : public PaperdinkUIBaseClass
{
    public:
        char wday[4];
        char month[4];
        uint8_t month_num;
        uint8_t mday;
        uint8_t mil_hour;
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        uint8_t day_offset; // 1st day of the month offset, Monday = 0
        int year;
        // Offset to change start week. 0=>Sun, 1=>Sat, 2=>Fri ... 6=>Mon
        uint8_t week_start_offset;

        int8_t fetch_data(const char *time_zone, uint8_t week_start_offset = 6);
        void display_day_date(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w);
        void display_last_updated_time(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w);
};

extern PaperdinkUIDateClass Paperdink_Date;

#endif /* _PAPERDINK_TIME_H */
