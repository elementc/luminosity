#include "src/c/callbacks.h"
#include "src/c/declarations.h"

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