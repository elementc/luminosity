#include "src/c/callbacks.h"
#include "src/c/declarations.h"
#include "src/c/update.h"

// tick has occurred
void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Time tick occured.");
  update_time();
}

// TODO: add status icons for "no location" and "no server", use bt icon layer
// to indicate
//  Triggered when bluetooth connects/disconnects
void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);

  if (!connected) {
    vibes_double_pulse();
  }
  effect_layer_remove_effect(s_bt_icon_layer_inverter);
  if (!connected && settings.Invert_Colors) {
    effect_layer_add_effect(s_bt_icon_layer_inverter, effect_invert_bw_only,
                            NULL);
  }
}

// Triggered when the battery has changed level
void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;

  // Update meter
  layer_mark_dirty(s_analog_layer);

  // update text readout
  snprintf(s_battery_string, sizeof(s_battery_string), "%d%c", s_battery_level,
           '%');
  APP_LOG(APP_LOG_LEVEL_DEBUG, "battery updated");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", s_battery_string);
  text_layer_set_text(s_battery_text_layer, s_battery_string);
}

void health_handler(HealthEventType event, void* context) {
  // Which type of event occurred?
  switch (event) {
  case HealthEventSignificantUpdate:
  case HealthEventMovementUpdate: {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Health Event");

    HealthMetric metric = HealthMetricStepCount;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    HealthServiceAccessibilityMask mask =
        health_service_metric_accessible(metric, start, end);

    if (mask & HealthServiceAccessibilityMaskAvailable) {
      int steps = (int)health_service_sum_today(metric);

      if (steps > 999) {
        snprintf(steps_str, 12, "%d,%03d", steps / 1000, steps % 1000);
      } else {
        snprintf(steps_str, 12, "%d", steps);
      }
      text_layer_set_text(s_steps_layer, steps_str);

      APP_LOG(APP_LOG_LEVEL_DEBUG, "steps: %s ", steps_str);
    } else {
      text_layer_set_text(s_steps_layer, "");
      APP_LOG(APP_LOG_LEVEL_DEBUG, "steps unavailable");
    }

  } break;
  default:
    break;
  }
}

void prv_unobstructed_will_change(GRect final_unobstructed_screen_area,
                                  void* context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(window_get_root_layer(window.w_main));
  if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
    // Screen is about to become obstructed, hide the date
    layer_set_hidden(text_layer_get_layer(s_date_layer), true);
  }
}

void prv_unobstructed_did_change(void* context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(window_get_root_layer(window.w_main));
  // Get the total available screen real-estate
  GRect bounds =
      layer_get_unobstructed_bounds(window_get_root_layer(window.w_main));
  if (grect_equal(&full_bounds, &bounds)) {
    // Screen is no longer obstructed, show the date
    layer_set_hidden(text_layer_get_layer(s_date_layer), false);
  }
  prv_update_display();
}


time_t last_tap_seconds = 0;
uint16_t last_tap_ms = 0;
// wait 500ms between toggles :P
void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  if (settings.enable_wind_ring) {
    time_t this_seconds;
    uint16_t this_ms;
    time_ms(&this_seconds, &this_ms);
    APP_LOG(APP_LOG_LEVEL_INFO, "tap occured %d s %d ms later.",
            (int)this_seconds - (int)last_tap_seconds, this_ms - last_tap_ms);

    if (this_seconds == last_tap_seconds) {
      if (this_ms - last_tap_ms > 750) {
        last_tap_seconds = this_seconds;
        last_tap_ms = this_ms;
      } else {
        return;
      }
    } else if (this_seconds - last_tap_seconds == 1) {
      if (this_ms + 1000 - last_tap_ms > 750) {
        last_tap_seconds = this_seconds;
        last_tap_ms = this_ms;
      } else {
        return;
      }
    } else {
      last_tap_seconds = this_seconds;
      last_tap_ms = this_ms;
    }

    // A tap event occured
    s_forecast_layer_displaying_wind = !s_forecast_layer_displaying_wind;
    prv_update_display();
  }
}