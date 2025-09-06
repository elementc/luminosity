#include "src/c/luminosity.h"
#include "src/c/declarations.h"

ClaySettings settings;

WeatherCache weather_cache;

WindowStorage window;

StringTable strings;

/* Battery */
char s_battery_string[6];
int s_battery_level;

/* Weather */
char forecast_clouds_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
char forecast_precip_type_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
char forecast_precip_intensity_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
char forecast_temp_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
char forecast_wind_intensity_str[HOURLY_WEATHER_DATA_BUFFER_LEN];


int temperature, tempHigh, tempLow, windSpeed, windBearing;
bool s_weather_ready;
Layer* s_forecast_layer;
Layer* s_wind_bearing_icon;
bool s_forecast_layer_displaying_wind;

/* Time and date */
GFont s_time_font, s_date_font;

/* Steps */
GFont s_steps_font;
char steps_str[12];

/* Bluetooth */
GBitmap* s_bt_icon_bitmap;

/* Space */
int s_sunset, s_sunrise;
bool s_space_ready;

char* condition_icons[] = {"clear-day",
                           "clear-night",
                           "rain",
                           "snow",
                           "sleet",
                           "wind",
                           "fog",
                           "cloudy",
                           "partly-cloudy-day",
                           "partly-cloudy-night"};

GBitmap* s_condition_icon_bitmap[10];

int upperright, lowerright, lowerleft, upperleft, step;
GRect bounds;
GPoint center;

/* strings for display */
char date_buffer[] = "2999-99-99";
char time_buffer[] = "00:00";
char temperature_buffer[8];
char forecast_high_low_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
char wind_speed_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
char wind_bearing_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
