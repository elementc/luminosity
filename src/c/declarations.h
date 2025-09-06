#pragma once
#include <pebble.h>

#define HOURLY_WEATHER_DATA_BUFFER_LEN 72

typedef struct {
  bool Analog;
  bool Metric;
  bool Knots;
  bool Invert_Colors;
  int sunrise;
  int sunset;
  bool enable_wind_ring;
} ClaySettings;
extern ClaySettings settings;

typedef struct {
  
} WeatherCache;
extern WeatherCache weather_cache;

typedef struct {
    Window* w_main;

    /* Text Layers */
    TextLayer* tl_battery;
    TextLayer* tl_current_temperature;
    TextLayer* tl_high_low_forecast_temperature;
    TextLayer* tl_wind_speed;
    TextLayer* tl_wind_bearing;
    TextLayer* tl_time;
    TextLayer* tl_date;
    TextLayer* tl_steps;

    /* Bitmap Layers */
    BitmapLayer* bl_conditions;
    BitmapLayer* bl_bt;

    /* Effect Layers */
    EffectLayer* el_conditions_inverter;
    EffectLayer* el_bt_inverter;
} WindowStorage;

extern WindowStorage window;

typedef struct {

} StringTable;
extern StringTable strings;


/* Battery */
extern char s_battery_string[6];
extern int s_battery_level;

/* Weather */
extern char forecast_clouds_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char forecast_precip_type_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char forecast_precip_intensity_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char forecast_temp_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char forecast_wind_intensity_str[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern int temperature, tempHigh, tempLow, windSpeed, windBearing;
extern bool s_weather_ready;
extern Layer* s_forecast_layer;
extern Layer* s_wind_bearing_icon;
extern bool s_forecast_layer_displaying_wind;

/* Time and date */
extern GFont s_time_font, s_date_font;
extern Layer *s_analog_layer, *s_24hour_layer;

/* Steps */
extern GFont s_steps_font;
extern char steps_str[12];

/* Bluetooth */
extern GBitmap* s_bt_icon_bitmap;

/* Space */
extern int s_sunset, s_sunrise;
extern bool s_space_ready;

extern char* condition_icons[];

extern GBitmap* s_condition_icon_bitmap[10];

extern int upperright, lowerright, lowerleft, upperleft, step;
extern GRect bounds;
extern GPoint center;

// Persistent storage key
#define SETTINGS_KEY 1
#define WEATHER_CACHE_KEY 2

extern char date_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char time_buffer[6];
extern char temperature_buffer[8];
extern char forecast_high_low_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char wind_speed_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char wind_bearing_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
