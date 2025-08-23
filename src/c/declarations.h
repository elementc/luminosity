#pragma once
#include <pebble.h>

#define HOURLY_WEATHER_DATA_BUFFER_LEN 72

typedef struct ClaySettings {
  bool Analog;
  bool Metric;
  bool Knots;
  bool Invert_Colors;
  int sunrise;
  int sunset;
  bool enable_wind_ring;
} ClaySettings;

typedef struct WeatherCache{
  
} WeatherCache;

extern ClaySettings settings;
extern WeatherCache weather_cache;

/* Main */
extern Window* s_main_window;

/* Battery */
extern TextLayer* s_battery_text_layer;
extern char s_battery_string[6];
extern int s_battery_level;

/* Weather */
extern TextLayer* s_temp_layer;
extern TextLayer* s_forecast_high_low_layer;
extern TextLayer* s_wind_speed_layer;
extern TextLayer* s_wind_bearing_layer;
extern char forecast_clouds_str[25];
extern char forecast_precip_type_str[25];
extern char forecast_precip_intensity_str[25];
extern char forecast_temp_str[25];
extern char forecast_wind_intensity_str[25];
extern char wind_direction[6];
extern char conditons_string[35];
extern int temperature, tempHigh, tempLow, windSpeed, windBearing;
extern bool s_weather_ready;
extern Layer* s_forecast_layer;
extern Layer* s_wind_bearing_icon;
extern BitmapLayer* s_conditions_layer;
extern EffectLayer* s_conditions_layer_inverter;
extern bool s_forecast_layer_displaying_wind;

/* Time and date */
extern TextLayer *s_time_layer, *s_date_layer;
extern GFont s_time_font, s_date_font;
extern Layer *s_analog_layer, *s_24hour_layer;

/* Steps */
extern TextLayer* s_steps_layer;
extern GFont s_steps_font;
extern char steps_str[12];

/* Bluetooth */
extern BitmapLayer* s_bt_icon_layer;
extern GBitmap* s_bt_icon_bitmap;
extern EffectLayer* s_bt_icon_layer_inverter;

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
