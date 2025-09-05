#include "src/c/luminosity.h"

// send an empty ping to get new weather data
void send_message() {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Sending a message!");
  // Begin dictionary
  DictionaryIterator* iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
// appmessage
void inbox_received_callback(DictionaryIterator* iterator, void* context) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "Inbox received message");
  bool need_weather_update = false;

  Tuple* cfg_analog_tuple = dict_find(iterator, MESSAGE_KEY_CFG_ANALOG);
  Tuple* cfg_celsius_tuple = dict_find(iterator, MESSAGE_KEY_CFG_CELSIUS);
  Tuple* cfg_invert_colors_tuple =
      dict_find(iterator, MESSAGE_KEY_CFG_INVERT_COLORS);
  Tuple* cfg_knots_tuple = dict_find(iterator, MESSAGE_KEY_CFG_KNOTS);
  Tuple* cfg_enable_wind_ring_tuple =
      dict_find(iterator, MESSAGE_KEY_CFG_ENABLE_WIND_RING);
  if (cfg_analog_tuple) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Inbox received settings.");
    bool new_Analog = cfg_analog_tuple->value->int32 == 1;
    bool new_Metric = cfg_celsius_tuple->value->int32 == 1;
    bool new_Knots = cfg_knots_tuple->value->int32 == 1;
    if (settings.Analog != new_Analog || settings.Metric != new_Metric ||
        settings.Knots != new_Knots)
      need_weather_update = true;
    settings.Analog = new_Analog;
    settings.Metric = new_Metric;
    settings.Invert_Colors = cfg_invert_colors_tuple->value->int32 == 1;
    settings.Knots = new_Knots;
    settings.enable_wind_ring = cfg_enable_wind_ring_tuple->value->int32 == 1;
    prv_save_settings();
  }

  // space data
  // Read tuples for data
  Tuple* sunrise_tuple = dict_find(iterator, MESSAGE_KEY_SPACE_SUNRISE);
  Tuple* sunset_tuple = dict_find(iterator, MESSAGE_KEY_SPACE_SUNSET);

  if (sunrise_tuple) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Inbox recieved space data.");
    s_sunset = sunset_tuple->value->int32;
    s_sunrise = sunrise_tuple->value->int32;
    settings.sunrise = s_sunrise;
    settings.sunset = s_sunset;
    s_space_ready = true;
    layer_mark_dirty(s_forecast_layer);
    prv_save_settings();
  }

  // Read tuples for data
  Tuple* temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple* forecast_high_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_HIGH);
  Tuple* forecast_low_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_LOW);
  Tuple* conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  Tuple* forecast_clouds_tuple =
      dict_find(iterator, MESSAGE_KEY_FORECAST_CLOUDS);
  Tuple* forecast_precip_type_tuple =
      dict_find(iterator, MESSAGE_KEY_FORECAST_PRECIP_TYPE);
  Tuple* forecast_precip_intensity_tuple =
      dict_find(iterator, MESSAGE_KEY_FORECAST_PRECIP_INTENSITY);
  Tuple* forecast_temp_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_TEMP);
  Tuple* forecast_wind_intensity_tuple =
      dict_find(iterator, MESSAGE_KEY_FORECAST_WIND_INTENSITY);
  Tuple* wind_speed_tuple = dict_find(iterator, MESSAGE_KEY_WIND_SPEED);
  Tuple* wind_bearing_tuple = dict_find(iterator, MESSAGE_KEY_WIND_BEARING);

  // If all data is available, use it
  if (temp_tuple) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Inbox received weather data");
    int reported_wind_speed = wind_speed_tuple->value->int32;
    windSpeed =
        (int)settings.Knots
            ? (settings.Metric ? 1.94384f * (double)reported_wind_speed
                               : 0.868976f * (double)reported_wind_speed)
            : reported_wind_speed;
    windBearing = (int)wind_bearing_tuple->value->int32;
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d˚",
             (int)temp_tuple->value->int32); // TODO units
    snprintf(wind_speed_buffer, sizeof(wind_speed_buffer), "%d%s", windSpeed,
             (settings.Knots ? "kts" : (settings.Metric ? "m/s" : "mph")));
    snprintf(wind_bearing_buffer, sizeof(wind_bearing_buffer), "%d˚ %s",
             windBearing, bearing_to_cardinal(windBearing));
    snprintf(forecast_high_low_buffer, sizeof(forecast_high_low_buffer),
             "%d˚/%d˚", (int)forecast_high_tuple->value->int32,
             (int)forecast_low_tuple->value->int32);
    snprintf(forecast_clouds_str, sizeof(forecast_clouds_str), "%s",
             forecast_clouds_tuple->value->cstring);
    snprintf(forecast_precip_type_str, sizeof(forecast_precip_type_str), "%s",
             forecast_precip_type_tuple->value->cstring);
    snprintf(forecast_precip_intensity_str,
             sizeof(forecast_precip_intensity_str), "%s",
             forecast_precip_intensity_tuple->value->cstring);
    snprintf(forecast_temp_str, sizeof(forecast_temp_str), "%s",
             forecast_temp_tuple->value->cstring);
    snprintf(forecast_wind_intensity_str, sizeof(forecast_wind_intensity_str),
             "%s", forecast_wind_intensity_tuple->value->cstring);
    text_layer_set_text(s_temp_layer, temperature_buffer);
    text_layer_set_text(s_forecast_high_low_layer, forecast_high_low_buffer);
    text_layer_set_text(s_wind_speed_layer, wind_speed_buffer);
    text_layer_set_text(s_wind_bearing_layer, wind_bearing_buffer);

    char* conditions = conditions_tuple->value->cstring;
    for (int i = 0; i < 10; i++) {
      if (strcmp(conditions, condition_icons[i]) == 0) {
        bitmap_layer_set_bitmap(s_conditions_layer, s_condition_icon_bitmap[i]);
        effect_layer_remove_effect(s_conditions_layer_inverter);
        if (settings.Invert_Colors) {
          effect_layer_add_effect(s_conditions_layer_inverter,
                                  effect_invert_bw_only, NULL);
        } else {
          effect_layer_remove_effect(s_conditions_layer_inverter);
        }
      }
    }
    s_weather_ready = true;
    need_weather_update = false;
    prv_update_display();
  }

  if ((cfg_analog_tuple && !temp_tuple) || need_weather_update) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Updated settings. Need new weather.");
    send_message();
  }
}

char reasonStr[20];
void getAppMessageResult(AppMessageResult reason) {
  switch (reason) {
  case APP_MSG_OK:
    snprintf(reasonStr, 20, "%s", "APP_MSG_OK");
    break;
  case APP_MSG_SEND_TIMEOUT:
    snprintf(reasonStr, 20, "%s", "SEND TIMEOUT");
    break;
  case APP_MSG_SEND_REJECTED:
    snprintf(reasonStr, 20, "%s", "SEND REJECTED");
    break;
  case APP_MSG_NOT_CONNECTED:
    snprintf(reasonStr, 20, "%s", "NOT CONNECTED");
    break;
  case APP_MSG_APP_NOT_RUNNING:
    snprintf(reasonStr, 20, "%s", "NOT RUNNING");
    break;
  case APP_MSG_INVALID_ARGS:
    snprintf(reasonStr, 20, "%s", "INVALID ARGS");
    break;
  case APP_MSG_BUSY:
    snprintf(reasonStr, 20, "%s", "BUSY");
    break;
  case APP_MSG_BUFFER_OVERFLOW:
    snprintf(reasonStr, 20, "%s", "BUFFER OVERFLOW");
    break;
  case APP_MSG_ALREADY_RELEASED:
    snprintf(reasonStr, 20, "%s", "ALRDY RELEASED");
    break;
  case APP_MSG_CALLBACK_ALREADY_REGISTERED:
    snprintf(reasonStr, 20, "%s", "CLB ALR REG");
    break;
  case APP_MSG_CALLBACK_NOT_REGISTERED:
    snprintf(reasonStr, 20, "%s", "CLB NOT REG");
    break;
  case APP_MSG_OUT_OF_MEMORY:
    snprintf(reasonStr, 20, "%s", "OUT OF MEM");
    break;
  case APP_MSG_CLOSED:
    snprintf(reasonStr, 20, "%s", "MSG CLOSED");
    break;
  case APP_MSG_INTERNAL_ERROR:
    snprintf(reasonStr, 20, "%s", "INTERNAL ERR");
    break;
  case APP_MSG_INVALID_STATE:
    snprintf(reasonStr, 20, "%s", "INVALID STATE");
    break;
  }
}

void inbox_dropped_callback(AppMessageResult reason, void* context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
  getAppMessageResult(reason);
  APP_LOG(APP_LOG_LEVEL_ERROR, reasonStr);
}

void outbox_failed_callback(DictionaryIterator* iterator,
                            AppMessageResult reason, void* context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator* iterator, void* context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
