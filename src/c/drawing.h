#include "src/c/luminosity.h"
#pragma once

#ifdef PBL_RECT

GPoint hours(int hour, int w, int h, int b);

#else //round

int hr_to_a(int hour);

#endif

void calculate_perimiter(Layer* layer);
