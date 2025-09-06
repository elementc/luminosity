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
  text_layer_set_text(window.tl_time, time_buffer);

  if (settings.Analog)
    layer_mark_dirty(window.l_analog);

  // Show the date

  if (settings.Analog)
    strftime(date_buffer, sizeof(date_buffer), "%a\n%b %e", tick_time);
  else
    strftime(date_buffer, sizeof(date_buffer), "%a %b %e", tick_time);
  text_layer_set_text(window.tl_date, date_buffer);
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
    layer_remove_from_parent(text_layer_get_layer(window.tl_time));
    layer_remove_from_parent(text_layer_get_layer(window.tl_battery));
    layer_add_child(winrl, window.l_analog);
    layer_set_frame(text_layer_get_layer(window.tl_date), DATE_LAYER_ANALOG);
    text_layer_set_text_alignment(window.tl_date, DATE_ALIGN_ANALOG);
  } else {
    layer_add_child(winrl, text_layer_get_layer(window.tl_time));
    layer_add_child(winrl, text_layer_get_layer(window.tl_battery));
    layer_remove_from_parent(window.l_analog);
    layer_set_frame(text_layer_get_layer(window.tl_date), DATE_LAYER_DIGITAL);
    text_layer_set_text_alignment(window.tl_date, DATE_ALIGN_DIGITAL);
  }

  if (forecast_ring_in_wind_mode) {
    layer_add_child(winrl, text_layer_get_layer(window.tl_wind_speed));
    layer_add_child(winrl, text_layer_get_layer(window.tl_wind_bearing));
    layer_add_child(winrl, window.l_wind_bearing_compass);
    layer_remove_from_parent(text_layer_get_layer(window.tl_current_temperature));
    layer_remove_from_parent(text_layer_get_layer(window.tl_high_low_forecast_temperature));
    layer_remove_from_parent(bitmap_layer_get_layer(window.bl_conditions));
    layer_remove_from_parent(
        effect_layer_get_layer(window.el_conditions_inverter));
  } else {
    layer_add_child(winrl, text_layer_get_layer(window.tl_current_temperature));
    layer_add_child(winrl, text_layer_get_layer(window.tl_high_low_forecast_temperature));
    layer_add_child(winrl, bitmap_layer_get_layer(window.bl_conditions));
    layer_add_child(winrl, effect_layer_get_layer(window.el_conditions_inverter));
    layer_remove_from_parent(text_layer_get_layer(window.tl_wind_speed));
    layer_remove_from_parent(text_layer_get_layer(window.tl_wind_bearing));
    layer_remove_from_parent(window.l_wind_bearing_compass);
  }

  if (settings.Invert_Colors && s_weather_ready) {
    bitmap_layer_set_background_color(window.bl_conditions, COLOR_TIME);
  } else {
    bitmap_layer_set_background_color(window.bl_conditions, COLOR_CLEAR);
  }
  layer_mark_dirty(window.l_forecast_ring);
  layer_mark_dirty(window.l_wind_bearing_compass);
  layer_mark_dirty(text_layer_get_layer(window.tl_current_temperature));
  layer_mark_dirty(text_layer_get_layer(window.tl_wind_speed));
  layer_mark_dirty(text_layer_get_layer(window.tl_high_low_forecast_temperature));
  layer_mark_dirty(text_layer_get_layer(window.tl_wind_bearing));
  layer_mark_dirty(bitmap_layer_get_layer(window.bl_conditions));
  APP_LOG(APP_LOG_LEVEL_INFO, "Updated display.");
  update_time_no_update_weather();
}
