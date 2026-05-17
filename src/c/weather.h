#pragma once
#include "src/c/luminosity.h"
const char* bearing_to_cardinal(int bearing);
void bearing_icon_update_proc(Layer* layer, GContext* ctx);

void update_strings_from_weather_cache();
