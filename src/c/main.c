#include "src/c/luminosity.h"
#include "src/c/callbacks.h"
#include "src/c/render.h"


void main_window_load(Window* window) {

  Layer* window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_unobstructed_bounds(window_layer);

  int w = window_bounds.size.w;
  int h = window_bounds.size.h;
  int targetAnalogWidth = w - 18;

  GRect tempRect = TEMP_RECT;
  GRect forecastHighLowRect = FORECAST_HIGHLOW_RECT;
  GRect timeRect = TIME_RECT;
  GRect stepRect = STEP_RECT;
  GRect analogRect = ANALOG_RECT;
  GRect conditionRect = CONDITION_RECT;
  GRect btRect = BT_RECT;
  GRect batteryRect = BAT_RECT;
  GRect dateRect;
  GTextAlignment dateAlign;
  if (settings.Analog) {
    dateRect = DATE_LAYER_ANALOG;
    dateAlign = DATE_ALIGN_ANALOG;
  } else {
    dateRect = DATE_LAYER_DIGITAL;
    dateAlign = DATE_ALIGN_DIGITAL;
  }

  s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_steps_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // Analog hands layer
  s_analog_layer = layer_create(analogRect);
  calculate_perimiter(s_analog_layer);
  layer_set_update_proc(s_analog_layer, analog_update_proc);

  // Conditions layer
  s_condition_icon_bitmap[0] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_CLEAR);
  s_condition_icon_bitmap[1] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_CLEAR_NIGHT);
  s_condition_icon_bitmap[2] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_RAIN);
  s_condition_icon_bitmap[3] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_SNOW);
  s_condition_icon_bitmap[4] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_SLEET);
  s_condition_icon_bitmap[5] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_WIND);
  s_condition_icon_bitmap[6] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_FOG);
  s_condition_icon_bitmap[7] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_CLOUDY);
  s_condition_icon_bitmap[8] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_PC);
  s_condition_icon_bitmap[9] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_PC_NIGHT);

  s_conditions_layer = bitmap_layer_create(conditionRect);
  layer_add_child(window_get_root_layer(window),
                  bitmap_layer_get_layer(s_conditions_layer));

  s_wind_bearing_icon = layer_create(conditionRect);
  layer_set_update_proc(s_wind_bearing_icon, bearing_icon_update_proc);
  layer_add_child(window_get_root_layer(window), s_wind_bearing_icon);

  s_conditions_layer_inverter = effect_layer_create(conditionRect);
  layer_add_child(window_get_root_layer(window),
                  effect_layer_get_layer(s_conditions_layer_inverter));

  // Create the Bluetooth icon GBitmap
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);

  // Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer = bitmap_layer_create(btRect);
  bitmap_layer_set_background_color(s_bt_icon_layer, COLOR_CLEAR);
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window),
                  bitmap_layer_get_layer(s_bt_icon_layer));

  s_bt_icon_layer_inverter = effect_layer_create(btRect);
  layer_add_child(window_get_root_layer(window),
                  effect_layer_get_layer(s_bt_icon_layer_inverter));

  // Weather forecast layer
  s_forecast_layer = layer_create(window_bounds);
  layer_set_update_proc(s_forecast_layer, forecast_update_proc);
  layer_add_child(window_get_root_layer(window), s_forecast_layer);

  // Create 24 hour layer
  s_24hour_layer = layer_create(window_bounds);
  layer_set_update_proc(s_24hour_layer, label_update_proc);
  layer_add_child(window_get_root_layer(window), s_24hour_layer);

  // Temperature layer
  s_temp_layer = text_layer_create(tempRect);
  text_layer_set_background_color(s_temp_layer, COLOR_CLEAR);
  text_layer_set_text(s_temp_layer, "");
  text_layer_set_font(s_temp_layer, s_date_font);
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_temp_layer));

  s_wind_speed_layer = text_layer_create(tempRect);
  text_layer_set_background_color(s_wind_speed_layer, COLOR_CLEAR);
  text_layer_set_text(s_wind_speed_layer, "");
  text_layer_set_font(s_wind_speed_layer, s_date_font);
  text_layer_set_text_alignment(s_wind_speed_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_wind_speed_layer));

  // forecast high/low text layer
  s_forecast_high_low_layer = text_layer_create(forecastHighLowRect);
  text_layer_set_background_color(s_forecast_high_low_layer, COLOR_CLEAR);
  text_layer_set_text(s_forecast_high_low_layer, "");
  text_layer_set_font(s_forecast_high_low_layer, s_date_font);
  text_layer_set_text_alignment(s_forecast_high_low_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_forecast_high_low_layer));

  // forecast high/low text layer
  s_wind_bearing_layer = text_layer_create(forecastHighLowRect);
  text_layer_set_background_color(s_wind_bearing_layer, COLOR_CLEAR);
  text_layer_set_text(s_wind_bearing_layer, "");
  text_layer_set_font(s_wind_bearing_layer, s_date_font);
  text_layer_set_text_alignment(s_wind_bearing_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_wind_bearing_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(timeRect);
  text_layer_set_background_color(s_time_layer, COLOR_CLEAR);
  text_layer_set_text(s_time_layer, "--:--");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Create date TextLayer
  s_date_layer = text_layer_create(dateRect);
  text_layer_set_background_color(s_date_layer, COLOR_CLEAR);
  text_layer_set_text_alignment(s_date_layer, dateAlign);
  text_layer_set_text(s_date_layer, "");
  text_layer_set_font(s_date_layer, s_date_font);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_date_layer));

  // Create battery meter Layer
  s_battery_text_layer = text_layer_create(batteryRect);
  text_layer_set_background_color(s_battery_text_layer, COLOR_CLEAR);
  text_layer_set_text(s_battery_text_layer, "");
  text_layer_set_font(s_battery_text_layer, s_steps_font);
  text_layer_set_text_alignment(s_battery_text_layer, GTextAlignmentLeft);

  // Steps layer
  s_steps_layer = text_layer_create(stepRect);
  text_layer_set_background_color(s_steps_layer, COLOR_CLEAR);
  text_layer_set_text(s_steps_layer, "");
  text_layer_set_font(s_steps_layer, s_steps_font);
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentRight);
#if defined(PBL_HEALTH)
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(s_steps_layer));
#endif

  // Things that are different if you're analog vs digital
  if (!settings.Analog)
    layer_add_child(window_get_root_layer(window),
                    text_layer_get_layer(s_time_layer));
  if (!settings.Analog)
    layer_add_child(window_get_root_layer(window),
                    text_layer_get_layer(s_battery_text_layer));
  if (settings.Analog)
    layer_add_child(window_get_root_layer(window), s_analog_layer);

  // Initialize the display
  apply_colors_to_layers();
  update_time_no_update_weather();
  battery_callback(battery_state_service_peek());
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  UnobstructedAreaHandlers handlers = {
      .will_change = prv_unobstructed_will_change,
      .did_change = prv_unobstructed_did_change};
  unobstructed_area_service_subscribe(handlers, NULL);
}

void main_window_unload(Window* window) {

  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_bt_icon_layer);

  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(s_temp_layer);
  text_layer_destroy(s_wind_speed_layer);
  text_layer_destroy(s_forecast_high_low_layer);
  text_layer_destroy(s_wind_bearing_layer);
  text_layer_destroy(s_battery_text_layer);
  layer_destroy(s_analog_layer);
  layer_destroy(s_forecast_layer);
  effect_layer_destroy(s_bt_icon_layer_inverter);
  effect_layer_destroy(s_conditions_layer_inverter);
}

void init() {
  prv_load_settings();

  s_space_ready = true;
  s_weather_ready = false;
  s_forecast_layer_displaying_wind = false;

  s_main_window = window_create();
  window_set_window_handlers(
      s_main_window,
      (WindowHandlers){.load = main_window_load, .unload = main_window_unload});
  window_stack_push(s_main_window, true);

  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);

#if defined(PBL_HEALTH)
  // Attempt to subscribe
  if (!health_service_events_subscribe(health_handler, NULL)) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
  health_handler(HealthEventSignificantUpdate, NULL);
#else
  APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
#endif

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  const int inbox_size = 350;
  const int outbox_size = 10;
  app_message_open(inbox_size, outbox_size);

  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = bluetooth_callback});
  // Subscribe to tap events
  accel_tap_service_subscribe(accel_tap_handler);
}

void deinit() {
  window_destroy(s_main_window);
  accel_tap_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
