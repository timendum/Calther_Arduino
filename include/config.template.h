#ifndef CONFIG_H
#define CONFIG_H

/* CONFIGURATION
 * Uncomment only one of the below #define statements
 * based on the paperd.ink device you have
 */
#define PAPERDINK_DEVICE Paperdink_Classic
//#define PAPERDINK_DEVICE Paperdink_Merlot

#define DEBUG Serial

#define SSID     "xxxxxx" // Wifi Network SSID (name of wifi network)
#define PASSWORD "xxxxxx" // Wifi Network password

/* Time zone from list https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv */
#define TIME_ZONE "CET-1CEST,M3.5.0,M10.5.0/3"

/* Number of times to update starting 12am
 * 1 = Updates every 24 hours
 * 2 = Updates every 12 hours
 * 3 = Updates every 8 hours. Not a good idea since it won't align with day changes.
 * 4 = Updates every 6 hours
 * ... and so on
 * Higher number means lower battery life
 */
#define UPDATES_PER_DAY 4

/* My information */
#define MY_JSON_URL    "https://xxxxx/paperd.json" // City for weather

#endif /* CONFIG_H */