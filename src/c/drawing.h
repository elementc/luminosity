#include "src/c/luminosity.h"
#pragma once

#ifdef PBL_RECT

GPoint get_edge_point_for_hour(int hour, int width, int height, int padding);

#else // round

int hr_to_a(int hour);

#endif

void calculate_perimeter(Layer* layer);

GPoint rayFrom(int tri, int radius);
