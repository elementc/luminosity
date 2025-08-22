#pragma once
#include <pebble.h>
#include <pebble-effect-layer/pebble-effect-layer.h>

void bluetooth_callback(bool connected);

void battery_callback(BatteryChargeState state);

void health_handler(HealthEventType event, void* context);

void prv_unobstructed_will_change(GRect final_unobstructed_screen_area,
                                  void* context);

void prv_unobstructed_did_change(void* context);

void tick_handler(struct tm* tick_time, TimeUnits units_changed);

void accel_tap_handler(AccelAxisType axis, int32_t direction);
