#pragma once
#include <pebble.h>
#include <pebble-effect-layer/pebble-effect-layer.h>

void label_update_proc(Layer* layer, GContext* ctx);
void analog_update_proc(Layer* layer, GContext* ctx);
void apply_colors_to_layers();

void main_window_load(Window* window);
void main_window_unload(Window* window);