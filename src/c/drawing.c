#include "src/c/luminosity.h"

#ifdef PBL_RECT

// for a given hour, and a given screen width and height, give the point at the screen's edge
// corresponding to the hour in the luminosity 24h circular frame, padded inwards by an amount.
GPoint get_edge_point_for_hour(int hour, int width, int height, int padding) {
  width -= padding * 2;
  height -= padding * 2;
  switch (hour % 24) {
  case 0:
    return GPoint(3 * width / 6 + padding, height + padding); // 0
  case 1:
    return GPoint(2 * width / 6 + padding, height + padding); // 1
  case 2:
    return GPoint(1 * width / 6 + padding, height + padding); // 2
  case 3:
    return GPoint(padding, 6 * height / 6 + padding); // 3
  case 4:
    return GPoint(padding, 5 * height / 6 + padding); // 4
  case 5:
    return GPoint(padding, 4 * height / 6 + padding); // 5
  case 6:
    return GPoint(padding, 3 * height / 6 + padding); // 6
  case 7:
    return GPoint(padding, 2 * height / 6 + padding); // 7
  case 8:
    return GPoint(padding, 1 * height / 6 + padding); // 8
  case 9:
    return GPoint(padding, padding); // 9
  case 10:
    return GPoint(1 * width / 6 + padding, padding); // 10
  case 11:
    return GPoint(2 * width / 6 + padding, padding); // 11
  case 12:
    return GPoint(3 * width / 6 + padding, padding); // 12
  case 13:
    return GPoint(4 * width / 6 + padding, padding); // 13
  case 14:
    return GPoint(5 * width / 6 + padding, padding); // 14
  case 15:
    return GPoint(width + padding, padding); // 15
  case 16:
    return GPoint(width + padding, 1 * height / 6 + padding); // 16
  case 17:
    return GPoint(width + padding, 2 * height / 6 + padding); // 17
  case 18:
    return GPoint(width + padding, 3 * height / 6 + padding); // 18
  case 19:
    return GPoint(width + padding, 4 * height / 6 + padding); // 19
  case 20:
    return GPoint(width + padding, 5 * height / 6 + padding); // 20
  case 21:
    return GPoint(width + padding, height + padding); // 21
  case 22:
    return GPoint(5 * width / 6 + padding, height + padding); // 22
  case 23:
  default:
    return GPoint(4 * width / 6 + padding, height + padding); // 23
  }
}

#else // round

// for round pebbles, figure out the angle for a particular location
int hr_to_a(int hour) { return DEG_TO_TRIGANGLE(180 + (15 * hour)); }

#endif

// perimeter calc
void calculate_perimeter(Layer* layer) {
  window.r_bounds = layer_get_bounds(layer);
  window.p_center = grect_center_point(&window.r_bounds);

  int perimeter = (window.r_bounds.size.w + window.r_bounds.size.h) * 2;
  step = perimeter / 60;
  int topcount = window.r_bounds.size.w / step;
  int sidecount = window.r_bounds.size.h / step;

  int halftop = topcount / 2;

  upperright = halftop;
  lowerright = halftop + sidecount;
  lowerleft = halftop + sidecount + topcount;
  upperleft = halftop + sidecount + topcount + sidecount;
}

// Analog hands drawing
GPoint rayFrom(int tri, int radius) {
  GPoint ray = {window.p_center.x + sin_lookup(tri) * radius / TRIG_MAX_RATIO,
                window.p_center.y - cos_lookup(tri) * radius / TRIG_MAX_RATIO};
  return ray;
}