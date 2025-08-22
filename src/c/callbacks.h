#pragma once
#include <pebble.h>
#include <pebble-effect-layer/pebble-effect-layer.h>

void bluetooth_callback(bool connected);
void battery_callback(BatteryChargeState state);
void health_handler(HealthEventType event, void* context);

