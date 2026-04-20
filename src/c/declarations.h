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
  char forecast_clouds[HOURLY_WEATHER_DATA_BUFFER_LEN];
  char forecast_precip_type[HOURLY_WEATHER_DATA_BUFFER_LEN];
  char forecast_precip_intensity[HOURLY_WEATHER_DATA_BUFFER_LEN];
  char forecast_temp[HOURLY_WEATHER_DATA_BUFFER_LEN];
  char forecast_wind_intensity[HOURLY_WEATHER_DATA_BUFFER_LEN];
  int temperature, temp_high, temp_low;

  // TODO: need reference time so we can index with old data.
} WeatherCache;
extern WeatherCache weather_cache;

typedef struct {
    /* Geometry */
    GRect r_bounds; /* This may only be used by the analog layer, actually, in the calculate_perimeter function, and therefore deletable if we delete that feature. */
    GPoint p_center;

    /* Window */
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

    /* Layers for rendering */
    Layer* l_analog;
    Layer* l_24hour;
    Layer* l_forecast_ring;
    Layer* l_wind_bearing_compass;
} WindowStorage;

extern WindowStorage window;

typedef struct {

} StringTable;
extern StringTable strings;


/* Battery */
extern char s_battery_string[6];
extern int s_battery_level;

/* Weather */
extern int windSpeed, windBearing;
extern bool s_weather_ready;
extern bool forecast_ring_in_wind_mode;

/* Time and date */
extern GFont s_time_font, s_date_font;

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

// Persistent storage key
#define SETTINGS_KEY 1
#define WEATHER_CACHE_KEY 2

extern char date_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char time_buffer[6];
extern char temperature_buffer[8];
extern char forecast_high_low_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char wind_speed_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
extern char wind_bearing_buffer[HOURLY_WEATHER_DATA_BUFFER_LEN];
