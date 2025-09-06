#include "src/c/luminosity.h"

bool update_buffers() {
  time_t temp = time(NULL);
  struct tm* tick_time = localtime(&temp);

  // Create a long-lived buffer, and show the time

  if (clock_is_24h_style()) {
    strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
  } else {
    strftime(time_buffer, sizeof(time_buffer), "%I:%M", tick_time);
  }
  text_layer_set_text(s_time_layer, time_buffer);

  if (settings.Analog)
    layer_mark_dirty(s_analog_layer);

  // Show the date

  if (settings.Analog)
    strftime(date_buffer, sizeof(date_buffer), "%a\n%b %e", tick_time);
  else
    strftime(date_buffer, sizeof(date_buffer), "%a %b %e", tick_time);
  text_layer_set_text(s_date_layer, date_buffer);
  APP_LOG(APP_LOG_LEVEL_INFO, "Updated time.");
  return tick_time->tm_min % 30 == 0;
}

// Triggered when the time has changed
void update_time() {
  if (update_buffers()) {
    send_message();
  }
}

void update_time_no_update_weather() { update_buffers(); }

void prv_update_display() {
  // hide and display things according to settings
  Layer* winrl = window_get_root_layer(window.w_main);
  GRect window_bounds = layer_get_unobstructed_bounds(winrl);
  int w = window_bounds.size.w;
  int h = window_bounds.size.h;

  if (settings.Analog) {
    layer_remove_from_parent(text_layer_get_layer(s_time_layer));
    layer_remove_from_parent(text_layer_get_layer(window.tl_battery));
    layer_add_child(winrl, s_analog_layer);
    layer_set_frame(text_layer_get_layer(s_date_layer), DATE_LAYER_ANALOG);
    text_layer_set_text_alignment(s_date_layer, DATE_ALIGN_ANALOG);
  } else {
    layer_add_child(winrl, text_layer_get_layer(s_time_layer));
    layer_add_child(winrl, text_layer_get_layer(window.tl_battery));
    layer_remove_from_parent(s_analog_layer);
    layer_set_frame(text_layer_get_layer(s_date_layer), DATE_LAYER_DIGITAL);
    text_layer_set_text_alignment(s_date_layer, DATE_ALIGN_DIGITAL);
  }

  if (s_forecast_layer_displaying_wind) {
    layer_add_child(winrl, text_layer_get_layer(s_wind_speed_layer));
    layer_add_child(winrl, text_layer_get_layer(s_wind_bearing_layer));
    layer_add_child(winrl, s_wind_bearing_icon);
    layer_remove_from_parent(text_layer_get_layer(window.tl_temperature));
    layer_remove_from_parent(text_layer_get_layer(s_forecast_high_low_layer));
    layer_remove_from_parent(bitmap_layer_get_layer(s_conditions_layer));
    layer_remove_from_parent(
        effect_layer_get_layer(s_conditions_layer_inverter));
  } else {
    layer_add_child(winrl, text_layer_get_layer(window.tl_temperature));
    layer_add_child(winrl, text_layer_get_layer(s_forecast_high_low_layer));
    layer_add_child(winrl, bitmap_layer_get_layer(s_conditions_layer));
    layer_add_child(winrl, effect_layer_get_layer(s_conditions_layer_inverter));
    layer_remove_from_parent(text_layer_get_layer(s_wind_speed_layer));
    layer_remove_from_parent(text_layer_get_layer(s_wind_bearing_layer));
    layer_remove_from_parent(s_wind_bearing_icon);
  }

  if (settings.Invert_Colors && s_weather_ready) {
    bitmap_layer_set_background_color(s_conditions_layer, COLOR_TIME);
  } else {
    bitmap_layer_set_background_color(s_conditions_layer, COLOR_CLEAR);
  }
  layer_mark_dirty(s_forecast_layer);
  layer_mark_dirty(s_wind_bearing_icon);
  layer_mark_dirty(text_layer_get_layer(window.tl_temperature));
  layer_mark_dirty(text_layer_get_layer(s_wind_speed_layer));
  layer_mark_dirty(text_layer_get_layer(s_forecast_high_low_layer));
  layer_mark_dirty(text_layer_get_layer(s_wind_bearing_layer));
  layer_mark_dirty(bitmap_layer_get_layer(s_conditions_layer));
  APP_LOG(APP_LOG_LEVEL_INFO, "Updated display.");
  update_time_no_update_weather();
}
