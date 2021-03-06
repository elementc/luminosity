#include "src/c/luminosity.h"

// An instance of the struct
 ClaySettings settings;

/* Main */
 Window *s_main_window;

/* Battery */
 TextLayer *s_battery_text_layer;
 char s_battery_string[6];
 int s_battery_level;

/* Weather */
 TextLayer *s_temp_layer;
 TextLayer *s_forecast_high_low_layer;
 TextLayer *s_wind_speed_layer;
 TextLayer *s_wind_bearing_layer;
 char forecast_clouds_str[25];
 char forecast_precip_type_str[25];
 char forecast_precip_intensity_str[25];
 char forecast_temp_str[25];
 char forecast_wind_intensity_str[25];
 char wind_direction[6];
 char conditons_string[35];
 int temperature, tempHigh, tempLow, windSpeed, windBearing;
 bool s_weather_ready;
 Layer *s_forecast_layer;
 Layer *s_wind_bearing_icon;
 BitmapLayer *s_conditions_layer;
 EffectLayer *s_conditions_layer_inverter;
 bool s_forecast_layer_displaying_wind;

/* Time and date */
 TextLayer *s_time_layer, *s_date_layer;
 GFont s_time_font, s_date_font;
 Layer *s_analog_layer, *s_24hour_layer;

/* Steps */
 TextLayer *s_steps_layer;
 GFont s_steps_font;
 char steps_str[12];

/* Bluetooth */
 BitmapLayer *s_bt_icon_layer;
 EffectLayer *s_bt_icon_layer_inverter;
 GBitmap *s_bt_icon_bitmap;

/* Space */
 int s_sunset, s_sunrise;
 bool s_space_ready;

 char* condition_icons[] = {
    "clear-day", "clear-night", "rain", "snow", "sleet", "wind", "fog", "cloudy", "partly-cloudy-day", "partly-cloudy-night"
};

 GBitmap* s_condition_icon_bitmap[10];

 int upperright, lowerright, lowerleft, upperleft, step;
 GRect bounds;
 GPoint center;

 char date_buffer[16];
 char time_buffer[] = "00:00";
 char temperature_buffer[8];
 char forecast_high_low_buffer[16];
 char wind_speed_buffer[16];
 char wind_bearing_buffer[16];
