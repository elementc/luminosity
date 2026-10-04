#include "src/c/luminosity.h"
#include <sys/param.h>

static bool is_night_hour(int hour) {
  return hour >= s_sunset || hour < s_sunrise;
}

static GColor sky_color(bool is_night, char clouds) {
  if (is_night) {
    if (clouds == '0')
      return COLOR_NIGHT;
    if (clouds == '1')
      return COLOR_NIGHT_PARTLY;
    return COLOR_NIGHT_CLOUDY;
  }
  if (clouds == '0')
    return COLOR_DAY;
  if (clouds == '1')
    return COLOR_DAY_PARTLY;
  return COLOR_DAY_CLOUDY;
}

static GColor wind_color(bool is_night) {
  return is_night ? COLOR_WIND_NIGHT : COLOR_WIND_DAY;
}

static bool precip_color(bool is_night, char type, GColor* out) {
  switch (type) {
  case 'r':
    *out = is_night ? COLOR_RAINY_NIGHT : COLOR_RAINY_DAY;
    return true;
  case 's':
    *out = is_night ? COLOR_SNOWY_NIGHT : COLOR_SNOWY_DAY;
    return true;
  case 'l':
    *out = is_night ? COLOR_SLEETY_NIGHT : COLOR_SLEETY_DAY;
    return true;
  default:
    return false;
  }
}

// Draw one hour's slice of a ring, `offset` pixels in from the edge and
// `thickness` pixels deep. Rects use filled rectangles, round screens use arcs.
static void draw_ring_segment(GContext* ctx, GRect bounds, int hour, int offset,
                              int thickness, GColor color) {
#ifdef PBL_RECT
  graphics_context_set_fill_color(ctx, color);
  GPoint p1 =
      get_edge_point_for_hour(hour, bounds.size.w, bounds.size.h, offset);
  GPoint p2 =
      get_edge_point_for_hour(hour + 1, bounds.size.w, bounds.size.h, offset);

  GRect segment;
  if (hour < 3 || hour >= 21) // bottom
    segment = GRect(p2.x, p1.y - thickness, p1.x - p2.x, thickness);
  else if (hour < 9) // left
    segment = GRect(p1.x, p1.y, thickness, p2.y - p1.y);
  else if (hour < 15) // top
    segment = GRect(p1.x, p1.y, p2.x - p1.x, thickness);
  else // right
    segment = GRect(p1.x - thickness, p1.y, thickness, p2.y - p1.y);

  graphics_fill_rect(ctx, segment, 0, GCornerNone);
#else
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, thickness);
  GRect ring = grect_crop(bounds, offset + thickness / 2);
  graphics_draw_arc(ctx, ring, GOvalScaleModeFitCircle, hr_to_a(hour),
                    hr_to_a(hour + 1));
#endif
}

// Draw forecast ring
void forecast_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);

  time_t now = time(NULL);
  struct tm* tick_time = localtime(&now);
  int hour = tick_time->tm_hour;
  int skip_stale_hour_count = 0;

  if (s_weather_ready && hour != weather_cache.weather_fetched_hour) {
    skip_stale_hour_count =
        MAX(MIN(abs(hour - weather_cache.weather_fetched_hour), 24), 0);

    if (skip_stale_hour_count != 0) {
      APP_LOG(APP_LOG_LEVEL_WARNING,
              "Rendering ring, but weather data is %d hour(s) stale. Currently "
              "%d, data from %d.",
              skip_stale_hour_count, hour, weather_cache.weather_fetched_hour);
    }
  }

  if (!s_space_ready) // if we have no space, we can do neither
    return;

  if (!s_weather_ready) { // with space only, draw just the sunrise/sunset ring
    for (int ring_hour = 0; ring_hour < 24; ring_hour++) {
      draw_ring_segment(ctx, bounds, ring_hour, 0, NO_WEATHER_SKY_LINE_WIDTH,
                        is_night_hour(ring_hour) ? COLOR_NIGHT : COLOR_DAY);
    }
    return;
  }

  // full set of rings
  // todo: was 2 4- skip_stale_hour_count, is not correct
  for (int ring_hour = 0; ring_hour < 24; ring_hour++) {
    int data_hour = ((24 - hour) + ring_hour) % 24;
    bool night = is_night_hour(ring_hour);

    // outer (light + temp + clouds) ring
    int sky_thickness =
        (weather_cache.forecast_temp[data_hour] - '0') + SKY_THICKNESS_BASE;
    draw_ring_segment(
        ctx, bounds, ring_hour, 0, sky_thickness,
        sky_color(night, weather_cache.forecast_clouds[data_hour]));

    // inner (precip or wind) ring
    if (forecast_ring_in_wind_mode) {
      int wind_thickness =
          (weather_cache.forecast_wind_intensity[data_hour] - '0') +
          WIND_THICKNESS_BASE;
      draw_ring_segment(ctx, bounds, ring_hour, sky_thickness, wind_thickness,
                        wind_color(night));
    } else {
      // c, r, s, p, _, ? = cloudy, rain, snow, partly cloudy, clear, unknown
      GColor precip;
      if (!precip_color(night, weather_cache.forecast_precip_type[data_hour],
                        &precip))
        continue; // don't draw clear segments!
      int precip_thickness =
          (weather_cache.forecast_precip_intensity[data_hour] - '0') +
          PRECIP_THICKNESS_BASE;
      draw_ring_segment(ctx, bounds, ring_hour, sky_thickness,
                        precip_thickness, precip);
    }
  }
}