#include <pebble.h>
#include <pebble-effect-layer/pebble-effect-layer.h>

#include "src/c/callbacks.h"
#include "src/c/declarations.h"
#include "src/c/layout_constants.h"
#include "src/c/messaging.h"
#include "src/c/render.h"
#include "src/c/settings.h"

void init() {
  prv_load_settings();

  s_space_ready = true;
  s_weather_ready = false;
  forecast_ring_in_wind_mode = false;

  window.w_main = window_create();
  window_set_window_handlers(
      window.w_main,
      (WindowHandlers){.load = main_window_load, .unload = main_window_unload});
  window_stack_push(window.w_main, true);

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
  window_destroy(window.w_main);
  accel_tap_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
